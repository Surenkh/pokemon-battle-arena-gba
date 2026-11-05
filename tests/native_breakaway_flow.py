"""Verify Breakaway in the GBA Practice drill and both Local Versus seats."""
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'tests/native_artifacts/breakaway'
OUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
ADAPTER = ROOT / 'tools/local_versus.lua'
SAVE = OUT / 'progress.sav'
shutil.copyfile(ROOT / 'tests/fixtures/nine_fighter_v1.sav', SAVE)
original = SAVE.read_bytes()
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)

def address(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)

MAIN, FEEL, PRACTICE, DEPTH, ARCADE, VERSUS = map(address, [
    'pkmn_debug', 'pkmn_feel_debug', 'pkmn_practice_debug',
    'pkmn_depth_debug', 'pkmn_arcade_debug', 'pkmn_versus_debug',
])
A, B, SELECT, START, RIGHT, LEFT, DOWN, R, L = 1, 2, 4, 8, 16, 32, 128, 256, 512
checks = peak = missed = 0
cases = []
errors = (OUT / 'errors.log').open('w')
driver = os.environ.get('PKMN_VERSUS_DRIVER', str(ROOT / '.tooling/mgba_versus_driver'))
process = subprocess.Popen([driver, str(ROM), str(ADAPTER), str(SAVE)], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)

def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    response = process.stdout.readline().strip()
    if not response:
        raise RuntimeError('Emulator stopped: ' + (OUT / 'errors.log').read_text()[-1200:])
    return response

def read(ptr=MAIN):
    return list(map(int, command(f's {ptr:08x}').split()[1:]))

def frames(n=1, left=0, right=0):
    global peak, missed
    command(f'f {n} {left} {right}')
    state = read()
    peak = max(peak, state[21])
    missed = max(missed, state[22])
    return state

def buttons(who, mask, n=1):
    return frames(n, mask if who == 0 else 0, mask if who == 1 else 0)

def tap(key, who=0):
    buttons(who, key)
    frames(3)

def capture(name):
    raw = OUT / (name + '.ppm')
    command('d ' + str(raw))
    image = Image.open(raw).convert('RGB')
    image.save(OUT / (name + '.png'))
    return image

def check(condition, message):
    global checks
    checks += 1
    if not condition:
        if process.poll() is None:
            capture('failure')
            message += ' state=' + str(read()) + ' feel=' + str(read(FEEL))
        raise AssertionError(message)

def wait_for(predicate, limit, message):
    for _ in range(limit):
        if predicate():
            check(True, message)
            return
        frames()
    check(False, message)

def retry():
    frames(1, SELECT)
    check(read()[1] == 8, 'Select starts a short lesson retry')
    frames(7)
    check(read()[1] == 3 and read()[10] == 100 and not read(PRACTICE)[2], 'Retry restores resources and cue')

def meter_pixels(image, who):
    x = 67 if who == 0 else 205
    return image.getpixel((x, 32)), image.getpixel((x, 36))

def escape(who, name):
    before = read()
    check(before[16 + who] == 6 and read(FEEL)[14 + who] == 14, 'Ordinary hit opens visible escape window')
    ready = capture(name + '-ready')
    buttons(who, L)
    for _ in range(8):
        buttons(who, L | R)
        if read()[16 + who] == 10:
            break
    check(read()[16 + who] == 10, 'Fresh shoulder chord triggers the backhop')
    after = read()
    check(after[10 + who] == before[10 + who] - 60 and after[6 + who] == before[6 + who],
          'Exactly 60 SP spent with resolved hit damage retained')
    check(read(FEEL)[14 + who] == 15 and after[14 + who] < 120 * 256, 'Backhop is airborne and labeled')
    # The debugger updates ahead of the next displayed framebuffer.
    buttons(who, L | R, 3)
    active = capture(name + '-backhop')
    notch, filled = meter_pixels(ready, who)
    spent_notch, empty = meter_pixels(active, who)
    check(min(notch) >= 140 and notch == spent_notch and filled[0] >= 180 and max(empty) < 100,
          f'Fixed 60 SP notch remains readable as the fill recedes ({notch}, {filled}, {empty})')
    buttons(who, L | R, 24)
    check(read()[10 + who] == after[10 + who], 'Held R spends only once')
    frames(3)
    return before[10 + who], after[10 + who]

