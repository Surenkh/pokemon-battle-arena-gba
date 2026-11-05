"""Choose challengers, beat all three Captains, and verify the native crest and SRAM."""
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
OUTPUT = ROOT / 'tests/native_artifacts/cup'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
SAVE = OUTPUT / 'cup.sav'
CUPS = json.loads((ROOT / 'config/cups.json').read_text())
CHARS = json.loads((ROOT / 'config/balance.json').read_text())['characters']
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, CUP, ARCADE, MASTERY, FEEL, DEPTH, CPU = map(symbol, [
    'pkmn_debug', 'pkmn_cup_debug', 'pkmn_arcade_debug', 'pkmn_mastery_debug', 'pkmn_feel_debug', 'pkmn_depth_debug', 'pkmn_cpu_debug'])
FONT = Image.open(ROOT / 'docs/pkfont.png').convert('RGB')
CRESTS = Image.open(ROOT / 'docs/cup_crest.png').convert('RGBA')
process = None
checks = peak = missed = 0
started = time.time()
launches, rounds, clears = [], [], []
observed_tactics = {}


def command(value):
    process.stdin.write(value + '\n'); process.stdin.flush()
    result = process.stdout.readline().strip()
    if not result:
        raise RuntimeError('Emulator stopped: ' + (OUTPUT / 'errors.log').read_text()[-1000:])
    return result


def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))


def frames(count=1, keys=0):
    global peak, missed
    command(f'f {count} {keys}')
    state = read(); peak = max(peak, state[21]); missed = max(missed, state[22])


def tap(keys):
    frames(1, keys); frames(5)


def capture(name):
    frames(6)
    path = OUTPUT / (name + '.ppm'); command('d ' + str(path))
    picture = Image.open(path).convert('RGB'); picture.save(OUTPUT / (name + '.png'))
    return picture


def check(value, message):
    global checks
    checks += 1
    if not value:
        if process: capture('failure')
        raise AssertionError(message + (' state=' + str(read()) + ' cup=' + str(read(CUP)) if process else ''))


def text_pixels(picture, value, y, center=120, advance=8):
    expected = Image.new('RGB', (len(value) * advance, 8))
    for index, char in enumerate(value):
        if char != ' ':
            glyph = ord(char) - 33
            expected.paste(FONT.crop((0, glyph * 8, advance, glyph * 8 + 8)), (index * advance, 0))
    def mask(image):
        return bytes(255 if min(pixel) >= 180 else 0 for pixel in image.getdata())
    x = center - expected.width // 2
    check(any(mask(picture.crop((x + dx, top, x + dx + expected.width, top + 8))) == mask(expected)
              for dx in (-1, 0, 1) for top in range(y - 5, y + 1)), 'Native glyphs: ' + value)
    check(len(value) <= 28, 'Native text fits the screen')


def right_pixels(picture, value, right, y):
    expected = Image.new('RGB', (len(value) * 8, 8))
    for index, char in enumerate(value):
        glyph = ord(char) - 33
        expected.paste(FONT.crop((0, glyph * 8, 8, glyph * 8 + 8)), (index * 8, 0))
    def mask(image):
        return bytes(255 if min(pixel) >= 180 else 0 for pixel in image.getdata())
    x = right - expected.width
    check(any(mask(picture.crop((x + dx, top, x + dx + expected.width, top + 8))) == mask(expected)
              for dx in (-1, 0, 1) for top in range(y - 5, y + 1)), 'Native bounded record value ' + value)


def boot():
    global process
    process = subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM), str(SAVE)],
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=(OUTPUT / 'errors.log').open('a'),
                               text=True, bufsize=1)
    frames(90); check(read()[1] == 0, 'Boot title')


def close():
    global process
    if process is not None and process.poll() is None:
        process.stdin.write('q\n'); process.stdin.flush(); process.wait(timeout=10)
        check(process.returncode == 0, 'Clean emulator shutdown')
    process = None


def menu(mode):
    if read()[1] == 0: tap(8)
    for _ in range(9):
        if read(ARCADE)[17] == mode: break
        tap(128)
    check(read()[1] == 15 and read(ARCADE)[17] == mode, 'Visible mode reached by directions')
    tap(1)


