"""Record both battle-music sections from the native ROM."""
from pathlib import Path
import array
import hashlib
import json
import math
import os
import re
import subprocess
import wave

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'tests/native_artifacts/combat-feedback-audio'
OUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)

def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)

MAIN, PRESENTATION = map(symbol, ['pkmn_debug', 'pkmn_presentation_debug'])
driver = os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver'))
errors = (OUT / 'errors.log').open('w')
process = subprocess.Popen([driver, str(ROM)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                           stderr=errors, text=True, bufsize=1)
checks = peak_cpu = missed = 0

def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    answer = process.stdout.readline().strip()
    if not answer:
        raise RuntimeError('Emulator stopped: ' + (OUT / 'errors.log').read_text()[-2000:])
    return answer

def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))

def frames(count=1, keys=0):
    global peak_cpu, missed
    command(f'f {count} {keys}')
    state = read()
    peak_cpu = max(peak_cpu, state[21])
    missed = max(missed, state[22])
    return state

def tap(keys):
    frames(1, keys)
    frames(3)

def check(value, description):
    global checks
    checks += 1
    if not value:
        raise AssertionError(description + ' state=' + str(read()) + ' audio=' + str(read(PRESENTATION)))

def record(name):
    path = OUT / (name + '.wav')
    command('o ' + str(path))
    positions = set()
    for _ in range(8):
        frames(120)
        positions.add(read(PRESENTATION)[22])
    command('c')
    with wave.open(str(path)) as recording:
        check(recording.getnchannels() == 2 and recording.getframerate() == 32768, 'Native stereo recording')
        samples = array.array('h', recording.readframes(recording.getnframes()))
    check(len(samples) > 1000, 'Recording contains samples')
    rms = math.sqrt(sum(value * value for value in samples) / len(samples))
    clipped = sum(abs(value) >= 32760 for value in samples)
    check(rms > 10 and clipped == 0, 'Battle music is audible and does not clip')
    return dict(positions=sorted(positions), rms=round(rms, 2), peak=max(abs(value) for value in samples))

try:
    frames(90)
    tap(512)
    check(read()[1] == 11, 'Title audio settings open')
    tap(128)
    tap(1)
    check(read(PRESENTATION)[1:3] == [1, 0], 'Music stays on while sound effects are muted')
    tap(2)
    tap(2)
    check(read()[1] == 9, 'Practice opens from the title')
    tap(1)
    frames(185)
    check(read()[1] == 3 and read(PRESENTATION)[25] == 2, 'Native battle track starts')
    normal = record('normal')
    check(normal['positions'] == [0, 1] and read(PRESENTATION)[31] == 0, 'Normal section loops its two patterns')

    for _ in range(24):
        state = read()
        maximum = json.loads((ROOT / 'config/balance.json').read_text())['characters'][state[3]]['max_hp']
        if 0 < state[7] * 4 <= maximum:
            break
        before = state[7]
        frames(1, 2)
        for _ in range(75):
            frames()
            if read()[7] < before:
                break
        frames(60)
    state = read()
    check(0 < state[7] * 4 <= maximum, 'Real hits put the rival at or below one-quarter health')
    check(read(PRESENTATION)[31] == 1, 'Low health switches the finale on')
    finale = record('finale')
    check(finale['positions'] == [2, 3], 'Finale loops its two more intense patterns')
    check(normal['rms'] != finale['rms'], 'The recorded sections have different mixes')
    check(missed == 0, 'No missed native display frames')
finally:
    if process.poll() is None:
        command('c')
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()

result = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
              normal=normal, finale=finale, independent_music_and_sound=True,
              peak_cpu_fraction=peak_cpu / 4096, missed_display_frames=missed,
              method='mGBA 0.10.2, real GBA controls, read-only telemetry and native stereo recording.')
(OUT / 'report.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
