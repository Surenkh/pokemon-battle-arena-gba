"""Exercise setup, focus and Back through real buttons and native pixels."""
from pathlib import Path
from PIL import Image
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/native_artifacts/navigation'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, DETAIL, DEPTH, ARCADE, AUDIO = map(symbol, [
    'pkmn_debug', 'pkmn_mastery_debug', 'pkmn_depth_debug', 'pkmn_arcade_debug', 'pkmn_presentation_debug'])
errors = (OUTPUT / 'errors.log').open('w')
process = subprocess.Popen(
    [os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM)],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)
checks = 0
peak = 0
missed = 0
menu_keys = 0


def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    line = process.stdout.readline().strip()
    if not line:
        raise RuntimeError('Emulator stopped. See ' + str(OUTPUT / 'errors.log'))
    return line


def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))


def frames(count=1, keys=0):
    global peak, missed
    command(f'f {count} {keys}')
    state = read()
    peak = max(peak, state[21])
    missed = max(missed, state[22])


def tap(keys):
    global menu_keys
    if read()[1] in (0, 1, 4, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16):
        menu_keys |= keys
    frames(keys=keys)
    frames(3)


def check(condition, message):
    global checks
    checks += 1
    if not condition:
        capture('failure')
        raise AssertionError(message + ' state=' + str(read()) + ' setup=' + str(read(DETAIL)[:4]))


def capture(name):
    frames(6)
    path = OUTPUT / (name + '.ppm')
    command('d ' + str(path))
    with Image.open(path) as picture:
        result = picture.convert('RGB')
    result.save(OUTPUT / (name + '.png'))
    return result


def row():
    return read(DEPTH)[2] if read()[1] == 9 else read(DETAIL)[3]


def menu():
    screen = read()[1]
    if screen == 3:
        tap(8)
        tap(128)
        tap(128)
        tap(1)
        screen = read()[1]
    if screen in (1, 9):
        if row():
            tap(2)
        tap(2)
    elif screen in (6, 7, 10, 11, 12, 13, 14, 16):
        tap(2)
        if read()[1] == 1:
            tap(2)
    elif screen == 0:
        tap(8)
    check(read()[1] == 15, 'Return to visible mode menu')


def choose(mode):
    menu()
    for _ in range(8):
        if read(ARCADE)[17] == mode:
            break
        tap(128)
    tap(1)


def fighter(character):
    check(read()[1] in (1, 9) and row() == 0, 'Fighter grid is focused')
    for _ in range(13):
        if read()[2] == character:
            break
        tap(16)
    check(read()[2] == character, 'Requested fighter selected')


def play(button=8):
    tap(button)
    check(read()[1] == 2, 'One confirmation starts countdown')
    frames(185)
    check(read()[1] == 3, 'Requested battle starts')


def gold(picture, area):
    pixels = picture.crop(area).getdata()
    return sum(r > 220 and g > 160 and b < 160 for r, g, b in pixels)


