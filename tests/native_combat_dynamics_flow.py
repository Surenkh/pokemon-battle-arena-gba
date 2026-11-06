"""Validate combat dynamics through one mGBA core and real Local Versus inputs."""
from pathlib import Path
from PIL import Image
import hashlib
import json
import os
import re
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'tests/native_artifacts/combat-dynamics'
OUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
ADAPTER = ROOT / 'tools/local_versus.lua'
SAVE = OUT / 'progress.sav'
words = [0] * 149
words[:5] = [0x504b4d38, 4, 1, 3, 2]
payload = struct.pack('<148I', *words[:-1])
checksum = 2166136261
for byte in payload:
    checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
save = bytearray(32768)
save[1024:1620] = payload + struct.pack('<I', checksum)
SAVE.write_bytes(save)
original = SAVE.read_bytes()
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)

def address(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)

MAIN, VERSUS, TECHNIQUE, DEPTH = map(address, ['pkmn_debug', 'pkmn_versus_debug', 'pkmn_technique_debug', 'pkmn_depth_debug'])
A, B, START, RIGHT, LEFT, UP, DOWN, L = 1, 2, 8, 16, 32, 64, 128, 512
BACKSTEP, CLASH, BLOCK = 11, 20, 5
checks = peak = missed = 0
errors = (OUT / 'errors.log').open('w')
driver = os.environ.get('PKMN_VERSUS_DRIVER', str(ROOT / '.tooling/mgba_versus_driver'))
process = subprocess.Popen([driver, str(ROM), str(ADAPTER), str(SAVE)], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)

def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    answer = process.stdout.readline().strip()
    if not answer:
        raise RuntimeError('Emulator stopped: ' + (OUT / 'errors.log').read_text()[-2000:])
    return answer

def read(ptr=MAIN):
    return list(map(int, command(f's {ptr:08x}').split()[1:]))

def frames(n=1, p1=0, p2=0):
    global peak, missed
    command(f'f {n} {p1} {p2}')
    state = read()
    peak = max(peak, state[21])
    missed = max(missed, state[22])
    return state

def tap(p1=0, p2=0):
    frames(1, p1, p2)
    return frames(3)

def capture(name):
    path = OUT / (name + '.ppm')
    command('d ' + str(path))
    Image.open(path).convert('RGB').save(OUT / (name + '.png'))

def check(condition, message):
    global checks
    checks += 1
    if not condition:
        capture('failure')
        raise AssertionError(message + ' state=' + str(read()) + ' depth=' + str(read(DEPTH)) + ' technique=' + str(read(TECHNIQUE)))

def rematch():
    tap(START)
    tap(DOWN)
    tap(A)
    frames(65)
    check(read()[1] == 3, 'Rematch returns both players to a fresh battle')
    frames(20)

try:
    info = command('i').split()
    check(info[1] == 'a67176cd3e6bf60a5016d7adc94357142dd12617', 'Pinned upstream mGBA input APIs')
    frames(90)
    tap(A)
    for _ in range(4):
        tap(DOWN)
    tap(A)
    check(read()[1] == 17 and read(VERSUS)[2] == 1, 'One emulator opens shared selection')
    while read(VERSUS)[4] != 2:
        tap(0, RIGHT)
    tap(A, A)
    frames(85)
    check(read()[1] == 3 and read()[2:4] == [0, 2], 'Pikachu and Squirtle enter together')

    # Both shots continue after their visual clash and hit the opposing fighter.
    initial_hp = read()[6:8]
    frames(1, B, B)
    clash_count = 0
    last_clash = False
    seen_active = False
    for _ in range(160):
        state = frames()
        t = read(TECHNIQUE)
        clash = bool(t[23] & (1 << CLASH))
        if clash and not last_clash:
            clash_count += 1
            check(state[6:8] == initial_hp, 'Clash itself inflicts no damage')
            seen_active = bool(t[13] and t[14])
            check(seen_active and state[29] >= 2, 'Both shots survive and show elemental bursts')
            capture('projectile-clash')
            frames(3)
            capture('projectile-clash-peak')
        last_clash = clash
        if state[6] < initial_hp[0] and state[7] < initial_hp[1]:
            break
    check(clash_count == 1 and seen_active, 'The same overlapping pair clashes exactly once')
    check(state[6:8] == [initial_hp[0] - 6, initial_hp[1] - 6], 'Both weakened shots travel onward and deal rounded half damage')
    frames(65)
    check(read()[6:8] == state[6:8], 'Each weakened shot hits at most once')

    rematch()
    before = read()
    state = frames(1, DOWN, DOWN)
    check(before[8] - state[8] == 6 * 60 and before[9] - state[9] == 6 * 60,
          'Stationary Down costs six stamina for both seats')
    frames(70)
    before = read()
    state = frames(1, UP, UP)
    check(before[8] - state[8] == 10 * 60 and before[9] - state[9] == 10 * 60,
          'Jump costs ten stamina for both seats')
    frames(4)
    check(read()[14] < before[14] and read()[15] < before[15], 'Both jump into the air')
    frames(70)
    before = read()
    state = frames(1, 0, DOWN | LEFT)
    check(before[9] - state[9] == 14 * 60, 'Squirtle directional Shell Roll costs fourteen total stamina')
    frames(70)

    rematch()
    frames(1, L, L)
    before = read()
    state = frames(1, L | LEFT, L | RIGHT)
    check(state[16:18] == [BACKSTEP, BACKSTEP], 'Both players can backstep with L plus away')
    check(before[8] - state[8] == 6 * 60 and before[9] - state[9] == 6 * 60,
          'Both backsteps spend six stamina')
    frames(5, L | LEFT, L | RIGHT)
    capture('backstep')
    frames(6, L | LEFT, L | RIGHT)
    after = read()
    check(before[12] - after[12] == 15 * 256 and after[13] - before[13] == 15 * 256,
          'Both backsteps make fifteen pixels of space')
    frames(35)

    rematch()
    for _ in range(160):
        state = read()
        if state[13] - state[12] <= 31 * 256:
            break
        frames(1, RIGHT)
    frames(3)
    state = read()
    check(state[13] - state[12] <= 34 * 256, 'Players reach close range')
    frames(1, 0, L)
    before_hp = read()[7]
    frames(1, A, L)
    blocked = False
    for _ in range(25):
        prior = read()
        state = frames(1, 0, L)
        if read(TECHNIQUE)[23] & (1 << BLOCK):
            blocked = True
            check(state[7] == before_hp, 'Guard prevents close attack health damage')
            check(state[13] - prior[13] == 6 * 256, 'A blocked close attack pushes the defender six pixels')
            capture('guard-push')
            break
    check(blocked, 'A real close attack is blocked')
    frames(25)
    check(read()[16] == 0, 'Blocked attacker eventually recovers')
    check(missed == 0, 'No missed native display frames')
finally:
    if process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()

check(process.returncode == 0 and SAVE.read_bytes() == original and not (OUT / 'errors.log').read_text().strip(),
      'Clean exit and unchanged versus save data')
report = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
              adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(), mgba_revision=info[1],
              emulated_gba_instances=1, both_shots_survive_clash=True, weakened_shots_hit=True,
              jump_and_dodge_costs=True, shell_roll_cost=True, both_player_backsteps=True, guard_push=True,
              zero_state_writes=True, save_byte_identical=True, peak_cpu_fraction=peak / 4096,
              missed_display_frames=missed,
              method='One upstream mGBA 0.11 core; real Lua input, native framebuffer, read-only telemetry.')
(OUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps(report, indent=2))
