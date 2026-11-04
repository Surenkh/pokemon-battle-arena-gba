"""Validate the tracked ROM and prepare deterministic GitHub Release assets."""
from pathlib import Path
import argparse, hashlib, json, re, zipfile

REPORTS = ('native-general', 'native-animation', 'native-effects', 'native-feel',
           'native-depth', 'native-cpu', 'native-presentation', 'native-arcade', 'native-mastery', 'native-response', 'native-navigation', 'native-practice', 'native-goals', 'native-performance', 'native-expansion', 'native-v11', 'native-versus', 'native-techniques', 'clean-build', 'host-and-assets')


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
Start/A on title: Visible mode menu
D-pad in menus: Choose a button or fighter
Start/A: Confirm
B: Back
Audio settings are available as a main-menu button; no shoulder shortcut is needed.

AUDIO
Music and sound effects have independent On/Off options.
Settings save when you leave Audio options and survive power-off.
Earlier six- and nine-fighter SRAM records, mastery and audio settings migrate.
The default mix is moderate; emulator/device output volume is separate.

ARCADE
Choose Arcade from the visible mode menu, then difficulty and Pokemon.
Five unique rivals from the other twelve fighters. Two continues.
Results show a mastery increment or the next unfinished goal.
The upcoming/retry rival has one authored trait and counter cue.
A/Start still continues directly; a finished/exhausted run has no next-rival cue.
Woodland, Volcanic, Shoreline, Frostpeak and Ancient Ruins rotate between rivals.
Retries keep the current arena.
Full HP/stamina between fights; draws retry for free.
Pause Rematch costs a continue; changing Pokemon ends the run.
With zero continues Rematch is dimmed and skipped. A finishes an exhausted run.
Records saves clears, no-continue clears and fastest total battle time.
Battle time includes active frames and failed attempts; menus/countdown/recall are excluded.
Unfinished runs do not resume after power-off.

THIRTEEN FIGHTERS
Psyduck: Mind Push adds strong psychic projectile knockback.
Jigglypuff: Float Step gives lighter gravity and longer jumps.
Machop: Heavy Hands adds melee guard-stamina pressure.
Vulpix, Gastly, Riolu and Totodile join the previous roster.
All thirteen fighters have animated movement/attacks, unique themed VFX and sounds.

PRACTICE
Choose Practice from the mode menu
Select in any fighter grid: Open settings directly for that Pokemon
Up/Down in settings: Choose a row. Left/Right: Change its value
B in settings: Return to the same fighter card
A on Fighter Info: Open guide. B: Return to its row
A/Start on grid or value rows: Play. Start also plays from Info
Select in practice: Retry with restored resources/positions and cleared statistics
A fresh action works on emulator frame 8 after reset; Practice KOs restart in 14.
Training options: Free, Block, Attack, Follow-up, Evade Super; Technique for Pikachu/Squirtle/Jigglypuff/Gastly.
Follow-up: Land A, then tap B when FOLLOW B appears.
Evade Super: Let the rival charge, then jump or time Down to avoid its super.
Drills show feedback from actual combat events and never award mastery.

ARENAS AND FIGHTER MASTERY
Quick Battle settings: Difficulty, Location and Fighter Info.
Practice settings: Training, Rival, Location and Fighter Info.
Arcade arena rotation is informational; the cursor skips it.
A check marks the chosen Pokemon, and an outline marks the focused row.
A attacks have a six-frame power-punch animation. Supers have eight-frame auras,
energy-ribbon shots and impact bursts. Ordinary attack timing/base damage and save schema remain compatible.
All arenas share the same floor, hitboxes and combat rules.
Fighter Mastery is a visible main-menu button. Left/Right changes the fighter.
Earn three medals per fighter: 10 damaging follow-up hits, 5 successful super
evasions, and a no-continue Arcade clear at any difficulty. Practice does not count.
Progress saves at the end of a regular round, on Rematch, or when leaving a fight.
Retain your emulator save file. Abrupt power-off during a fight can lose its
unsaved mastery steps. Existing no-continue records already count toward the medal.
Roster badges show the number of medals earned. Mastery/Records link with A.

Clean melee hits can follow with B when the FOLLOW cue appears.
Eevee may also follow with A at close range. One follow-up per sequence.
Supers bypass block and can be evaded with a jump or timed squeeze.

Normal round losses end with a Poke Ball recall. Tap A/Start after the ball
has closed to move to results sooner. Practice retains its quick restart.

Physical hardware and non-Linux rebuilding remain unverified.
Unofficial fan project, not affiliated with Nintendo, Game Freak or The Pokemon Company.
See CREDITS.md and RELEASE_NOTES.md. Editable source is attached to the GitHub Release.
'''.encode()
    bundle_files = [('PokemonBattleArena.gba', rom),
                    ('SHA256SUMS.txt', (sha + '  PokemonBattleArena.gba\n').encode()),
                    ('README.txt', readme), ('RELEASE_NOTES.md', notes.encode()), ('CREDITS.md', credits)]
    if build.get('local_versus'):
        adapter = (root / 'tools/local_versus.lua').read_bytes()
        report = json_file(evidence / 'native-versus.json')
        check(report.get('adapter_sha256') == digest(adapter), 'Input adapter differs from native-tested script')
        check(report.get('emulated_gba_instances') == 1, 'Local Versus must be tested in one emulated GBA')
        guide = (root / 'docs/LOCAL_VERSUS.md').read_bytes()
        introduction = b'''LOCAL VERSUS PREVIEW: TWO KEYBOARD PLAYERS, ONE GAME WINDOW
