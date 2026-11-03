"""Post-match goals/scouting through real buttons, pixels and offline SRAM fixtures."""
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
OUT = ROOT / 'tests/native_artifacts/goals'
OUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, GOALS, MASTERY, ARCADE, FEEL, DEPTH = map(symbol, [
    'pkmn_debug', 'pkmn_goals_debug', 'pkmn_mastery_debug',
    'pkmn_arcade_debug', 'pkmn_feel_debug', 'pkmn_depth_debug'])
CHARACTERS = json.loads((ROOT / 'config/balance.json').read_text())['characters']
ACTIONS = ['KEEP SOME SPACE', 'JUMP FIRE', 'USE SUPER', 'BAIT THEN CLOSE',
           'BLOCK FIRST A', 'AVOID MISSED A', 'JUMP THE PULSE', 'PUNISH LANDING', 'LEAVE SPACE', 'KEEP SOME SPACE', 'PUNISH LANDING', 'AVOID MISSED A', 'LEAVE SPACE']
FONT = Image.open(ROOT / 'docs/pkfont.png').convert('RGB')
SAVE = OUT / 'goals.sav'
process = None
checks = peak = missed = 0
cases = []
scouts = set()
started = time.time()


def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    line = process.stdout.readline().strip()
    if not line:
        raise RuntimeError('Emulator stopped; see ' + str(OUT / 'errors.log'))
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
    frames(1, keys)
    frames(3)


def capture(name):
    path = OUT / (name + '.ppm')
    command('d ' + str(path))
    picture = Image.open(path).convert('RGB')
    picture.save(OUT / (name + '.png'))
    return picture


def check(value, message):
    global checks
    checks += 1
    if not value:
        capture('failure')
        raise AssertionError(message + ' state=' + str(read()) + ' goals=' + str(read(GOALS)[:11]))


def mask(picture):
    pixels = picture.tobytes()
    return bytes(255 if min(pixels[index:index + 3]) >= 180 else 0 for index in range(0, len(pixels), 3))


def text_pixels(picture, text, y):
    """Match the entire actual native glyph mask, not only debug fields."""
    expected = Image.new('RGB', (len(text) * 8, 8))
    for index, char in enumerate(text):
        if char != ' ':
            glyph = ord(char) - 33
            expected.paste(FONT.crop((0, glyph * 8, 8, glyph * 8 + 8)), (index * 8, 0))
    target = mask(expected)
    x = 120 - expected.width // 2
    matches = any(mask(picture.crop((x + dx, top, x + dx + expected.width, top + 8))) == target
                  for dx in (-1, 0, 1) for top in range(y - 5, y + 1))
    check(matches, 'Actual framebuffer displays ' + text)
    check(len(text) <= 28, 'Text respects eight-pixel side margins')
    for left, right in ((0, 8), (232, 240)):
        check(not any(mask(picture.crop((left, y - 4, right, y + 4)))),
              'No clipped native text at screen edge')


def close():
    global process
    if process is not None and process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
        assert process.returncode == 0
    process = None


def fixture(fighter=0, followups=0, evades=0, perfect=False):
    """Valid schema-4 bytes prepared BEFORE boot; no live gameplay writes."""
    close()
    words = [0] * 149
    words[:4] = [0x504b4d38, 4, 1, 3]
    words[121 + fighter * 2:123 + fighter * 2] = [followups, evades]
    if perfect:
        words[4 + fighter * 9:7 + fighter * 9] = [1, 1, 600]
    payload = struct.pack('<148I', *words[:-1])
    checksum = 2166136261
    for value in payload:
        checksum = ((checksum ^ value) * 16777619) & 0xffffffff
    data = bytearray(32768)
    data[1024:1620] = payload + struct.pack('<I', checksum)
    SAVE.write_bytes(data)


def boot():
    global process
    process = subprocess.Popen(
        [os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM), str(SAVE)],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=(OUT / 'errors.log').open('a'),
        text=True, bufsize=1)
    frames(90)
    check(read()[1] == 0, 'Boot title')


def choose(mode, fighter=0, level=0):
    if read()[1] == 0:
        tap(8)
    check(read()[1] == 15, 'Visible mode menu')
    for _ in range(7):
        if read(ARCADE)[17] == mode:
            break
        tap(128)
    tap(1)
    for _ in range(13):
        if read()[2] == fighter:
            break
        tap(16)
    if mode != 2:
        tap(4)
        for _ in range(3):
            if read()[4] == level:
                break
            tap(32)
    tap(8)
    check(read()[1] == 2, 'One confirm enters the existing countdown')
    check(not read(GOALS)[1] and not read(GOALS)[10], 'Prior result text cleared')
    frames(185)