def board(route=0, level=0):
    if read()[1] in (0, 15): menu(8)
    check(read()[1] == 18 and read(CUP)[1] and not read(CUP)[2], 'Cup board distinct from active run')
    while read(CUP)[3] != route: tap(16)
    while read()[4] != level: tap(64)


def crest_pixels(picture, route, center=(207, 60), scale=1):
    sprite = CRESTS.crop((0, route * 16, 16, (route + 1) * 16))
    if scale != 1:
        sprite = sprite.resize((16 * scale, 16 * scale), Image.Resampling.NEAREST)
    left, top = (value - 8 * scale for value in center)
    expected = [(x, y, sprite.getpixel((x, y))) for y in range(sprite.height) for x in range(sprite.width)]
    accents = [(x, y, rgb) for x, y, (*rgb, a) in expected if a and max(rgb) > 100]
    def matches(dx, dy):
        return sum(max(abs(c - q) for c, q in zip(rgb, picture.getpixel((left + x + dx, top + y + dy)))) <= 12
                   for x, y, rgb in accents)
    check(max(matches(dx, dy) for dx in (-1, 0, 1) for dy in (-1, 0, 1)) >= len(accents) * .9,
          'Native crest sprite for ' + CUPS[route]['name'])


def check_countdown(rival, fighter, level, captain=False):
    cpu = next(i for i, char in enumerate(CHARS) if char['id'] == rival['fighter'])
    check(read()[1] == 2 and read()[2:5] == [fighter, cpu, level], 'Chosen fighter and authored rival start')
    check(read(MASTERY)[2] == rival['arena'], 'Authored rival location')
    check(read()[6:10] == [CHARS[fighter]['max_hp'], CHARS[cpu]['max_hp'],
                           CHARS[fighter]['max_stamina'] * 60, CHARS[cpu]['max_stamina'] * 60],
          'Ordinary combat resources for every rival')
    picture = capture('countdown-' + rival['name'].lower())
    text_pixels(picture, ('CAPTAIN ' if captain else '') + rival['name'], 104)
    text_pixels(picture, rival['cue'], 119)
    frames(185); check(read()[1] == 3 and not read(DEPTH)[19], 'Cup uses timed combat')


def choose_challenger(route, slot):
    check(read()[1] == 20 and read(CUP)[25] == 0xffffffff, 'Cup waits for a challenger choice')
    defeated = read(CUP)[26]
    for _ in range(3):
        if read(ARCADE)[20] == slot:
            break
        tap(16)
        check(not defeated & (1 << read(ARCADE)[20]), 'The picker skips defeated rivals')
    check(read(ARCADE)[20] == slot, 'Requested challenger card focused')
    rival = CUPS[route]['challengers'][slot]
    picture = capture(f'picker-{route}-{read(CUP)[4]}-{slot}')
    text_pixels(picture, rival['cue'], 129)
    text_pixels(picture, ('WOODLAND', 'VOLCANIC', 'SHORELINE', 'FROSTPEAK', 'ANCIENT RUINS')[rival['arena']], 115)
    for index, candidate in enumerate(CUPS[route]['challengers']):
        x = 44 + index * 76
        text_pixels(picture, candidate['fighter'].upper(), 91, x, advance=6)
        if defeated & (1 << index): text_pixels(picture, 'DEFEATED', 67, x)
    tap(1)
    check(read(CUP)[25:28] == [slot, defeated, 0], 'Confirmed choice locks the challenger and keeps the ordinary CPU')
    return rival


def launch(route, fighter, level, slot=0, check_back=False):
    board(route, level); tap(1)
    check(read()[1] == 1, 'Shared thirteen-fighter Cup selector')
    while read()[2] != fighter: tap(16)
    tap(4); tap(128)
    check(read(MASTERY)[3] == 3, 'Cup rival row is informational; navigation skips it')
    tap(2); tap(8)
    if check_back:
        tap(2)
        check(read()[1:3] == [1, fighter] and not read(CUP)[2], 'Back from first choice keeps the selected fighter')
        tap(1)
    rival = choose_challenger(route, slot)
    check(read(CUP)[5:9] == [1, 0, 0, 0], 'Fresh run resets time and style and grants one continue')
    check_countdown(rival, fighter, level)
    launches.append(dict(cup=route, fighter=fighter, level=level, challenger=slot, arena=rival['arena']))


