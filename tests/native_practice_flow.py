"""Play both drills and measure real Practice retries without memory writes."""
from pathlib import Path
from PIL import Image
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/native_artifacts/practice'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
characters = json.loads((ROOT / 'config/balance.json').read_text())['characters']
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, DEPTH, COACH, MASTERY, FEEL, PRESENTATION = map(symbol, [
    'pkmn_debug', 'pkmn_depth_debug', 'pkmn_practice_debug', 'pkmn_mastery_debug',
    'pkmn_feel_debug', 'pkmn_presentation_debug'])
errors = (OUTPUT / 'errors.log').open('w')
process = subprocess.Popen(
    [os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM)],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)
checks = 0
peak = 0
missed = 0
retries = []
lessons = []


def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    line = process.stdout.readline().strip()
    if not line:
        raise RuntimeError('Emulator stopped. See ' + str(OUTPUT / 'errors.log'))
    return line


def read(address=MAIN):
    operation = 'v' if address in (COACH, FEEL) else 's'
    return list(map(int, command(f'{operation} {address:08x}').split()[1:]))


def frames(count=1, keys=0):
    global peak, missed
    command(f'f {count} {keys}')
    state = read()
    peak = max(peak, state[21])
    missed = max(missed, state[22])


def tap(keys):
    frames(keys=keys)
    frames(3)


def capture(name):
    path = OUTPUT / (name + '.ppm')
    command('d ' + str(path))
    with Image.open(path) as picture:
        picture.save(OUTPUT / (name + '.png'))


def check(condition, message):
    global checks
    checks += 1
    if not condition:
        capture('failure')
        raise AssertionError(message + ' state=' + str(read()) + ' coach=' + str(read(COACH)))


def setup(character, mode, arena=0):
    if read()[1] == 3:
        tap(8)
        tap(128)
        tap(128)
        tap(1)
    check(read()[1] == 9, 'Practice setup')
    if read(DEPTH)[2]:
        tap(2)
    for _ in range(9):
        if read()[2] == character:
            break
        tap(16)
    tap(4)
    for _ in range(5):
        if read(DEPTH)[3] == mode:
            break
        tap(16)
    tap(128)
    for _ in range(9):
        if read(DEPTH)[4] == (character + 1) % 9:
            break
        tap(16)
    tap(128)
    for _ in range(3):
        if read(MASTERY)[1] == arena:
            break
        tap(16)
    check(read()[2] == character and read(DEPTH)[3] == mode, 'Chosen fighter and training type')
    capture('setup-' + str(mode))
    tap(1)
    frames(185)
    check(read()[1] == 3 and read(DEPTH)[19] == 1, 'Actual training match')
    check(read(MASTERY)[2] == arena, 'Chosen arena')


def no_mastery():
    check(read(MASTERY)[4:8] == [0, 0, 0, 0], 'Drill cannot award or advance mastery')


def retry():
    before = read()
    frames(keys=4)
    check(read()[1] == 8 and read(COACH)[5] == 6, 'Reset enters six-frame Ready')
    elapsed = 1
    while read()[1] == 8 and elapsed < 15:
        frames()
        elapsed += 1
    current = read()
    character = current[2]
    check(current[1] == 3 and current[5] == 0, 'Ready returns to an unadvanced combat state')
    check(current[6] == characters[character]['max_hp'] and
          current[8] == characters[character]['max_stamina'] * 60 and current[10:12] == [100, 100], 'Retry restores real resources')
    check(current[2:4] == before[2:4], 'Retry retains participants')
    check(read(COACH)[10:13] == [0, 0, 0], 'Retry clears every projectile and pending action')
    check(read(DEPTH)[5:8] == [0, 0, 0] and not read(COACH)[2], 'Manual retry clears statistics and completion')
    frames(keys=1)
    elapsed += 1
    check(read()[16] == 2 and elapsed <= 15, 'Fresh melee acts within the retry budget')
    retries.append(dict(character=characters[character]['id'], mode=read(DEPTH)[3], actionable_frame=elapsed))
    frames(35)


def followup():
    tap(1)
    for _ in range(80):
        if read(FEEL)[14] in (7, 8):
            break
        frames()
    check(read(FEEL)[14] in (7, 8), 'Real hit-confirm opening')
    frames(keys=2)
    check(read()[16] == 3 and read(DEPTH)[8] == 1 and not read(COACH)[2], 'B spends a real follow-up before success')
    for _ in range(100):
        if read(COACH)[2]:
            break
        frames()
    check(read(COACH)[1:3] == [4, 1] and read(DEPTH)[14] >= 1, 'Damaging B follow-up completes the lesson')
    no_mastery()


