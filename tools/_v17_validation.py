"""Build evidence for the Breakaway release on the temporary validation branch."""
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

root = Path(__file__).resolve().parents[1]
work = Path(os.environ['RUNNER_TEMP']) / 'v17-validation'
work.mkdir(exist_ok=True)
rom = root / 'out/PokemonBattleArena.gba'
host_suites = ('combat movement feel depth cpu presentation arcade roster mastery practice '
               'goals expansion selection_power local_versus techniques cup starter breakaway').split()
native_suites = ('general animation effects feel depth cpu presentation arcade mastery response navigation practice '
                 'goals performance expansion v11 versus techniques cup starter breakaway').split()

def write(path, data):
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + '\n')

def execute(args, label):
    result = subprocess.run(args, cwd=root, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    (work / (label + '.log')).write_text(result.stdout)
    if result.returncode:
        print(result.stdout[-14000:], flush=True)
        if label.startswith('native-'):
            log = root / 'tests/native_artifacts' / label.removeprefix('native-') / 'errors.log'
            if log.exists():
                print(log.read_text()[-3500:], flush=True)
        if label == 'native-breakaway':
            import base64
            captures = root / 'tests/native_artifacts/breakaway'
            for name in ('practice-ready.png', 'practice-backhop.png', 'failure.png'):
                image = captures / name
                if image.is_file():
                    print('SCREENSHOT ' + name + ' ' + base64.b64encode(image.read_bytes()).decode(), flush=True)
        raise RuntimeError(label + ' failed with exit ' + str(result.returncode))
    print(label + ' passed', flush=True)
    return result.stdout.strip()

def host(name):
    executable = work / ('host-' + name)
    execute(['g++', '-std=c++17', '-O2', '-Iinclude', 'src/pkmn_combat_core.cpp',
             'tests/' + name + '_tests.cpp', '-o', str(executable)], 'compile-' + name)
    return dict(suite=name, status='passed', output=execute([str(executable)], 'host-' + name))

def host_and_assets():
    with ThreadPoolExecutor(max_workers=4) as pool:
        passed = list(pool.map(host, host_suites))
    assets = execute(['npm', 'run', 'test:assets'], 'graphics-assets')
    audio = execute(['npm', 'run', 'test:audio'], 'audio-assets')
    packaging = execute([sys.executable, '-m', 'unittest', 'discover', '-s', 'tests', '-p', 'release_tests.py', '-v'],
                        'packaging-tests')
    write(work / 'host-and-assets.json', dict(status='passed', host_suites=passed,
          asset_tests=dict(status='passed', output=assets), audio_tests=dict(status='passed', output=audio),
          publication_tests=dict(status='passed', output=packaging)))

def native(name):
    script = 'native_flow.py' if name == 'general' else 'native_' + name + '_flow.py'
    execute([sys.executable, 'tests/' + script], 'native-' + name)

def run_native():
    native('breakaway')
    native('depth')
    with ThreadPoolExecutor(max_workers=4) as pool:
        list(pool.map(native, [name for name in native_suites if name not in {'breakaway', 'depth'}]))

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def assemble():
    evidence = root / 'docs/verification/v1.7.0'
    evidence.mkdir(exist_ok=True)
    verified_hash = sha(rom)
    native_dir = root / 'tests/native_artifacts'
    sys.path.insert(0, str(root / 'tools'))
    from package_release import verify_report
    count = 0
    for name in native_suites:
        path = (native_dir / 'native_test_report.json' if name == 'general'
                else native_dir / name / 'report.json')
        data = json.loads(path.read_text())
        if data['status'] != 'passed':
            raise RuntimeError('Native suite did not pass: ' + name)
        data.setdefault('rom_sha256', verified_hash)
        verify_report(data, verified_hash)
        write(evidence / ('native-' + name + '.json'), data)
        count += data.get('checks', 0)

    rebuilt = Path(os.environ['RUNNER_TEMP']) / 'v17-rebuild'
    identical = (rebuilt / 'out/PokemonBattleArena.gba').read_bytes() == rom.read_bytes()
    matched = True
    for folder in ('graphics', 'content', 'audio'):
        generated = {p.relative_to(root): sha(p) for p in (root / folder).rglob('*') if p.is_file()}
        regenerated = {p.relative_to(rebuilt): sha(p) for p in (rebuilt / folder).rglob('*') if p.is_file()}
        matched = matched and generated == regenerated
    for header in ('pkmn_character_defs.h', 'pkmn_cpu_defs.h', 'pkmn_technique_defs.h',
                   'pkmn_selection_layout.h', 'pkmn_cup_defs.h'):
        matched = matched and sha(root / 'include' / header) == sha(rebuilt / 'include' / header)
    if not identical or not matched:
        raise RuntimeError('Clean build or generated inputs differed')
    write(evidence / 'clean-build.json', dict(status='passed', rom_sha256=verified_hash,
          rom_bytes=rom.stat().st_size, rom_byte_identical=identical, generated_inputs_match=matched,
          method='Independent clean source snapshot with regenerated assets, audio and config. Pinned devkitARM 16.1.0.'))
    shutil.copyfile(work / 'host-and-assets.json', evidence / 'host-and-assets.json')
    for image in ('practice-ready.png', 'practice-backhop.png', 'practice-complete.png',
                  'versus-p1-backhop.png', 'versus-p2-backhop.png'):
        shutil.copyfile(native_dir / 'breakaway' / image, evidence / image)

    build_path = root / 'docs/build_versions.json'
    build = json.loads(build_path.read_text())
    build.update(game_release='1.7.0', ruleset='1.7.0', rom_sha256=verified_hash,
                 rom_bytes=rom.stat().st_size, breakaway=True, breakaway_power_cost=60)
    write(build_path, build)
    shutil.copyfile(rom, root / 'downloads/PokemonBattleArena.gba')
    (root / 'downloads/SHA256SUMS.txt').write_text(verified_hash + '  PokemonBattleArena.gba\n')
    execute([sys.executable, 'tools/package_release.py'], 'package-release')
    shutil.copyfile(root / 'dist/release/PokemonBattleArena-v1.7.0.zip',
                    root / 'downloads/PokemonBattleArena-v1.7.0.zip')
    print(f'{len(host_suites)} host suites, {len(native_suites)} native suites, {count} native checks', flush=True)
    print('ROM SHA-256 ' + verified_hash, flush=True)

{'host': host_and_assets, 'native': run_native, 'assemble': assemble}[sys.argv[1]]()