try:
    frames(90)
    frames(24, 1)
    check(read()[1] == 15, 'Held title confirmation never activates the next screen')
    frames()
    tap(1)
    check(read()[1] == 1, 'Quick Battle is the initial mode')
    play(1)
    check(read()[2] == 0, 'Three-input shortcut launches the highlighted fighter')

    choose(4)
    tap(1)
    tap(128)
    tap(1)
    tap(2)
    check(read(AUDIO)[1:3] == [0, 0] and read()[1] == 15, 'Audio settings return to their mode row')
    check(read(ARCADE)[17] == 4, 'Audio Back restores originating focus')
    capture('mode-menu')

    # The first product task needs no roster traversal or shoulder shortcut.
    choose(0)
    fighter(0)
    frames(24, 4)
    check(row() == 1 and read()[2] == 0, 'Select goes directly to settings and never repeats while held')
    frames()
    tap(128)
    tap(16)
    check(row() == 2 and read(DETAIL)[1] == 1 and read()[2] == 0, 'Pikachu selects Volcanic directly')
    picture = capture('pikachu-volcanic')
    check(gold(picture, (8, 122, 16, 135)) > 10 and gold(picture, (224, 122, 232, 135)) > 10, 'Arena focus reaches both ends of the full row')
    tap(128)
    tap(1)
    check(read()[1] == 10, 'Visible Fighter Info opens with A')
    tap(2)
    check(read()[1] == 1 and row() == 3 and read()[2] == 0, 'Guide Back restores its row and fighter')
    tap(2)
    check(row() == 0 and read()[2] == 0, 'Settings Back restores the chosen card')
    play()
    check(read()[2] == 0 and read(DETAIL)[2] == 1, 'Pikachu actually fights in Volcanic')

    choose(2)
    fighter(8)
    tap(4)
    tap(16)
    tap(128)
    for _ in range(13):
        if read(DEPTH)[4] == 2:
            break
        tap(16)
    tap(128)
    tap(16)
    check(read(DEPTH)[2:5] == [3, 1, 2] and read(DETAIL)[1] == 2, 'Machop configures blocking Squirtle in Shoreline')
    capture('machop-practice')
    tap(128)
    tap(1)
    check(read()[1] == 10, 'Practice uses the same visible Info action')
    tap(2)
    check(row() == 4 and read()[2] == 8, 'Practice guide restores its exact context')
    play()  # Start also plays from the Info row.
    check(read()[2:4] == [8, 2] and read(DETAIL)[2] == 2, 'Practice launches the configured fighter, rival and arena')

    # Every roster card uses the same Select, Info and Back convention.
    for mode in (0, 1, 2):
        choose(mode)
        for character in range(13):
            fighter(character)
            before = (read()[4], read(DETAIL)[1], read(DEPTH)[3:5])
            tap(4)
            check(row() == 1 and read()[2] == character, 'Direct settings retain every fighter')
            tap(128)
            if mode == 1:
                check(row() == 3, 'Arcade skips the informational rotating-arena row')
                tap(64)
                check(row() == 1, 'Arcade skips the rotating row in both directions')
                tap(128)
            else:
                tap(128)
                if mode == 2:
                    tap(128)
            tap(1)
            check(read()[1] == 10, 'Info is reachable without a shoulder button')
            tap(2)
            check(row() == (4 if mode == 2 else 3), 'Info returns to its action row')
            tap(2)
            check(row() == 0 and read()[2] == character, 'Back restores the exact chosen card')
            check(before == (read()[4], read(DETAIL)[1], read(DEPTH)[3:5]), 'Guide trip preserves configured values')
        capture('setup-' + str(mode))

    choose(0)
    play()
    command(f'a 8000 {MAIN:08x}')
    frames()
    check(read()[1] == 6, 'Actual normal battle reaches results')
    picture = capture('result-rematch')
    check(gold(picture, (8, 136, 112, 154)) > 50 and gold(picture, (128, 136, 232, 154)) == 0, 'Only Rematch has the initial active outline')
    tap(16)
    picture = capture('result-change')
    check(gold(picture, (8, 136, 112, 154)) == 0 and gold(picture, (128, 136, 232, 154)) > 50, 'Only Change has the moved active outline')

    choose(1)
    fighter(0)
    play()
    for remaining in (2, 1, 0):
        if remaining == 0:
            tap(8)
            tap(128)
            check(read()[1] == 4 and read(ARCADE)[20] == 2, 'Pause skips unavailable Rematch')
            capture('pause-no-continue')
            tap(64)
            check(read(ARCADE)[20] == 0, 'Reverse navigation also skips unavailable Rematch')
            tap(2)
            frames(65)
        frames(6000)
        check(read()[1] == 12 and read()[20] == 2 and read(ARCADE)[4] == remaining, 'Natural loss consumes no continue before confirmation')
        if remaining:
            tap(1)
            check(read()[1] == 2 and read(ARCADE)[4] == remaining - 1, 'Continue confirmation consumes exactly one')
            frames(185)
    capture('arcade-finish')
    tap(1)
    check(read()[1] == 15 and not read(ARCADE)[2], 'A finishes an exhausted run visibly')
    tap(2)
    for expected in (15, 1, 2):
        frames(keys=1)
        check(read()[1] == expected, 'Rapid A taps take one transition each')
        frames()
    check(not menu_keys & 0x300, 'Every tested menu route uses no shoulder button')
    check(missed == 0, 'Zero missed native display frames')
    report = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
                  configured_product_tasks=2, roster_guide_round_trips=27, menu_keys=menu_keys,
                  held_confirmation_safe=True, result_focus_unique=True, exhausted_continue_paths=True,
                  peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
                  method='mGBA 0.10.2, real buttons, read-only telemetry and framebuffer captures; no gameplay-memory writes')
    (OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
finally:
    if process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()
