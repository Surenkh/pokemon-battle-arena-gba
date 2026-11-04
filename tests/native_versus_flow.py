"""One-core Local Versus through real upstream mGBA raw-key/Lua input APIs."""
from pathlib import Path
from PIL import Image
import hashlib
import json
import os
import re
import shutil
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/native_artifacts/versus'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
ADAPTER = ROOT / 'tools/local_versus.lua'
CHARACTERS = json.loads((ROOT / 'config/balance.json').read_text())['characters']
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)


def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)


MAIN, VERSUS, DEPTH, MASTERY, ARCADE = map(symbol, [
    'pkmn_debug', 'pkmn_versus_debug', 'pkmn_depth_debug', 'pkmn_mastery_debug', 'pkmn_arcade_debug'])
save = OUTPUT / 'progress.sav'
shutil.copyfile(ROOT / 'tests/fixtures/nine_fighter_v1.sav', save)
original_save = save.read_bytes()
errors = (OUTPUT / 'errors.log').open('w')
driver = os.environ.get('PKMN_VERSUS_DRIVER', str(ROOT / '.tooling/mgba_versus_driver'))
process = subprocess.Popen([driver, str(ROM), str(ADAPTER), str(save)], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)
checks = peak = missed = 0
movie = []
started = time.time()


def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    result = process.stdout.readline().strip()
    if not result:
        raise RuntimeError('mGBA stopped: ' + (OUTPUT / 'errors.log').read_text()[-1500:])
    return result


def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))


def frames(count=1, p1=0, p2=0):
    global peak, missed
    command(f'f {count} {p1} {p2}')
    state = read()
    peak = max(peak, state[21])
    missed = max(missed, state[22])


def tap(p1=0, p2=0):
    frames(1, p1, p2)
    frames(3)


def capture(name=None):
    path = OUTPUT / ((name or 'current') + '.ppm')
    command('d ' + str(path))
    image = Image.open(path).convert('RGB')
    if name:
        image.save(OUTPUT / (name + '.png'))
    return image


def check(condition, description):
    global checks
    checks += 1
    if not condition:
        if process.poll() is None:
            capture('failure')
            description += ' state=' + str(read()) + ' versus=' + str(read(VERSUS))
        raise AssertionError(description)


def choose(player, character):
    for _ in range(13):
        if read(VERSUS)[3 + player] == character:
            return
        tap(16 if player == 0 else 0, 16 if player == 1 else 0)
    check(False, 'Requested fighter reachable for player ' + str(player))


def select_again():
    state = read()[1]
    if state == 3:
        tap(8)
        tap(128)
        tap(128)
        tap(1)
    elif state == 6:
        tap(2)
    check(read()[1] == 17, 'Return directly to the shared chooser')


def start(p1, p2, arena=0):
    choose(0, p1)
    choose(1, p2)
    while read(VERSUS)[9] != arena:
        tap(256)
    tap(1, 1)
    check(read()[1] == 2, 'Both confirmations start a single countdown')
    frames(65)
    check(read()[1] == 3, 'Both players share the active fight')
    check(read()[2:4] == [p1, p2] and read(MASTERY)[2] == arena, 'Actual fighters and location match both selections')


def win_round(player, record=False):
    # A simple public-input policy. It earns power and damage through real hits.
    for step in range(2400):
        state = read()
        if state[1] == 5:
            frames(160)
            break
        if state[1] != 3:
            break
        x, enemy_x = state[12 + player], state[13 - player]
        distance = abs(enemy_x - x) // 256
        buttons = 0
        if state[10 + player] == 100 and state[8 + player] >= 1500 and state[14 + player] >= 120 * 256:
            buttons = 256 if step % 2 == 0 else 0
        elif distance > 18:
            buttons = 16 if enemy_x > x else 32
        elif step % 24 == 0:
            buttons = 1
        frames(1, buttons if player == 0 else 0, buttons if player == 1 else 0)
        if record and step % 3 == 0 and len(movie) < 150:
            movie.append(capture())
    check(read()[1] == 6, 'Physical inputs reach a completed round')
    check(read(VERSUS)[8 if player else 7] > 0, 'Round win belongs to the correct player')


