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

    def find(self, tag):
        return copy.deepcopy(self.release)

    def tag_commit(self, tag):
        return self.tag

    def create(self, meta, target, body):
        self.writes.append('draft')
        self.release = dict(id=1, tag_name=meta['tag'], target_commitish=target, body=body,
                            draft=True, assets=[], html_url='https://github.com/test/game/releases/tag/' + meta['tag'])
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

    def test_version_mismatch_cannot_package(self):
        self.js('package.json', dict(version='0.6.0'))
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
