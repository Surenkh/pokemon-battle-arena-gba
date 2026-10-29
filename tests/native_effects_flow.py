"""Native elemental-effect regression: physical inputs, read-only telemetry."""
from pathlib import Path
import os, subprocess, re, json, time

root = Path(__file__).resolve().parents[1]
out = root / 'tests/native_artifacts/effects'
out.mkdir(parents=True, exist_ok=True)
symbols = subprocess.check_output(['readelf', '-sW', str(root / '.gbastudio/PokemonBattleArena.elf')], text=True)
address = int(re.search(r'\b([0-9a-f]{8})\s+128\s+OBJECT\s+GLOBAL[^\n]+pkmn_debug', symbols).group(1), 16)
checks = 0
report = []
started = time.time()

with (out / 'errors.log').open('w') as errors:
    process = subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER', str(root / 'tests/mgba_driver')), str(root / 'out/PokemonBattleArena.gba')], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=errors, text=True, bufsize=1)
    def command(value):
        process.stdin.write(value + '\n')
        process.stdin.flush()
        result = process.stdout.readline().strip()
        if not result:
            raise RuntimeError('Emulator exited unexpectedly')
        return result
    def frames(count, keys=0):
        command(f'f {count} {keys}')
    def state():
        return list(map(int, command(f's {address:08x}').split()[1:]))
    def press(keys):
        frames(2, keys)
        frames(2)
    def check(condition, message):
        global checks
        checks += 1
        if not condition:
            raise AssertionError(message + ' ' + str(state()))
    try:
        frames(90)
        press(8)
        for character, name in enumerate(['pikachu', 'charmander', 'squirtle', 'bulbasaur', 'eevee', 'meowth']):
            while state()[2] != character:
                press(16)
            masks = themes = supers = peak = particles = missed = 0
            rounds = 0
            for attempt in range(5):
                check(state()[1] == 1, 'Selector before match')
                press(1)
                frames(185)
                press(2)
                # Observe the real travel animation from the initial spacing.
                for tick in range(55):
                    v = state()
                    masks |= v[30]
                    themes |= v[31]
                    check(v[29] <= 12, 'Decorative pool bounded')
                    if tick % 3 == 0:
                        command('d ' + str(out / f'{name}_shot_{tick:02}.ppm'))
                    frames(1)
                line = command(f'a 6000 {address:08x}')
                values = {key: int(value) for key, value in re.findall(r'(\w+)=(\d+)', line)}
                rounds += 1
                masks |= values['effect_frames']
                themes |= values['effect_themes']
                supers += values['supers']
                particles = max(particles, values['peak_particles'])
                peak = max(peak, values['peak_q12'])
                v = state()
                missed = max(missed, v[22])
                check(v[1] == 6, 'Round completes')
                check(particles <= 12, 'Stress pool bounded')
                # Change selection, preserving all observations before renderer cleanup.
                press(16)
                press(1)
                if supers and masks & 15 == 15 and themes & (1 << character) and themes & (1 << (8 + character)):
                    break
            # Close supers can hit before completing a travel cycle.
            check(masks & 15 == 15, 'All four normal travel frames rendered')
            check(supers > 0, 'Player earned and activated a real super')
            check(bool(themes & (1 << character)), 'Character-specific hit effect rendered')
            check(bool(themes & (1 << (8 + character))), 'Character-specific muzzle effect rendered')
            check(missed == 0, 'No missed native display frames')
            report.append(dict(character=name, rounds=rounds, player_supers=supers, effect_frame_mask=masks, theme_mask=themes, peak_particles=particles, peak_cpu_fraction=peak/4096, missed_frames=missed))
            print('Native elemental effects passed:', name, flush=True)
    finally:
        if process.poll() is None:
            process.stdin.write('q\n')
            process.stdin.flush()
            process.wait(timeout=10)

result = dict(status='passed', checks=checks, characters=report, wall_seconds=round(time.time()-started, 2), method='mGBA 0.10.2; physical button inputs; read-only telemetry; no game-memory writes')
(out / 'report.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