def observe_captain(route):
    # Look for an actual rush attack, deliberate guard or ranged shot on the ROM.
    if route in observed_tactics:
        return
    for _ in range(100):
        state = read()
        target = read(CPU)[2]
        distance = abs(state[13] - state[12]) // 256
        if state[1] != 3:
            break
        if route == 0 and state[17] == 2 and distance < 30 and target <= 33:
            break
        if route == 1 and state[17] == 1 and read(CPU)[1] == 4:
            break
        if route == 2 and state[17] == 3 and distance >= 44 and target >= 60:
            break
        keys = (16 if state[13] > state[12] else 32) if route == 1 and distance > 31 else 0
        frames(3, keys)
    else:
        check(False, 'Captain signature action is observable during normal play')
    check(state[1] == 3, 'Captain signature seen before round end')
    observed_tactics[route] = dict(cup=route, tactic=CUPS[route]['captain']['tactic'],
                                  action=state[17], target_px=target, distance_px=distance)


def abandon():
    tap(8); check(read()[1] == 4, 'Pause Cup')

    while read(ARCADE)[20] != 2: tap(128)
    tap(1)
    check(read()[1] == 1 and not read(CUP)[2], 'Changing Pokemon ends Cup without a record')
    tap(2); check(read()[1] == 18, 'Back from Cup fighter selection returns to board')


def finish(auto=True):
    if auto: command(f'g 6500 {MAIN:08x} {FEEL:08x}')
    else: frames(5000)
    frames(190)
    check(read()[1] == 19, 'Cup result reached')
    state, detail = read(), read(CUP)
    rounds.append(dict(cup=detail[3], outcome=state[20], wins=detail[4], continues=detail[5], style=detail[8], ticks=detail[7]))
    return state[20]


def cup_words(data, offset):
    return list(struct.unpack_from('<40I', data, offset))


def journal_records(data):
    words = [cup_words(data, offset) for offset in (4096, 5120)]
    newest = max(range(2), key=lambda i: words[i][2])
    return [words[newest][3 + i * 4:7 + i * 4] for i in range(9)], newest, words


def check_base_records(data):
    # Cup mastery may migrate the legacy save, but never earns Arcade records.
    current = [offset for offset in (1024, 2048) if struct.unpack_from('<II', data, offset) == (0x504b4d38, 4)]
    if current:
        offset = max(current, key=lambda o: struct.unpack_from('<I', data, o + 8)[0])
        check(list(struct.unpack_from('<III', data, offset + 16)) == [2, 1, 3600], 'Legacy Pikachu Arcade record unchanged')
        check(list(struct.unpack_from('<III', data, offset + 16 + 17 * 12)) == [7, 3, 14400], 'Legacy Meowth Arcade record unchanged')
        for index in range(39):
            if index not in (0, 17): check(not struct.unpack_from('<I', data, offset + 16 + index * 12)[0], 'Cup awards no ordinary Arcade clears')
    else:
        check(data[:1024] == legacy[:1024], 'Legacy save preserved without base write')