try:
    info = command('i').split()
    check(info[1] == 'a67176cd3e6bf60a5016d7adc94357142dd12617', 'Pinned upstream mGBA with real Lua input')
    frames(90)
    tap(B)
    check(read()[1] == 9, 'Title shortcut opens Practice')
    tap(RIGHT)
    check(read()[2] == 1, 'Practice selects a fighter without a signature technique')
    tap(SELECT)
    tap(LEFT)
    check(read(DEPTH)[3] == 6, 'Breakaway is selectable on Charmander')
    capture('practice-option')
    tap(A)
    frames(185)
    wait_for(lambda: read(FEEL)[14] == 14, 80, 'Dummy lands a real ordinary hit')

    frames(1, L | R)
    check(read(PRACTICE)[11] == 16384, 'Escape buffers in hitstop')
    tap(START)
    check(read()[1] == 4 and read(PRACTICE)[11] == 0 and read()[10] == 100, 'Pause clears queued escape')
    tap(START)
    frames(75)
    check(read()[1] == 3 and read()[10] == 100 and not read(PRACTICE)[2], 'Resume requires a new press')
    retry()
    wait_for(lambda: read(FEEL)[14] == 14, 80, 'Retried dummy hit reopens the window')
    escape(0, 'practice')
    check(read(PRACTICE)[2] == 1 and read()[10] == 40, 'Successful real escape clears the drill')
    capture('practice-complete')
    retry()

    tap(START)
    tap(DOWN)
    tap(DOWN)
    tap(A)
    check(read()[1] == 9, 'Change returns to Practice setup')
    tap(B)
    check(read()[1] == 15, 'Practice Back returns to modes')
    tap(DOWN)
    check(read(ARCADE)[17] == 7, 'Local Versus is visible')
    tap(A)
    check(read()[1] == 17 and read(VERSUS)[2] == 1, 'Local Versus is connected')
    frames(1, A, A)
    frames(65)
    check(read()[1] == 3, 'Both players ready into one fight')
    check(read()[10:12] == [0, 0], 'Each player starts without power')

    for who in (0, 1):
        for shot in range(3):
            rival_hp = read()[7 - who]
            tap(B, who)
            wait_for(lambda: read()[7 - who] < rival_hp, 100, 'Real projectile earns meter')
            frames(35)
            check(read()[10 + who] == (shot + 1) * 22, 'Clean projectile adds 22 SP')

    for who in (0, 1):
        attacker = 1 - who
        for _ in range(110):
            state = read()
            dx = state[12 + who] - state[12 + attacker]
            if abs(dx) <= 22 * 256:
                break
            buttons(attacker, RIGHT if dx > 0 else LEFT, 2)
        check(abs(read()[12] - read()[13]) <= 22 * 256, 'Attacker closes to melee range')
        tap(A, attacker)
        wait_for(lambda: read(FEEL)[14 + who] == 14, 40, 'Versus hit enables defender escape')
        sp_before, sp_after = escape(who, 'versus-p' + str(who + 1))
        cases.append(dict(player=who + 1, sp_before=sp_before, sp_after=sp_after))

    check(missed == 0, 'No missed display frames')
    process.stdin.write('q\n')
    process.stdin.flush()
    process.wait(timeout=10)
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    errors.close()

check(process.returncode == 0 and SAVE.read_bytes() == original and not (OUT / 'errors.log').read_text().strip(),
      'Clean emulator exit and unchanged solo SRAM')
report = dict(
    status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
    adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(), mgba_revision=info[1],
    drill_completed=True, both_player_sides=True, power_spent=60, versus_cases=cases,
    pause_discards_escape=True, zero_state_writes=True, save_byte_identical=True,
    peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
)
(OUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps(report, indent=2))