def finish(auto=False):
    if auto:
        command(f'b 6500 {MAIN:08x} {FEEL:08x}')
    else:
        frames(6500)
    if read()[1] == 5:
        frames(180)
    check(read()[1] in (6, 12, 13), 'Real fight reaches results')
    frames(8)


def goal_case(name, expected=None):
    g = read(GOALS)
    check(g[0] == 0x474f414c and g[1] and g[10] & 1, 'Result goal is visible')
    if expected is not None:
        check(g[2:7] == expected, name + ' has the actual per-match delta')
    if g[2] == 4:
        text = 'ALL 3 MASTERY MEDALS!'
    else:
        label = {1: 'FOLLOW-UP HITS', 2: 'SUPER EVADES', 3: 'NO-CONTINUE ARCADE'}[g[2]]
        text = ('' if g[6] else 'NEXT: ') + label + ' '
        text += (str(g[3]) + '>' if g[6] else '') + str(g[4]) + '/' + str(g[5])
    picture = capture(name)
    text_pixels(picture, text, 124 if read()[1] == 6 else 84)
    before = read(MASTERY)[4:8]
    frames(60)
    check(read(GOALS)[2:7] == g[2:7] and read(MASTERY)[4:8] == before, 'Results never re-award or rotate goals')
    cases.append(dict(name=name, fighter=g[7], kind=g[2], before=g[3], after=g[4], target=g[5], advanced=bool(g[6])))
    return picture


def real_followup(target):
    for tick in range(24000):
        state = read()
        if read(MASTERY)[4] >= target:
            return
        if state[1] in (5, 6):
            if state[1] == 5:
                frames(180)
            tap(1)
            frames(185)
            continue
        status = int(command(f'v {FEEL:08x}').split()[15])
        distance = abs(state[13] - state[12]) // 256
        keys = 0
        if status in (7, 8):
            keys = 2
        elif distance > 20 and state[16] in (0, 1):
            keys = 16 if state[13] > state[12] else 32
        elif state[16] in (0, 1) and state[8] >= 1800 and tick % 8 == 0:
            keys = 1
        frames(1, keys)
    check(False, 'Real follow-up advances mastery')


def real_evade():
    for _ in range(36000):
        state = read()
        if read(MASTERY)[5] == 5:
            return
        if state[1] in (5, 6):
            if state[1] == 5:
                frames(180)
            tap(1)
            frames(185)
            continue
        distance = abs(state[13] - state[12]) // 256
        keys = 0
        if state[17] == 5:
            age = state[5] - read(DEPTH)[28]
            arrival = 30 - age + max(0, distance - 22) / 5
            if 7 <= arrival <= 13 and state[14] >= 120 * 256 and state[16] in (0, 1):
                keys = 64
        elif state[11] == 100 and distance < 76 and state[16] in (0, 1):
            keys = 32 if state[13] > state[12] else 16
        frames(1, keys)
    check(False, 'Real CPU super evasion earns the fifth step')