Use an mGBA 0.11 development build with raw keyboard scripting.
Open PokemonBattleArena.gba, then Tools > Scripting and load local_versus.lua.
Focus the game window. P1 uses arrows, J/K, U/I and Enter; O is Select/options/retry.
P2 uses WASD, F/G, Q/E and T. Choose Local Versus from the visible mode menu.
Both players press Ready. First to two wins takes the set.
Read LOCAL_VERSUS.md for full setup, controls and preview limitations.

'''
        bundle_files[2] = ('README.txt', introduction + readme)
        bundle_files.extend([('local_versus.lua', adapter), ('LOCAL_VERSUS.md', guide)])
    if build.get('fighter_techniques'):
        report = json_file(evidence / 'native-techniques.json')
        check(report.get('signature_fighters') == ['jigglypuff', 'gastly'] and report.get('drills_completed'),
              'Both signature fighters and their drills need native evidence')
        guide = (root / 'docs/TECHNIQUES_PLAYER_GUIDE.md').read_bytes()
        introduction = b'''FIGHTER TECHNIQUES PREVIEW
Jigglypuff: tap Up again during descent for Air Brake (8 stamina).
Gastly: B casts Shadow Orb; a fresh B arms its delayed burst (8 extra stamina).
Practice > Training > Technique teaches both moves. Select retries quickly.
With the keyboard adapter loaded, P1 O is Select for options and retry.
Read FIGHTER_TECHNIQUES.md for timing, costs and counters.

'''
        bundle_files[2] = ('README.txt', introduction + bundle_files[2][1])
        bundle_files.append(('FIGHTER_TECHNIQUES.md', guide))
    if build.get('rival_cup'):
        report = json_file(evidence / 'native-cup.json')
        check(report.get('status') == 'passed' and report.get('rom_sha256') == sha,
              'Rival Cup needs passed evidence for this ROM')
        verify_report(report, sha)
        check(report.get('authored_cups') == 3 and report.get('named_rivals') == 9
              and sorted(entry.get('cup', -1) for entry in report.get('clears', [])) == [0, 1, 2]
              and report.get('earned_style_clear')
              and report.get('journal_power_cycle') and report.get('journal_corruption_fallback')
              and report.get('interrupted_write_fallback') and report.get('base_save_preserved'),
              'Rival Cup needs three real clears, style and independent save recovery evidence')
        guide = (root / 'docs/RIVAL_CUP_PLAYER_GUIDE.md').read_bytes()
        introduction = b'''RIVAL CUP PREVIEW
Three short routes, nine named rivals, fixed locations and optional style goals.
Choose Rival Cup, then Cup, difficulty and Pokemon. A continues to the next rival.
One continue per run. Draws retry free. Cup records save separately from Arcade.
Read RIVAL_CUP.md for goals, controls, achievements and save behavior.

'''
        bundle_files[2] = ('README.txt', introduction + bundle_files[2][1])
        bundle_files.append(('RIVAL_CUP.md', guide))
    if build.get('starter_techniques'):
        check(build.get('fighter_techniques') and build.get('local_versus'),
              'Starter release needs the technique guide and native-tested Versus adapter')
        report = json_file(evidence / 'native-starter.json')
        verify_report(report, sha)
        check(report.get('status') == 'passed' and report.get('rom_sha256') == sha
              and report.get('adapter_sha256') == digest((root / 'tools/local_versus.lua').read_bytes())
              and report.get('signature_fighters') == ['pikachu', 'squirtle']
              and report.get('drills_completed') and report.get('both_player_sides')
              and report.get('first_frame_actions') and report.get('pause_gesture_cleared')
              and report.get('pause_freezes_mobility') and report.get('fast_retry')
              and report.get('save_byte_identical') and report.get('emulated_gba_instances') == 1,
              'Starter techniques require this ROM/adapter, both lessons/sides, pause/retry and save-isolation evidence')
        introduction = b'''STARTER TECHNIQUES
Pikachu: double tap Left/Right for Volt Step (12 stamina).
Squirtle: Down plus Left/Right starts Shell Roll (20 total stamina).
Both add movement without invulnerability. Their Practice Technique lessons are available.
Read FIGHTER_TECHNIQUES.md for timings, recovery, counters and all four signature moves.

'''
        bundle_files[2] = ('README.txt', introduction + bundle_files[2][1])
    output.mkdir(parents=True, exist_ok=True)
    (output / 'PokemonBattleArena.gba').write_bytes(rom)
    archive = output / ('PokemonBattleArena-v' + version + '.zip')
    with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, data in bundle_files:
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
