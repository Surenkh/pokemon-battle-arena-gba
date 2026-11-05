"""Check v1.8 feedback with real Local Versus inputs and native pixels."""
from pathlib import Path
from PIL import Image, ImageChops
import hashlib
import json
import os
import re
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'tests/native_artifacts/combat-feedback'
OUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
ADAPTER = ROOT / 'tools/local_versus.lua'
SAVE = OUT / 'progress.sav'
# A valid save prepared before boot enables both audio buses and carries a record.
words = [0] * 149
words[:5] = [0x504b4d38, 4, 1, 3, 2]
payload = struct.pack('<148I', *words[:-1])
checksum = 2166136261
for byte in payload:
    checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
save = bytearray(32768)
save[1024:1620] = payload + struct.pack('<I', checksum)
SAVE.write_bytes(save)
original_save = SAVE.read_bytes()
symbols = subprocess.check_output(['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)

def symbol(name):
    return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', symbols)[1], 16)

MAIN, VERSUS, TECHNIQUE, PRESENTATION = map(symbol, [
    'pkmn_debug', 'pkmn_versus_debug', 'pkmn_technique_debug', 'pkmn_presentation_debug'])
driver = os.environ.get('PKMN_VERSUS_DRIVER', str(ROOT / '.tooling/mgba_versus_driver'))
errors = (OUT / 'errors.log').open('w')
process = subprocess.Popen([driver, str(ROM), str(ADAPTER), str(SAVE)], stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)
checks = peak = missed = 0

def command(value):
    process.stdin.write(value + '\n')
    process.stdin.flush()
    answer = process.stdout.readline().strip()
    if not answer:
        raise RuntimeError('Emulator stopped: ' + (OUT / 'errors.log').read_text()[-2000:])
    return answer

def read(address=MAIN):
    return list(map(int, command(f's {address:08x}').split()[1:]))

def frames(count=1, p1=0, p2=0):
    global peak, missed
    command(f'f {count} {p1} {p2}')
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
    image = Image.open(path).convert('RGB')
    image.save(OUT / (name + '.png'))
    return image

def check(value, description):
    global checks
    checks += 1
    if not value:
        capture('failure')
        raise AssertionError(description + ' state=' + str(read()) + ' audio=' + str(read(PRESENTATION)))

def shoot_until_health(target):
    for _ in range(30):
        if read()[7] <= target:
            return
        before = read()[7]
        frames(1, 2)
        for _ in range(75):
            frames()
            if read()[7] < before:
                break
        frames(60)
    check(False, 'Normal attacks can reach the requested health through real play')

try:
    check(command('i').split()[1] == 'a67176cd3e6bf60a5016d7adc94357142dd12617', 'Pinned upstream mGBA input APIs')
    frames(90)
    tap(1)
    for _ in range(4):
        tap(128)
    tap(1)
    check(read()[1] == 17 and read(VERSUS)[2] == 1, 'Connected shared selection')
    while read(VERSUS)[4] != 2:
        tap(0, 16)
    check(read(VERSUS)[3:5] == [0, 2], 'Pikachu and Squirtle selected independently')
    tap(1, 1)
    frames(65)
    check(read()[1] == 3 and read()[2:4] == [0, 2], 'Both selected fighters enter one battle')
    frames(35)
    check(read(PRESENTATION)[25] == 2 and read(PRESENTATION)[31] == 0, 'Battle music starts in its normal section')

    before = read()[6:8]
    frames(1, 2, 2)
    clashed = False
    for _ in range(80):
        state = frames()
        if read(TECHNIQUE)[23] & (1 << 20):
            clashed = True
            check(state[6:8] == before, 'Normal projectile clash causes no damage')
            check(state[29] >= 2 and state[31] & 5 == 5, 'Both fighter-colored bursts were drawn')
            capture('projectile-clash')
            frames(3)
            capture('projectile-clash-peak')
            break
    check(clashed, 'Opposing shots meet through ordinary button inputs')

    frames(50)
    frames(1, 128, 128)
    dodge_images = []
    dodge_frames = [set(), set()]
    for tick in range(32):
        state = frames()
        for who in range(2):
            if state[27 + who] == 8:
                dodge_frames[who].add(state[25 + who])
        if tick in (6, 10):
            dodge_images.append(capture('dodge-' + str(tick)))
    check(all(len(values) >= 2 for values in dodge_frames), 'Both players animate their compact dodge poses')
    for who in range(2):
        x = read()[12 + who] // 256
        region = (max(0, x - 22), 84, min(240, x + 22), 124)
        check(ImageChops.difference(dodge_images[0].crop(region), dodge_images[1].crop(region)).getbbox() is not None,
              'Visible dodge motion changes around player ' + str(who + 1))

    shoot_until_health(28)
    state = read()
    check(0 < state[7] <= 28 and state[10] == 100, 'Real attacks earn full power and bring the rival near defeat')
    check(read(PRESENTATION)[31] == 1 and read(PRESENTATION)[22] in (2, 3), 'Low health selects the battle finale')
    frames(1, 256)
    super_finish = False
    for _ in range(160):
        state = frames()
        if read(TECHNIQUE)[23] & (1 << 21):
            super_finish = True
        if state[1] == 5:
            break
    check(super_finish and state[1] == 5 and state[20] == 1, 'A real super KO selects the super finish')
    frames(3)
    capture('super-finish')
    frames(160)
    check(read()[1] == 6, 'The normal Poké Ball recall reaches results')

    tap(1)
    frames(65)
    check(read()[1] == 3, 'Next round starts with the same fighters')
    shoot_until_health(8)
    check(0 < read()[7] <= 8, 'A second round reaches a melee finish through real attacks')
    for _ in range(200):
        state = read()
        if (state[13] - state[12]) // 256 <= 33:
            break
        frames(1, 16)
    frames(3)
    state = read()
    check(29 <= (state[13] - state[12]) // 256 <= 35, 'Players are just outside melee reach')
    frames(1, 0, 1)
    frames(9)
    for _ in range(10):
        state = read()
        if (state[13] - state[12]) // 256 <= 26:
            break
        frames(1, 16)
    frames(1, 1)
    punish_finish = False
    for _ in range(40):
        state = frames()
        if read(TECHNIQUE)[23] & (1 << 22):
            punish_finish = True
        if state[1] == 5:
            break
    check(punish_finish and state[1] == 5 and state[20] == 1, 'A real melee recovery punish selects the punish finish')
    frames(3)
    capture('punish-finish')

    frames(160)
    tap(2)
    check(read()[1] == 17, 'Results return to the shared chooser')
    roster_dodges = []
    names = [fighter['id'] for fighter in json.loads((ROOT / 'config/balance.json').read_text())['characters']]
    for character in range(13):
        while read(VERSUS)[3] != character:
            tap(16)
        while read(VERSUS)[4] != (character + 1) % 13:
            tap(0, 16)
        tap(1, 1)
        frames(65)
        check(read()[1] == 3 and read()[2:4] == [character, (character + 1) % 13], 'Both selected roster fighters enter')
        frames(1, 128, 128)
        poses = [set(), set()]
        for tick in range(30):
            state = frames()
            for who in range(2):
                if state[27 + who] == 8:
                    poses[who].add(state[25 + who])
            if tick == 9 and character in (0, 2, 7, 10):
                capture('dodge-' + names[character])
        check(all(len(value) >= 2 for value in poses), 'Both fighters advance through their dodge poses')
        roster_dodges.append(names[character])
        tap(8)
        tap(128)
        tap(128)
        tap(1)
        check(read()[1] == 17, 'Pause returns both players to the chooser')
    check(missed == 0, 'No missed native display frames')
finally:
    if process.poll() is None:
        process.stdin.write('q\n')
        process.stdin.flush()
        process.wait(timeout=10)
    errors.close()

check(SAVE.read_bytes() == original_save, 'Local Versus preserves existing save data')
result = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
              adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(),
              both_projectile_colors=True, both_dodge_sides=True, roster_dodges=roster_dodges, danger_music=True,
              super_finish=True, punish_finish=True, recall_complete=True, save_byte_identical=True,
              peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
              method='One upstream mGBA 0.11 core with real Lua input, native framebuffer and read-only telemetry.')
(OUT / 'report.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
