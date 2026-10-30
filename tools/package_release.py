"""Validate the tracked ROM and prepare deterministic GitHub Release assets."""
from pathlib import Path
import argparse, hashlib, json, re, zipfile

REPORTS = ('native-general', 'native-animation', 'native-effects', 'native-feel',
           'native-depth', 'clean-build', 'host-and-assets')


def digest(data):
    return hashlib.sha256(data).hexdigest()


def check(value, message):
    if not value:
        raise ValueError(message)


def json_file(path):
    return json.loads(path.read_text())


def verify_report(value, rom_sha):
    if isinstance(value, dict):
        for key, item in value.items():
            if key in ('missed_frames', 'missed_display_frames'):
                check(item == 0, 'A native report contains missed display frames')
            if key == 'rom_sha256':
                check(item == rom_sha, 'A validation report belongs to another ROM')
            verify_report(item, rom_sha)
    elif isinstance(value, list):
        for item in value:
            verify_report(item, rom_sha)


def prepare(root, output):
    root, output = Path(root), Path(output)
    version = json_file(root / 'package.json')['version']
    check(re.fullmatch(r'\d+\.\d+\.\d+(?:-[A-Za-z0-9.-]+)?', version), 'Invalid release version')
    build = json_file(root / 'docs/build_versions.json')
    check(build['game_release'] == version, 'Package and built release versions differ')
    balance = json_file(root / 'config/balance.json')
    check(build['ruleset'] == balance['ruleset_version'], 'Built and configured rulesets differ')
    rom = (root / 'downloads/PokemonBattleArena.gba').read_bytes()
    check(192 <= len(rom) <= 32 * 1024 * 1024, 'Invalid GBA ROM size')
    check(rom[0xB2] == 0x96 and rom[0xBD] == ((-sum(rom[0xA0:0xBD]) - 0x19) & 255),
          'Invalid GBA header or complement checksum')
    sha = digest(rom)
    check(sha == build['rom_sha256'] and len(rom) == build['rom_bytes'], 'ROM differs from the validated build')
    check((root / 'downloads/SHA256SUMS.txt').read_text().strip() == sha + '  PokemonBattleArena.gba',
          'Tracked ROM checksum is inconsistent')
    evidence = root / 'docs/verification' / ('v' + version)
    for name in REPORTS:
        report = json_file(evidence / (name + '.json'))
        check(report.get('status') == 'passed', name + ' did not pass')
        if name != 'host-and-assets':
            check(report.get('rom_sha256') == sha, name + ' is missing this ROM hash')
        verify_report(report, sha)
    clean = json_file(evidence / 'clean-build.json')
    check(clean.get('rom_byte_identical') and clean.get('generated_inputs_match'), 'Clean rebuild was not identical')
    notes = (root / 'docs/releases' / ('v' + version + '.md')).read_text()
    check('v' + version in notes.splitlines()[0], 'Release notes have the wrong version')
    credits = (root / 'assets/source/CREDITS.md').read_bytes()
    readme = f'''Pokemon Battle Arena v{version}

Open PokemonBattleArena.gba in a Game Boy Advance emulator such as mGBA.
No compiler, network service or separate commercial ROM is required to play.

GBA CONTROLS (keyboard/controller mappings depend on your emulator)
Left / Right: Move
Up: Jump
Down: Timed squeeze dodge
A: Melee
B: Fire
L held: Block ordinary attacks
R: Super at 100 SP and 25 stamina
Start: Pause

PRACTICE
B on the title: Practice setup
R in a selector: Fighter guide
Select in practice setup: Cycle fighter / behavior / rival options
Select in practice: Reset arena, resources and statistics

Clean melee hits can follow with B when the FOLLOW cue appears.
Eevee may also follow with A at close range. One follow-up per sequence.
Supers bypass block and can be evaded with a jump or timed squeeze.

Physical hardware and non-Linux rebuilding remain unverified.
Unofficial fan project, not affiliated with Nintendo, Game Freak or The Pokemon Company.
See CREDITS.md and RELEASE_NOTES.md. Editable source is attached to the GitHub Release.
'''.encode()
    output.mkdir(parents=True, exist_ok=True)
    (output / 'PokemonBattleArena.gba').write_bytes(rom)
    archive = output / ('PokemonBattleArena-v' + version + '.zip')
    with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, data in [('PokemonBattleArena.gba', rom), ('SHA256SUMS.txt', (sha + '  PokemonBattleArena.gba\n').encode()),
                           ('README.txt', readme), ('RELEASE_NOTES.md', notes.encode()), ('CREDITS.md', credits)]:
            entry = zipfile.ZipInfo(name, (1980, 1, 1, 0, 0, 0))
            entry.create_system = 3
            entry.external_attr = 0o100644 << 16
            entry.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(entry, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
    assets = ['PokemonBattleArena.gba', archive.name, 'SHA256SUMS.txt']
    sums = sha + '  PokemonBattleArena.gba\n' + digest(archive.read_bytes()) + '  ' + archive.name + '\n'
    (output / 'SHA256SUMS.txt').write_text(sums)
    result = dict(version=version, tag='v' + version, rom_sha256=sha, rom_bytes=len(rom),
                  title=notes.splitlines()[0].lstrip('# '), assets=assets, prerelease='-' in version,
                  asset_sha256={name: digest((output / name).read_bytes()) for name in assets})
    marker = f'<!-- pkmn-release:{version}:{sha} -->'
    (output / 'release-notes.md').write_text(notes.rstrip() + '\n\nDownload the ROM or player ZIP from Assets below. '
                                           'The ZIP includes controls, notes and credits. Source ZIP/tar archives are provided by GitHub.\n\n' + marker + '\n')
    result['marker'] = marker
    (output / 'release.json').write_text(json.dumps(result, indent=2) + '\n')
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', type=Path, default=Path('dist/release'))
    args = parser.parse_args()
    print(json.dumps(prepare(args.root, args.output), indent=2))
