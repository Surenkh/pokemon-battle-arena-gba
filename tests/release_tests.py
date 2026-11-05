"""Publication integrity, deterministic bundles and immutable version behavior."""
from pathlib import Path
import copy, json, sys, tempfile, unittest, zipfile
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from package_release import REPORTS, digest, prepare
from publish_release import publish


class FakeGitHub:
    def __init__(self):
        self.release = None
        self.tag = None
        self.data = {}
        self.writes = []
        self.interrupt = False
        self.latest = None

    def find(self, tag):
        return copy.deepcopy(self.release)

    def tag_commit(self, tag):
        return self.tag

    def create(self, meta, target, body):
        self.writes.append('draft')
        self.release = dict(id=1, tag_name=meta['tag'], target_commitish=target, body=body,
                            draft=True, prerelease=meta['prerelease'], assets=[], html_url='https://github.com/test/game/releases/tag/' + meta['tag'])
        return copy.deepcopy(self.release)

    def upload(self, release, name, data):
        if self.interrupt and self.data:
            self.interrupt = False
            raise RuntimeError('Simulated network interruption')
        self.writes.append('upload:' + name)
        asset = dict(id=len(self.data) + 1, name=name)
        self.data[asset['id']] = data
        self.release['assets'].append(asset)
        return asset

    def delete_asset(self, asset):
        self.writes.append('delete:' + asset['name'])
        self.release['assets'].remove(asset)
        del self.data[asset['id']]

    def download(self, asset):
        return self.data[asset['id']]

    def finish(self, release, latest):
        self.writes.append('publish')
        self.release['draft'] = False
        self.tag = release['target_commitish']
        self.latest = latest
        return copy.deepcopy(self.release)


class ReleaseTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / 'repo'
        self.output = Path(self.temp.name) / 'release'
        self.target = 'a' * 40
        rom = bytearray(256)
        rom[0xb2] = 0x96
        rom[0xbd] = (-sum(rom[0xa0:0xbd]) - 0x19) & 255
        self.sha = digest(rom)
        self.write('downloads/PokemonBattleArena.gba', rom)
        self.write('downloads/SHA256SUMS.txt', self.sha + '  PokemonBattleArena.gba\n')
        self.js('package.json', dict(version='0.5.0'))
        self.js('config/balance.json', dict(ruleset_version='0.5.0'))
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0', rom_sha256=self.sha, rom_bytes=256))
        self.write('docs/releases/v0.5.0.md', '# Game v0.5.0\n\nRelease notes.\n')
        self.write('assets/source/CREDITS.md', 'Credits\n')
        for name in REPORTS:
            self.js('docs/verification/v0.5.0/' + name + '.json', dict(status='passed', rom_sha256=self.sha,
                    missed_display_frames=0, generated_inputs_match=True, rom_byte_identical=True))

    def write(self, name, data):
        p = self.root / name
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_bytes(data if isinstance(data, (bytes, bytearray)) else data.encode())

    def js(self, name, data):
        self.write(name, json.dumps(data))

    def test_bundle_is_deterministic_and_contains_playable_rom(self):
        meta = prepare(self.root, self.output)
        other = Path(self.temp.name) / 'other'
        prepare(self.root, other)
        for name in meta['assets']:
            self.assertEqual((self.output / name).read_bytes(), (other / name).read_bytes())
        with zipfile.ZipFile(self.output / meta['assets'][1]) as z:
            self.assertEqual(digest(z.read('PokemonBattleArena.gba')), self.sha)
            self.assertEqual(set(z.namelist()), {'PokemonBattleArena.gba', 'SHA256SUMS.txt', 'README.txt', 'RELEASE_NOTES.md', 'CREDITS.md'})

    def test_corrupted_or_mismatched_build_cannot_package(self):
        self.write('downloads/PokemonBattleArena.gba', b'wrong')
        with self.assertRaises(ValueError): prepare(self.root, self.output)

    def test_local_versus_bundle_contains_exact_native_tested_adapter(self):
        adapter = b'-- native-tested input adapter\n'
        self.write('tools/local_versus.lua', adapter)
        self.write('docs/LOCAL_VERSUS.md', '# Local Versus\nKeyboard setup.\n')
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0',
                rom_sha256=self.sha, rom_bytes=256, local_versus=True))
        self.js('docs/verification/v0.5.0/native-versus.json', dict(status='passed',
                rom_sha256=self.sha, adapter_sha256=digest(adapter), emulated_gba_instances=1))
        meta = prepare(self.root, self.output)
        with zipfile.ZipFile(self.output / meta['assets'][1]) as archive:
            self.assertEqual(archive.read('local_versus.lua'), adapter)
            self.assertIn('LOCAL_VERSUS.md', archive.namelist())
            self.assertIn(b'TWO KEYBOARD PLAYERS', archive.read('README.txt'))
        self.write('tools/local_versus.lua', b'-- changed after testing\n')
        with self.assertRaises(ValueError):
            prepare(self.root, self.output)

    def test_technique_bundle_requires_both_native_tested_fighters_and_drills(self):
        guide = b'# Fighter techniques\nAir Brake and Shadow Burst.\n'
        self.write('docs/TECHNIQUES_PLAYER_GUIDE.md', guide)
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0',
                rom_sha256=self.sha, rom_bytes=256, fighter_techniques=True))
        report = dict(status='passed', rom_sha256=self.sha,
                      signature_fighters=['jigglypuff', 'gastly'], drills_completed=True)
        self.js('docs/verification/v0.5.0/native-techniques.json', report)
        meta = prepare(self.root, self.output)
        with zipfile.ZipFile(self.output / meta['assets'][1]) as archive:
            self.assertEqual(archive.read('FIGHTER_TECHNIQUES.md'), guide)
            self.assertIn(b'FIGHTER TECHNIQUES PREVIEW', archive.read('README.txt'))
        for change in [dict(signature_fighters=['gastly']), dict(drills_completed=False)]:
            self.js('docs/verification/v0.5.0/native-techniques.json', dict(report, **change))
            with self.assertRaises(ValueError):
                prepare(self.root, self.output)

    def test_version_mismatch_cannot_package(self):
        self.js('package.json', dict(version='0.6.0'))
        with self.assertRaises(ValueError): prepare(self.root, self.output)

    def starter_fixture(self):
        adapter = b'-- tested adapter\n'
        self.write('tools/local_versus.lua', adapter)
        self.write('docs/LOCAL_VERSUS.md', '# Local Versus\n')
        self.write('docs/TECHNIQUES_PLAYER_GUIDE.md', '# Four techniques\nVolt Step, Shell Roll, Air Brake, Shadow Burst.\n')
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0', rom_sha256=self.sha,
                rom_bytes=256, local_versus=True, fighter_techniques=True, starter_techniques=True))
        self.js('docs/verification/v0.5.0/native-versus.json', dict(status='passed', rom_sha256=self.sha,
                adapter_sha256=digest(adapter), emulated_gba_instances=1))
        self.js('docs/verification/v0.5.0/native-techniques.json', dict(status='passed', rom_sha256=self.sha,
                signature_fighters=['jigglypuff', 'gastly'], drills_completed=True))
        report = dict(status='passed', rom_sha256=self.sha, adapter_sha256=digest(adapter),
                      signature_fighters=['pikachu', 'squirtle'], emulated_gba_instances=1,
                      drills_completed=True, both_player_sides=True, first_frame_actions=True,
                      pause_gesture_cleared=True, pause_freezes_mobility=True, fast_retry=True,
                      save_byte_identical=True, missed_display_frames=0)
        self.js('docs/verification/v0.5.0/native-starter.json', report)
        return report

    def test_starter_bundle_includes_both_moves_and_matching_native_evidence(self):
        self.starter_fixture()
        meta = prepare(self.root, self.output)
        with zipfile.ZipFile(self.output / meta['assets'][1]) as archive:
            for text in [b'Volt Step (12 stamina)', b'Shell Roll (20 total stamina)']:
                self.assertIn(text, archive.read('README.txt'))
            self.assertIn(b'Four techniques', archive.read('FIGHTER_TECHNIQUES.md'))

    def test_starter_publication_rejects_partial_stale_or_modified_validation(self):
        report = self.starter_fixture()
        for change in [dict(status='failed'), dict(rom_sha256='0'*64), dict(adapter_sha256='0'*64),
                       dict(signature_fighters=['pikachu']), dict(emulated_gba_instances=2),
                       dict(drills_completed=False), dict(both_player_sides=False),
                       dict(first_frame_actions=False), dict(pause_gesture_cleared=False),
                       dict(pause_freezes_mobility=False), dict(fast_retry=False),
                       dict(save_byte_identical=False), dict(missed_display_frames=1)]:
            with self.subTest(change=change):
                self.js('docs/verification/v0.5.0/native-starter.json', dict(report, **change))
                with self.assertRaises(ValueError): prepare(self.root, self.output)

    def test_cup_bundle_requires_real_clears_and_save_recovery_for_this_rom(self):
        guide = b'# Rival Cup\nThree routes and optional style goals.\n'
        self.write('docs/RIVAL_CUP_PLAYER_GUIDE.md', guide)
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0',
                rom_sha256=self.sha, rom_bytes=256, rival_cup=True))
        report = dict(status='passed', rom_sha256=self.sha, authored_cups=3, named_rivals=9,
                      clears=[dict(cup=i) for i in range(3)], earned_style_clear=True,
                      journal_power_cycle=True, journal_corruption_fallback=True,
                      interrupted_write_fallback=True, base_save_preserved=True, missed_display_frames=0)
        self.js('docs/verification/v0.5.0/native-cup.json', report)
        meta = prepare(self.root, self.output)
        with zipfile.ZipFile(self.output / meta['assets'][1]) as archive:
            self.assertEqual(archive.read('RIVAL_CUP.md'), guide)
            self.assertIn(b'RIVAL CUP PREVIEW', archive.read('README.txt'))
        for change in [dict(clears=[]), dict(earned_style_clear=False), dict(journal_power_cycle=False),
                       dict(journal_corruption_fallback=False), dict(interrupted_write_fallback=False),
                       dict(base_save_preserved=False), dict(rom_sha256='0' * 64), dict(missed_display_frames=1)]:
            self.js('docs/verification/v0.5.0/native-cup.json', dict(report, **change))
            with self.assertRaises(ValueError): prepare(self.root, self.output)

    def test_cup_brackets_require_choices_captain_tactics_and_persistent_crests(self):
        self.write('docs/RIVAL_CUP_PLAYER_GUIDE.md', '# Rival Cup\nChoose two challengers.\n')
        self.js('docs/build_versions.json', dict(game_release='0.5.0', ruleset='0.5.0',
                rom_sha256=self.sha, rom_bytes=256, rival_cup=True, rival_cup_brackets=True))
        report = dict(status='passed', rom_sha256=self.sha, authored_cups=3, named_rivals=12,
                      clears=[dict(cup=i) for i in range(3)], earned_style_clear=True, journal_power_cycle=True,
                      journal_corruption_fallback=True, interrupted_write_fallback=True, base_save_preserved=True,
                      branching_choices=True, defeated_challenger_skipped=True, retry_rival_locked=True,
                      captain_tactics=[dict(cup=i, tactic=tactic) for i, tactic in enumerate(('RUSH', 'GUARD', 'ZONE'))],
                      earned_crests_power_cycle=True, missed_display_frames=0)
        path = 'docs/verification/v0.5.0/native-cup.json'
        self.js(path, report)
        meta = prepare(self.root, self.output)
        with zipfile.ZipFile(self.output / meta['assets'][1]) as archive:
            self.assertIn(b'Pick two challengers', archive.read('README.txt'))
        for change in [dict(named_rivals=9), dict(branching_choices=False), dict(defeated_challenger_skipped=False),
                       dict(retry_rival_locked=False), dict(captain_tactics=report['captain_tactics'][:2]),
                       dict(earned_crests_power_cycle=False)]:
            with self.subTest(change=change):
                self.js(path, dict(report, **change))
                with self.assertRaises(ValueError): prepare(self.root, self.output)

    def test_failed_stale_or_missed_native_evidence_is_rejected(self):
        name = 'docs/verification/v0.5.0/native-general.json'
        for report in [dict(status='failed', rom_sha256=self.sha), dict(status='passed', rom_sha256='0'*64),
                       dict(status='passed', rom_sha256=self.sha, characters=[dict(missed_frames=1)])]:
            with self.subTest(report=report):
                self.js(name, report)
                with self.assertRaises(ValueError): prepare(self.root, self.output)

    def test_uploads_are_verified_before_publication_and_rerun_is_read_only(self):
        prepare(self.root, self.output)
        remote = FakeGitHub()
        result = publish(remote, self.output, self.target)
        self.assertFalse(result['already_published'])
        self.assertEqual(remote.writes[-1], 'publish')
        self.assertEqual(len(remote.data), 3)
        self.assertTrue(remote.latest)
        before = list(remote.writes)
        self.assertTrue(publish(remote, self.output, self.target)['already_published'])
        self.assertEqual(remote.writes, before)

    def test_release_candidate_is_a_prerelease_and_never_replaces_latest_stable(self):
        version = '1.0.0-rc.1'
        self.js('package.json', dict(version=version))
        self.js('docs/build_versions.json', dict(game_release=version, ruleset='0.5.0', rom_sha256=self.sha, rom_bytes=256))
        self.write('docs/releases/v' + version + '.md', '# Game v' + version + '\n\nRelease candidate.\n')
        for name in REPORTS:
            self.js('docs/verification/v' + version + '/' + name + '.json', dict(status='passed', rom_sha256=self.sha,
                    missed_display_frames=0, generated_inputs_match=True, rom_byte_identical=True))
        meta = prepare(self.root, self.output)
        self.assertTrue(meta['prerelease'])
        remote = FakeGitHub()
        publish(remote, self.output, self.target)
        self.assertTrue(remote.release['prerelease'])
        self.assertFalse(remote.latest)
        before = list(remote.writes)
        self.assertTrue(publish(remote, self.output, self.target)['already_published'])
        self.assertEqual(remote.writes, before)

    def test_interrupted_upload_stays_draft_and_can_resume(self):
        prepare(self.root, self.output)
        remote = FakeGitHub(); remote.interrupt = True
        with self.assertRaises(RuntimeError): publish(remote, self.output, self.target)
        self.assertTrue(remote.release['draft'])
        self.assertNotIn('publish', remote.writes)
        publish(remote, self.output, self.target)
        self.assertFalse(remote.release['draft'])
        self.assertEqual(remote.writes.count('draft'), 1)

    def test_changed_prepared_asset_is_rejected_before_any_write(self):
        prepare(self.root, self.output)
        (self.output / 'PokemonBattleArena.gba').write_bytes(b'changed')
        remote = FakeGitHub()
        with self.assertRaises(ValueError): publish(remote, self.output, self.target)
        self.assertEqual(remote.writes, [])

    def test_existing_tag_and_unrelated_draft_are_not_overwritten(self):
        prepare(self.root, self.output)
        remote = FakeGitHub(); remote.tag = 'b' * 40
        with self.assertRaises(ValueError): publish(remote, self.output, self.target)
        self.assertEqual(remote.writes, [])
        remote.tag = None
        remote.release = dict(draft=True, body='A user draft', target_commitish=self.target)
        with self.assertRaises(ValueError): publish(remote, self.output, self.target)
        self.assertEqual(remote.writes, [])

    def test_published_wrong_rom_is_never_replaced(self):
        prepare(self.root, self.output)
        remote = FakeGitHub(); publish(remote, self.output, self.target)
        before = list(remote.writes)
        rom = next(a for a in remote.release['assets'] if a['name'] == 'PokemonBattleArena.gba')
        remote.data[rom['id']] = b'corrupt'
        with self.assertRaises(ValueError): publish(remote, self.output, self.target)
        self.assertEqual(remote.writes, before)


if __name__ == '__main__': unittest.main()
