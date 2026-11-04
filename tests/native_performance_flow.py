"""Records frame budget, native values and SRAM round trips for the v1 candidate."""
from pathlib import Path
from PIL import Image
import hashlib
import json
import os
import re
import struct
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/native_artifacts/performance'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
SAVE = OUTPUT / 'records.sav'
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, DEPTH, ARCADE, MASTERY, AUDIO = map(symbol, [
    'pkmn_debug', 'pkmn_depth_debug', 'pkmn_arcade_debug', 'pkmn_mastery_debug', 'pkmn_presentation_debug'])
FONT = Image.open(ROOT / 'docs/pkfont.png').convert('RGB')
process = None
checks = peak = missed = records_peak = 0
switches = entries = 0
started = time.time()
samples = []


def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    result = process.stdout.readline().strip()
    if not result:
        raise RuntimeError('Emulator stopped; see ' + str(OUTPUT / 'errors.log'))
    return result


def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))


def frames(count=1, keys=0):
    global peak, missed, records_peak
    for _ in range(count):
        before = read()
        command(f'f 1 {keys}')
        state = read()
        usage = read(DEPTH)[20]
        peak = max(peak, state[21])
        missed = max(missed, state[22])
        if before[1] == 14 and state[1] == 14:
            records_peak = max(records_peak, usage)


def tap(keys):
    frames(1, keys)
    frames(3)


def capture(name):
    path = OUTPUT / (name + '.ppm')
    command('d ' + str(path))
    picture = Image.open(path).convert('RGB')
    picture.save(OUTPUT / (name + '.png'))
    return picture


def check(value, message):
    global checks
    checks += 1
    if not value:
        capture('failure')
        raise AssertionError(message + ' state=' + str(read()) + ' record=' + str(read(ARCADE)[8:12]))


def mask(picture):
    pixels = picture.tobytes()
    return bytes(255 if min(pixels[i:i + 3]) >= 180 else 0 for i in range(0, len(pixels), 3))


def right_text(picture, value, y):
    expected = Image.new('RGB', (len(value) * 8, 8))
    for index, char in enumerate(value):
        if char != ' ':
            glyph = ord(char) - 33
            expected.paste(FONT.crop((0, glyph * 8, 8, glyph * 8 + 8)), (index * 8, 0))
    x = 224 - expected.width
    target = mask(expected)
    matches = any(mask(picture.crop((x + dx, top, x + dx + expected.width, top + 8))) == target
                  for dx in (-1, 0, 1) for top in range(y - 5, y + 1))
    check(matches, 'Actual native value ' + value + ' fits its Records row')
    check(not any(mask(picture.crop((232, y - 4, 240, y + 4)))), 'Records value respects the side margin')


def fixture():
    """Prepare bounded schema-4 values offline; all navigation uses GBA keys."""
    values = []
    words = [0] * 149
    words[:4] = [0x504b4d38, 4, 10, 3]
    for index in range(39):
        record = [100 + index, index + 1, (1000 + index * 23) * 60]
        if index == 0:
            record = [999999, 999999, 0xffffffff]
        elif index == 38:
            record = [0, 0, 0]
        words[4 + index * 3:7 + index * 3] = record
        values.append(record)
    for fighter in range(13):
        words[121 + fighter * 2:123 + fighter * 2] = [min(fighter,10), fighter % 5]
    payload = struct.pack('<148I', *words[:-1])
    checksum = 2166136261
    for byte in payload:
        checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
    data = bytearray(32768)
    data[1024:1620] = payload + struct.pack('<I', checksum)
    SAVE.write_bytes(data)
    return values


def boot():
    global process
    process = subprocess.Popen(
        [os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM), str(SAVE)],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=(OUTPUT / 'errors.log').open('a'),
        text=True, bufsize=1)
    frames(90)
    check(read()[1] == 0, 'Boot title')


def close():
    global process
    if process is not None and process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
        assert process.returncode == 0
    process = None


def choose(mode):
    if read()[1] == 0:
        tap(8)
    check(read()[1] == 15, 'Visible mode menu')
    for _ in range(9):
        if read(ARCADE)[17] == mode:
            break
        tap(128)
    tap(1)


def records_pass(values, muted):
    global entries, switches
    choose(3)
    check(read()[1] == 14, 'Records opens directly')
    for level in range(3):
        for _ in range(3):
            if read()[4] == level:
                break
            tap(128)
        for fighter in range(13):
            for _ in range(13):
                if read()[2] == fighter:
                    break
                tap(16)
            expected = values[fighter * 3 + level]
            check(read(ARCADE)[8:11] == expected, 'Selected fighter/difficulty has the correct record')
            frames(4)
            picture = capture(('muted-' if muted else 'audio-') + str(fighter) + '-' + str(level))
            right_text(picture, str(expected[0]), 80)
            right_text(picture, str(expected[1]), 94)
            right_text(picture, str(expected[2] // 60) + ' SEC' if expected[2] else '--', 108)
            entries += 1
            tap(1)
            check(read()[1] == 16 and read()[2] == fighter, 'A opens Mastery for the same fighter')
            check(read(MASTERY)[4:6] == [min(fighter,10), fighter % 5], 'Mastery snapshot remains intact')
            tap(1)
            check(read()[1] == 14 and read()[4] == level, 'A returns to the same Records entry')
    for _ in range(130):
        previous = read()[2]
        frames(1, 16)
        check(read()[2] == (previous + 1) % 13, 'A one-frame direction changes fighter immediately')
        frames(1)
        check(read(ARCADE)[8:11] == values[read()[2] * 3 + read()[4]], 'Rapid navigation never shows another entry')
        switches += 1
    samples.append(dict(muted=muted,records_peak_cpu_fraction=records_peak / 4096,journey_peak_cpu_fraction=peak / 4096))
    tap(2)
    check(read()[1] == 15, 'Back returns to the mode menu')


try:
    values = fixture()
    boot()
    records_pass(values, False)
    choose(4)
    check(read()[1] == 11, 'Audio options open')
    tap(1)
    tap(128)
    tap(1)
    tap(2)
    check(read(AUDIO)[1:3] == [0, 0] and not read(ARCADE)[11], 'Audio changes save successfully')
    close()
    boot()
    check(read(AUDIO)[1:3] == [0, 0], 'Power cycle retains audio preferences')
    records_pass(values, True)
    check(records_peak / 4096 <= .9, 'Rapid Records redraws stay below 90 percent of a display frame')
    check(peak / 4096 <= .9, 'Records/Mastery/audio journey stays below 90 percent')
    check(missed == 0, 'Zero missed display frames')
    report = dict(status='passed', checks=checks, records_entries=entries, rapid_switches=switches,
                  all_fighters=True, all_difficulties=True, maximum_counters=True, maximum_time=True,
                  native_value_glyph_masks=True, mastery_round_trips=True, audio_on_off=True,
                  sram_power_cycle=True, retained_records=True, frame_budget=.9, samples=samples,
                  records_peak_cpu_fraction=records_peak / 4096, peak_cpu_fraction=peak / 4096,
                  missed_display_frames=missed, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
                  wall_seconds=round(time.time() - started, 2),
                  method='mGBA 0.10.2 real GBA keys, per-frame read-only telemetry and exact framebuffer glyph masks; bounded schema-4 SRAM fixture prepared offline before boot, audio save and actual SRAM restart; no gameplay-memory writes')
    (OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
finally:
    close()