def evade(button, pause=False):
    while read()[5] < 50:
        frames()
    check(read(COACH)[3] and read()[17] == 5, 'Rival genuinely starts one super')
    if pause:
        tap(8)
        frozen = read()[5:21]
        coach = read(COACH)[:5]
        frames(40)
        check(read()[1] == 4 and read()[5:21] == frozen and read(COACH)[:5] == coach, 'Pause freezes the drill and its coaching')
        tap(2)
        frames(65)
    pressed = False
    for _ in range(140):
        state = read()
        age = state[5] - read(DEPTH)[28]
        keys = button if not pressed and state[17] == 5 and age >= 35 else 0
        if keys:
            pressed = True
        frames(keys=keys)
        if read(COACH)[2]:
            break
    check(pressed and read(COACH)[1:5] == [8, 1, 1, 1], 'A complete nearby jump/squeeze dodge succeeds')
    check(read()[6] == characters[read()[2]]['max_hp'], 'Super did not hit the player')
    no_mastery()


try:
    frames(90)
    tap(2)
    for character in range(9):
        setup(character, 3, character % 3)
        followup()
        frames(20)
        capture('followup-' + characters[character]['id'])
        retry()
        # Retry's fresh A can be followed through the same normal opening.
        check(read()[18] == 3600, 'Practice keeps its frozen timeout')
        lessons.append(dict(character=characters[character]['id'], followup=True))

        setup(character, 4, (character + 1) % 3)
        evade(64, pause=character == 0)
        frames(20)
        capture('jump-' + characters[character]['id'])
        retry()
        evade(128)
        frames(20)
        capture('squeeze-' + characters[character]['id'])
        lessons[-1].update(jump=True, squeeze=True)

    # Free, Block and Attack use the same retry contract as the two lessons.
    for character in range(9):
        for mode in (0, 1, 2):
            setup(character, mode, (character + mode) % 3)
            retry()
            no_mastery()

    # A press made during Ready must not emerge as a stale attack afterward.
    frames(keys=6)
    frames(keys=2)
    frames()
    frames(9, 1)
    check(read()[1] == 3 and read()[16] == 0 and read(COACH)[11:13] == [0, 0], 'Reset ignores pressed and held attacks across Ready')
    frames()
    frames(keys=1)
    check(read()[16] == 2, 'A new edge works after releasing the held button')

    # A failed dodge reports an actual super hit, then offers the same fast retry.
    setup(0, 4, 2)
    frames(170)
    check(read(COACH)[1:3] == [9, 0] and read()[6] < characters[0]['max_hp'], 'A real failed dodge has accurate coaching')
    capture('super-hit')
    retry()

    # Resolve a real Practice KO, then measure the automatic restart.
    setup(0, 3, 0)
    for frame in range(9000):
        state = read()
        if state[1] == 5:
            break
        status = read(FEEL)[14]
        distance = abs(state[13] - state[12]) // 256
        keys = 2 if status in (7, 8) else 16 if distance > 20 else 1 if state[16] == 0 and state[8] >= 1200 and frame % 8 == 0 else 0
        frames(keys=keys)
    check(read()[1] == 5 and read(PRESENTATION)[24] == 6, 'Actual Practice KO has a six-frame hold')
    old_hits = read(DEPTH)[18]
    elapsed = 1
    while read()[1] != 3 and elapsed < 15:
        frames()
        elapsed += 1
    frames(keys=1)
    elapsed += 1
    check(read()[16] == 2 and elapsed <= 15, 'Automatic KO restart accepts a fresh action within budget')
    check(read(DEPTH)[18] >= old_hits and read(DEPTH)[31] >= 2, 'Automatic restart retains Practice statistics')
    no_mastery()

    # Return through normal menus and verify training resources never leak.
    tap(8)
    tap(128)
    tap(128)
    tap(1)
    tap(2)
    tap(64)
    tap(64)
    tap(1)
    tap(1)
    check(read()[1] == 2 and read()[10:12] == [0, 0] and not read(DEPTH)[19], 'Ordinary match keeps its normal countdown and earned meter')
    check(missed == 0, 'Zero missed native display frames')
    report = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
                  characters=lessons, manual_retries=retries, automatic_ko_actionable_frame=elapsed,
                  stale_inputs_cleared=True, practice_excluded_from_mastery=True, normal_mode_restored=True,
                  peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
                  method='mGBA 0.10.2, actual button input, resolved events, read-only telemetry and native captures; no gameplay-memory writes')
    (OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({key: value for key, value in report.items() if key not in ('characters', 'manual_retries')}, indent=2))
finally:
    if process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()