try:
    emulator_info = command('i').split()
    check(emulator_info[1] == 'a67176cd3e6bf60a5016d7adc94357142dd12617', 'Actual linked mGBA core matches the pinned development revision')
    frames(90)
    check(read(VERSUS)[2] == 1, 'Shipped Lua adapter connects through the actual mGBA API')
    tap(1)
    for _ in range(3):
        tap(128)
    check(read(ARCADE)[17] == 7, 'Local Versus is beside Practice in the visible menu')
    capture('mode-menu')
    tap(1)
    check(read()[1] == 17, 'Local Versus opens a shared selection screen')

    for who in range(13):
        other = read(VERSUS)[4]
        frames(1, 16)
        check(read(VERSUS)[3] == (who + 1) % 13, 'P1 cursor responds on the first sampled frame')
        check(read(VERSUS)[4] == other, 'P1 cursor leaves P2 unchanged')
        frames(3)
    for _ in range(13):
        before = read(VERSUS)
        frames(1, 0, 16)
        check(read(VERSUS)[4] == (before[4] + 1) % 13, 'P2 cursor responds on the first sampled frame')
        check(read(VERSUS)[3] == before[3], 'P2 cursor leaves P1 unchanged')
        frames(3)

    tap(1)
    chosen = read(VERSUS)[3]
    tap(16, 16)
    check(read(VERSUS)[3] == chosen and read(VERSUS)[5] == 1, 'Ready P1 stays locked while P2 can browse')
    tap(2)
    check(not read(VERSUS)[5], 'P1 can cancel ready with its own Back key')
    choose(0, 7)
    choose(1, 10)
    capture('shared-selection')

    for character in range(13):
        start(character, (character + 1) % 13, character % 5)
        before = read()
        frames(1, 32, 16)
        after = read()
        check(after[12] < before[12] and after[13] > before[13], 'Both fighters move independently on the first sampled frame')
        frames(1)
        frames(1, 1, 2)
        check(read()[16:18] == [2, 3], 'Simultaneous P1 melee and P2 shot start on the first sampled frame')
        check(read(VERSUS)[13:15] == [1, 2], 'Host keys keep their own pressed edges')
        frames(90, 1, 2)
        check(read()[16:18] == [0, 0], 'Held attacks cannot automatically repeat')
        frames(2)
        frames(1, 64, 64)
        check(read()[14] < 120 * 256 and read()[15] < 120 * 256, 'Both players can jump independently')
        frames(100)
        frames(1, 512, 512)
        check(read()[16:18] == [1, 1], 'Both players can hold guard')
        frames(2)
        frames(1, 128, 128)
        check(read()[16:18] == [4, 4], 'Both players can start squeeze')
        frames(50)
        if character == 0:
            capture('shared-fight')
        select_again()

    start(0, 1, 4)
    tap(0, 8)
    check(read()[1] == 4, 'P2 can pause the shared fight')
    frozen = read(VERSUS)[15]
    frames(40, 1, 2)
    check(read(VERSUS)[15] == frozen, 'Pause freezes all combat state')
    frames(2)
    tap(0, 8)
    frames(70)
    check(read()[1] == 3 and read()[16:18] == [0, 0], 'Resume discards attacks pressed during Pause')
    command('x')
    frames(5)
    check(read()[1] == 4 and not read(VERSUS)[2], 'Adapter loss pauses the shared fight')
    frozen = read(VERSUS)[15]
    frames(25)
    check(read(VERSUS)[15] == frozen, 'Disconnected controls cannot advance combat')
    capture('adapter-disconnected')
    command('c')
    frames(5)
    check(read()[1] == 4 and read(VERSUS)[2], 'Reconnection keeps the fight paused')
    tap(1)
    frames(70)
    check(read()[1] == 3, 'A fresh confirmation resumes after reconnect')
    command('l')
    frames(3)
    check(read(VERSUS)[2] == 1, 'Reloading the adapter removes old callbacks cleanly')
    select_again()

    start(0, 0)
    capture('mirror-fight')
    win_round(1, True)
    check(read(VERSUS)[7:9] == [0, 1], 'P2 wins its own mirror-match round')
    capture('p2-round-win')
    tap(0, 1)
    frames(65)
    win_round(1)
    check(read(VERSUS)[7:9] == [0, 2] and read(VERSUS)[10], 'P2 wins a best-of-three set')
    capture('p2-set-win')
    tap(1)
    frames(65)
    check(read(VERSUS)[7:9] == [0, 0] and read()[1] == 3, 'One confirmation rematches the set with fresh scores')
    win_round(0)
    check(read(VERSUS)[7:9] == [1, 0], 'P1 can also win a round after a rematch')
    capture('p1-round-win')
    tap(0, 2)
    check(read()[1] == 17 and read(VERSUS)[7:9] == [0, 0], 'P2 can choose Change Pokemon and reset the set')
    start(0, 0)
    frames(3605)
    frames(160)
    check(read()[1] == 6 and read()[20] == 3, 'Equal-life timeout reaches the neutral draw result')
    check(read(VERSUS)[7:9] == [0, 0], 'A native drawn round awards neither player a point')
    capture('draw-result')
    tap(0, 2)
    tap(2)
    check(read()[1] == 15 and not read(VERSUS)[1], 'P1 Back returns to the visible mode menu')
    tap(128)
    check(read(ARCADE)[17] == 3, 'Normal Records remains reachable after Local Versus')
    tap(1)
    frames(8)
    check(read()[1] == 14, 'Normal Records still opens')
    check(missed == 0, 'Zero missed native display frames')

    if movie:
        images = [frame.resize((480, 320), Image.Resampling.NEAREST) for frame in movie]
        images[0].save(OUTPUT / 'local-versus-native.gif', save_all=True,
                       append_images=images[1:], duration=50, loop=0)
finally:
    if process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()

check(process.returncode == 0, 'The one-core input driver exits cleanly')
check(save.read_bytes() == original_save, 'Local Versus leaves existing solo progress byte-identical')
check(not (OUTPUT / 'errors.log').read_text().strip(), 'Actual upstream Lua API completes without script errors')
report = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
              adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(), emulated_gba_instances=1,
              raw_host_key_events=True, combat_state_writes=False, input_mailbox_only=True,
              first_frame_actions=True, roster_per_player=13, locations=5, mirror_matches=True,
              p1_and_p2_wins=True, first_to_two=True, quick_rematch=True,
              pause_and_reconnect=True, save_byte_identical=True,
              peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
              mgba_revision=emulator_info[1], mgba_version=emulator_info[2],
              wall_seconds=round(time.time() - started, 2),
              method='One upstream mGBA 0.11 development core. Actual InputContext key events feed the shipped Lua adapter; only its declared input mailbox is written. Read-only native telemetry and actual framebuffer captures. Physical keyboard/Qt GUI and gamepads remain unverified.')
(OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps(report, indent=2))