try:
    fixture()
    boot()
    choose(0)
    real_followup(1)
    finish()
    goal_case('quick-progress', [1, 0, 1, 10, 1])
    tap(1)
    check(read()[1] == 2, 'One confirm rematches without an added screen')
    frames(185)
    finish()
    goal_case('quick-no-change', [1, 1, 1, 10, 0])

    fixture(followups=9)
    boot()
    choose(0)
    real_followup(10)
    finish()
    picture = goal_case('quick-medal', [1, 9, 10, 10, 1])
    check(read(GOALS)[10] & 4, 'New medal announcement is retained')
    text_pixels(picture, 'MASTERY MEDAL EARNED!', 112)
    close()
    boot()
    choose(0)
    finish()
    goal_case('saved-medal-next-goal', [2, 0, 0, 5, 0])

    fixture(3, 10, 4)
    boot()
    choose(0, 3, 1)
    real_evade()
    finish()
    picture = goal_case('evade-medal', [2, 4, 5, 5, 1])
    text_pixels(picture, 'MASTERY MEDAL EARNED!', 112)

    # Both the longer fighter name and the longest fallback line fit. Every
    # fighter gets its own snapshot rather than borrowing the prior selection.
    for fighter in range(13):
        fixture(fighter, 10, 5)
        boot()
        choose(0, fighter)
        finish()
        goal_case('remaining-arcade-' + CHARACTERS[fighter]['id'], [3, 0, 0, 1, 0])
        check(read(GOALS)[7] == fighter, 'Goal belongs to the selected fighter')
    fixture(8, 10, 5, True)
    boot()
    choose(0, 8)
    finish()
    goal_case('all-medals', [4, 0, 0, 0, 0])

    # Optional Practice drills retain their existing retry and no permanent goal.
    fixture()
    boot()
    choose(2)
    frames(1, 4)
    frames(8)
    check(read()[1] == 3 and not read(GOALS)[1] and not read(GOALS)[10], 'Practice retry has no mastery result or scouting')

    # A naturally exhausted run cannot scout a rival the player cannot enter.
    fixture(0, 10, 5)
    boot()
    choose(1)
    for remaining in (2, 1, 0):
        finish()
        check(read()[20] == 2 and read(ARCADE)[4] == remaining, 'Idle player loses with the expected continues')
        goal_case('exhausted-' + str(remaining), [3, 0, 0, 1, 0])
        if remaining == 0:
            check(read(GOALS)[8] == 0 and not read(GOALS)[10] & 2, 'Exhausted run removes the rival cue')
        tap(1)
        check(read()[1] == (2 if remaining else 15), 'Continue/finish still uses one confirm')
        if remaining:
            frames(185)

    # Real five-rival runs cover the authored cues, loss/continue paths and an
    # actual newly earned no-continue medal. Fixture counters are only the two
    # pre-existing technique medals; the clear itself must be played and saved.
    perfect_clear = False
    champion_seen = False
    loss_seen = False
    exhausted_seen = True
    advances = 0
    for attempt in range(20):
        fighter = 3 if attempt % 2 == 0 else 4
        fixture(fighter, 10, 5)
        boot()
        frames(attempt * 7)
        choose(1, fighter)
        while True:
            finish(auto=True)
            state, run, goals = read(), read(ARCADE), read(GOALS)
            picture = goal_case('arcade-' + str(attempt) + '-' + str(run[3]) + '-' + str(run[4]))
            if state[1] == 13:
                champion_seen = True
                check(goals[8] == 0 and not goals[10] & 2, 'Champion has no fictional sixth rival')
                if run[5] == 0:
                    perfect_clear = True
                    check(goals[2:7] == [3, 0, 1, 1, 1] and goals[10] & 4, 'Real no-continue clear shows the new medal increment')
                    capture('champion-new-medal')
                break
            rival = run[12 + run[3]]
            can_retry = state[20] != 2 or run[4] > 0
            if can_retry:
                check(goals[8] == rival + 1 and goals[10] & 2, 'Cue describes the actual next or retry rival')
                cue = CHARACTERS[rival]['style']['trait_name'] + ': ' + ACTIONS[rival]
                text_pixels(picture, cue, 120)
                if rival not in scouts:
                    capture('scout-' + CHARACTERS[rival]['id'])
                scouts.add(rival)
            else:
                check(goals[8] == 0 and not goals[10] & 2, 'Exhausted run has no impossible next rival')
                exhausted_seen = True
            if state[20] == 2:
                loss_seen = True
            before = run[3:7]
            tap(1)
            if not can_retry:
                check(read()[1] == 15, 'One confirm finishes an exhausted run')
                break
            check(read()[1] == 2, 'One confirm starts the next or repeated rival')
            check(read()[3] == rival, 'Scouted rival is the fighter actually loaded')
            check(read(ARCADE)[3] == before[0] and read(ARCADE)[6] == before[3], 'Results/scouting add no stage or battle ticks')
            if state[20] == 2:
                check(read(ARCADE)[4] == before[1] - 1, 'Continue still costs exactly one')
            else:
                check(read(ARCADE)[4] == before[1], 'Winning/drawing never spends a continue')
            advances += 1
            frames(185)
        if len(scouts) == len(CHARACTERS) and perfect_clear and loss_seen:
            break
    check(len(scouts) == len(CHARACTERS), 'All thirteen actual Arcade rivals display their own cue')
    check(champion_seen and perfect_clear and loss_seen, 'Actual wins, paid losses and a no-continue Champion validated')
    check(missed == 0, 'Zero missed display frames')
    report = dict(status='passed', checks=checks, cases=cases, scouting_fighters=sorted(scouts),
                  actual_arcade_advances=advances, arcade_runs=attempt + 1, champion=champion_seen,
                  new_no_continue_medal=perfect_clear, paid_loss=loss_seen, exhausted_run=exhausted_seen,
                  actual_followup_delta=True, actual_new_followup_medal=True, actual_new_evade_medal=True, power_cycle=True,
                  practice_excluded=True, native_glyph_masks=True, unchanged_confirm_paths=True,
                  peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
                  rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(), wall_seconds=round(time.time() - started, 2),
                  method='mGBA 0.10.2 real GBA keys, read-only telemetry and exact native font masks; valid schema-4 SRAM fixtures prepared offline before boot, actual earned increments and five-rival clears; no gameplay-memory writes')
    (OUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({key: value for key, value in report.items() if key != 'cases'}, indent=2))
finally:
    close()