legacy = (ROOT / 'tests/fixtures/six_fighter_v08.sav').read_bytes()
SAVE.write_bytes(legacy)
try:
    boot(); close(); initial = SAVE.read_bytes()
    check(initial[:len(legacy)] == legacy, 'Boot performs no save migration or empty Cup writes')
    boot(); menu(8)
    for route in range(3):
        board(route); text_pixels(capture('board-' + str(route)), 'EASY', 28)
    # All fighters, including mirrors, use the same setup. Each difficulty/route launches.
    for fighter in range(13):
        launch(0, fighter, fighter % 3, fighter % 3, check_back=fighter == 0)
        if fighter == 0:
            tap(8); clock = read(CUP)[7]; frames(90); check(read(CUP)[7] == clock, 'Pause freezes Cup clock')
            tap(128); tap(1)
            check(read(CUP)[5:7] == [0, 1] and read(CUP)[7] == clock, 'Pause rematch costs one continue and retains elapsed time')
            frames(185); tap(8); tap(128)
            check(read(ARCADE)[20] == 2, 'Unavailable Cup rematch is skipped')
            tap(2); frames(65)
        abandon()
    for route in (1, 2):
        for level in range(3): launch(route, 3, level, level); abandon()
    close(); check(SAVE.read_bytes()[4096:5280] == initial[4096:5280], 'Abandoned runs never write Cup journal')
    boot(); menu(8)
    launch(0, 3, 0, 2)
    for remaining in (1, 0):
        check(finish(False) == 2, 'Idle player loses a real Cup round')
        check(read(CUP)[4] == 0 and read(CUP)[14] == 0, 'Loss grants no win or clear')
        picture = capture('loss-' + str(remaining))
        text_pixels(picture, 'A USE CONTINUE' if remaining else 'A FINISH RUN', 132)
        tap(1)
        if remaining:
            check(read()[1] == 2 and read(CUP)[5] == 0 and read(CUP)[25:27] == [2, 0], 'Continue retries same rival')
            frames(185)
    check(read()[1] == 18 and not read(CUP)[2], 'Exhausted run returns to Cup board')
    # Earn real clears; the driver supplies only standard physical button masks.
    for route in range(3):
        champion = False
        for attempt in range(18):
            first = attempt % 3
            second = (first + 2) % 3
            launch(route, 3, 0, first)
            if route == 1:
                # Block actual incoming attacks, then leave guard to recover stamina.
                recovering = False
                for _ in range(240):
                    state = read()
                    if read(CUP)[8] >= 5 or state[1] != 3: break
                    if state[8] < 2200: recovering = True
                    if state[8] > 5000: recovering = False
                    keys = (32 if state[13] > state[12] else 16) if recovering else 512
                    frames(12, keys)
            while True:
                outcome = finish()
                detail = read(CUP)
                if detail[9]:
                    champion = True
                    check(detail[4] == 3 and detail[10] and detail[14] == 1 and detail[17] == detail[7], 'Exactly one saved clear and exact cumulative active time')
                    check(detail[16] == int(detail[6] == 0), 'Perfect requires no paid continue')
                    picture = capture('champion-' + str(route)); text_pixels(picture, 'CUP CHAMPION!', 37)
                    text_pixels(picture, CUPS[route]['crest'] + ' EARNED', 91, 136)
                    crest_pixels(picture, route, center=(31, 100), scale=2)
                    check(detail[15] == int(detail[8] >= detail[19]), 'Style record matches real resolved events')
                    clears.append(dict(cup=route, attempts=attempt + 1, style=detail[15], perfect=detail[16], ticks=detail[17],
                                       challengers=[first, second], captain=CUPS[route]['captain']['name']))
                    frames(100); check(read(CUP)[14] == 1, 'Champion does not award twice while waiting')
                    if route == 0:
                        tap(1)
                        check(read()[1] == 20 and read(CUP)[4:9] == [0, 1, 0, 0, 0] and not read(CUP)[26],
                              'Rematch reopens all choices and starts a fresh run')
                        tap(2); tap(2)
                    else:
                        tap(2)
                    crest_pixels(capture('earned-board-' + str(route)), route)
                    break
                if outcome == 2 and not detail[5]: tap(2); break
                before = detail[7]
                check(not detail[14], 'Challenger wins alone never award a crest')
                if outcome == 1 and detail[4] == 1:
                    text_pixels(capture('choose-next-' + str(route)), 'A CHOOSE RIVAL', 132)
                    tap(1)
                    check(read(CUP)[26] == 1 << first, 'First win leaves two eligible challengers')
                    rival = choose_challenger(route, second)
                    check(read(CUP)[7] == before, 'Selection and countdown excluded from elapsed time')
                    check_countdown(rival, 3, 0)
                elif outcome == 1:
                    rival = CUPS[route]['captain']
                    picture = capture('captain-scout-' + str(route))
                    text_pixels(picture, 'A FACE CAPTAIN', 132)
                    text_pixels(picture, rival['cue'], 108)
                    tap(1)
                    check(read(CUP)[25:28] == [3, (1 << first) | (1 << second), route + 1],
                          'Only the Captain unlocks after two distinct wins')
                    check(read(CUP)[7] == before, 'Captain countdown excluded from elapsed time')
                    check_countdown(rival, 3, 0, captain=True)
                    observe_captain(route)
                else:
                    slot = detail[25]
                    rival = CUPS[route]['captain'] if slot == 3 else CUPS[route]['challengers'][slot]
                    text_pixels(capture('retry-scout-' + str(route)), rival['cue'], 108)
                    tap(1)
                    check(read(CUP)[25:27] == detail[25:27] and read(CUP)[7] == before, 'Retry keeps the locked rival and time')
                    check_countdown(rival, 3, 0, captain=slot == 3)
            if champion: break
        check(champion, 'Real three-win clear for Cup ' + str(route))
    close(); earned = SAVE.read_bytes(); check_base_records(earned)
    records, newest, words = journal_records(earned)
    check(sum(record[0] for record in records) == 3, 'Only three completed runs recorded')
    check(any(record[1] for record in records), 'A real native style clear was earned and saved')
    boot(); menu(8)
    for route in range(3):
        board(route, 0); check(read(CUP)[14:18] == records[route * 3], 'Cup achievements survive power cycle')
        crest_pixels(capture('saved-board-' + str(route)), route)
        board(route, 1); text_pixels(capture('locked-board-' + str(route)), 'LOCK', 61, 207)
    tap(2); menu(2); tap(8); frames(185); frames(180, 2); tap(8); tap(128); tap(128); tap(1); tap(2)
    close(); check(SAVE.read_bytes()[4096:5280] == earned[4096:5280], 'Practice never writes Cup achievements')
    # Corrupt and interrupted newest writes recover the older independent copy.
    previous = 1 - newest
    for interrupted in (False, True):
        raw = bytearray(earned); offset = (4096, 5120)[newest]
        if interrupted: raw[offset + 64:offset + 160] = bytes(96)
        else: raw[offset + 40] ^= 1
        SAVE.write_bytes(raw); boot(); menu(8)
        for route in range(3):
            board(route, 0); check(read(CUP)[14:18] == words[previous][3 + route * 12:7 + route * 12], 'Older Cup copy recovered after corruption/interruption')
        close(); check_base_records(SAVE.read_bytes())
    raw = bytearray(earned)
    for offset in (4096, 5120): raw[offset] ^= 1
    SAVE.write_bytes(raw); boot(); menu(8)
    for route in range(3): board(route); check(read(CUP)[14:18] == [0, 0, 0, 0], 'Both bad Cup slots reset only Cup data')
    close(); check_base_records(SAVE.read_bytes())
    # Maximum legal imported records must still fit beside the baked labels.
    maximum = [0x43555031, 1, 99] + [9999, 9999, 9999, 0xffffffff] * 9
    payload = struct.pack('<39I', *maximum); checksum = 2166136261
    for byte in payload: checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
    raw[4096:4256] = payload + struct.pack('<I', checksum)
    SAVE.write_bytes(raw); boot(); menu(8)
    for route in range(3):
        for level in range(3):
            board(route, level); picture = capture(f'bounded-record-{route}-{level}')
            check(read(CUP)[14:18] == [9999, 9999, 9999, 0xffffffff], 'All nine maximum Cup records load')
            for right in (76, 149, 228): right_pixels(picture, '9999', right, 114)
            crest_pixels(picture, route)
    close()
    check(missed == 0, 'Zero missed native display frames')
    check(peak < 4096, 'Cup transitions fit the native frame budget')
    report = dict(status='passed', checks=checks, authored_cups=3, named_rivals=12, launches=launches,
                  rounds=rounds, clears=clears, earned_style_clear=any(r['style'] for r in clears),
                  branching_choices=True, defeated_challenger_skipped=True, retry_rival_locked=True,
                  captain_tactics=list(observed_tactics.values()), earned_crests_power_cycle=True,
                  journal_power_cycle=True, journal_corruption_fallback=True, interrupted_write_fallback=True,
                  base_save_preserved=True, practice_excluded=True, save_copy_bytes=160, save_offsets=[4096, 5120],
                  peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
                  rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(), wall_seconds=round(time.time() - started, 2),
                  method='mGBA 0.10.2; real GBA button masks, read-only telemetry and native glyph matching; legacy fixture and offline SRAM corruption; no gameplay-memory writes')
    (OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n'); print(json.dumps(report, indent=2))
finally:
    close()
