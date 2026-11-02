"""Measure sampled taps, visible windup and projectile release in the real ROM."""
from pathlib import Path
from PIL import Image, ImageChops
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tests/native_artifacts/response'
OUTPUT.mkdir(parents=True, exist_ok=True)
ROM = ROOT / 'out/PokemonBattleArena.gba'
SYMBOLS = subprocess.check_output(
    ['readelf', '-sW', str(ROOT / '.gbastudio/PokemonBattleArena.elf')], text=True)
BALANCE = json.loads((ROOT / 'config/balance.json').read_text())
CHARACTERS = BALANCE['characters']
BUTTONS = dict(melee=1, fire=2, jump=64, squeeze=128, guard=512, super=256, right=16)
PLAYER_REGION = (32, 62, 97, 125)
checks = 0
peak = 0
missed = 0


def check(condition, message):
    global checks
    checks += 1
    if not condition:
        raise AssertionError(message)


def symbol(name):
    match = re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b' + name + r'\b', SYMBOLS)
    return int(match.group(1), 16)


MAIN = symbol('pkmn_debug')
FEEL = symbol('pkmn_feel_debug')
DEPTH = symbol('pkmn_depth_debug')
ARCADE = symbol('pkmn_arcade_debug')


class Emulator:
    def __init__(self):
        self.errors = (OUTPUT / 'errors.log').open('a')
        self.process = subprocess.Popen(
            [os.environ.get('PKMN_MGBA_DRIVER', str(ROOT / 'tests/mgba_driver')), str(ROM)],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=self.errors,
            text=True, bufsize=1)
        self.frames(90)

    def command(self, value):
        self.process.stdin.write(value + '\n')
        self.process.stdin.flush()
        line = self.process.stdout.readline().strip()
        if not line:
            raise RuntimeError('Emulator stopped. See ' + str(OUTPUT / 'errors.log'))
        return line

    def read(self, address):
        operation = 'v' if address == FEEL else 's'
        return list(map(int, self.command(f'{operation} {address:08x}').split()[1:]))

    def frames(self, count=1, keys=0):
        global peak, missed
        self.command(f'f {count} {keys}')
        state = self.read(MAIN)
        peak = max(peak, state[21])
        missed = max(missed, state[22])
        return state

    def tap(self, keys):
        self.frames(keys=keys)
        self.frames(3)

    def practice(self, character, phase=0):
        self.tap(2)
        for _ in range(character):
            self.tap(16)
        check(self.read(MAIN)[1:3] == [9, character], 'Correct fighter in Practice setup')
        self.tap(1)
        self.frames(200 + phase)
        check(self.read(MAIN)[1] == 3 and self.read(DEPTH)[3] == 0, 'Active Practice with idle dummy')

    def capture(self, name='current'):
        path = OUTPUT / (name + '.ppm')
        self.command('d ' + str(path))
        with Image.open(path) as picture:
            return picture.convert('RGB')

    def close(self):
        if self.process.poll() is None:
            self.process.stdin.write('q\n')
            self.process.stdin.flush()
            self.process.wait(timeout=10)
        self.errors.close()
        check(self.process.returncode == 0, 'Emulator exits cleanly')


def replay(character, button, phase):
    game = Emulator()
    try:
        game.practice(character, phase)
        index = 14 if button == 64 else 12 if button == 16 else 16
        original = game.read(MAIN)[index]
        images = []
        accepted = None
        release = None
        for frame in range(1, 37):
            state = game.frames(keys=button if frame == 1 else 0)
            if state[index] != original and accepted is None:
                accepted = frame
            events = game.read(FEEL)[1]
            event = 1 if button == 2 else 3
            if button in (2, 256) and events & (1 << event) and release is None:
                release = frame
            picture = game.capture()
            images.append(picture.crop(PLAYER_REGION))
            if character in (0, 8) and button == 2 and phase == 0 and frame in (1, 2, 8, 14):
                picture.save(OUTPUT / f'{CHARACTERS[character]["id"]}-fire-{frame:02}.png')
        return images, accepted, release
    finally:
        game.close()


def rejected_and_held(character):
    game = Emulator()
    try:
        game.practice(character)
        state = game.frames(keys=2)
        check(state[16] == 3, 'Initial shot starts')
        attack_tick = game.read(DEPTH)[27]
        while state[16] == 3:
            state = game.frames(keys=2)
        check(state[7] == CHARACTERS[(character + 1) % 9]['max_hp'], 'First shot is still traveling')
        game.frames()
        state = game.frames(keys=2)
        check(state[16] != 3 and state[27] != 5, 'Occupied projectile slot rejects B without a charge pose')
        for _ in range(90):
            game.frames(keys=2)
        check(game.read(DEPTH)[27] == attack_tick, 'Holding B never creates another attack')

        # Spend guard stamina through physical input, then release guard before B.
        while game.read(MAIN)[8] >= BALANCE['actions']['fire']['stamina_cost'] * 60:
            game.frames(keys=512)
        game.frames()
        state = game.frames(keys=2)
        check(state[16] != 3 and state[27] != 5, 'Insufficient stamina never displays an accepted firing cue')
        check(game.read(FEEL)[9] > 0, 'Rejected input has the existing stamina feedback')
    finally:
        game.close()


results = []
for character, definition in enumerate(CHARACTERS):
    for phase in (0, 4, 8, 12):
        idle, _, _ = replay(character, 0, phase)
        actions = BUTTONS if phase == 0 else dict(fire=2)
        for action, button in actions.items():
            pictures, accepted, released = replay(character, button, phase)
            visible = next((frame for frame, (before, after) in enumerate(zip(idle, pictures), 1)
                            if ImageChops.difference(before, after).getbbox()), None)
            description = f'{definition["name"]} {action} phase {phase}'
            check(accepted == 1, description + ' accepted on the first sampled frame')
            check(visible == 2, description + ' visibly responds on frame two')
            if action == 'fire':
                check(released == definition['fire']['startup_ticks'] + 1, description + ' retains projectile timing')
            if action == 'super':
                check(released == BALANCE['actions']['super']['startup_ticks'] + 1, description + ' retains super timing')
            results.append(dict(character=definition['id'], action=action, phase=phase,
                                accepted_frame=accepted, visible_frame=visible, release_frame=released))
    rejected_and_held(character)

# Check the real runtime font, including every mode hint's side margins.
game = Emulator()
try:
    game.tap(8)
    for row, y in enumerate((41, 56, 71, 96, 108, 120, 132)):
        game.frames(6)
        check(game.read(ARCADE)[17] == row, 'Expected visible mode row')
        picture = game.capture()
        for left, right in ((0, 8), (232, 240)):
            edge = picture.crop((left, y - 5, right, y + 6))
            check(all(min(pixel) < 180 for pixel in edge.getdata()), 'Mode hint stays within side margins')
        if row == 6:
            picture.save(OUTPUT / 'mastery-menu.png')
        game.tap(128)
finally:
    game.close()

check(missed == 0, 'Zero missed native display frames')
report = dict(status='passed', checks=checks, rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),
              sampled_trials=len(results), cases=results, held_and_rejected_fighters=len(CHARACTERS),
              menu_hints=7, peak_cpu_fraction=peak / 4096, missed_display_frames=missed,
              method='mGBA 0.10.2; one-frame physical inputs and matched no-input framebuffer replays; read-only telemetry')
(OUTPUT / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps({key: value for key, value in report.items() if key != 'cases'}, indent=2))
