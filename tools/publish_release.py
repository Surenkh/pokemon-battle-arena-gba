"""Publish verified assets with the repository-scoped GitHub Actions token."""
from pathlib import Path
import argparse, json, os, re, urllib.error, urllib.parse, urllib.request
from package_release import check, digest


class GitHub:
    def __init__(self, repository, token):
        check(re.fullmatch(r'[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+', repository), 'Invalid repository')
        check(token, 'Run the publisher in GitHub Actions with its built-in token')
        self.repository, self.token = repository, token
        self.api = 'https://api.github.com/repos/' + repository

    def request(self, url, method='GET', data=None, content_type='application/json', binary=False, missing=False):
        if isinstance(data, dict):
            data = json.dumps(data).encode()
        headers = {'Accept': 'application/octet-stream' if binary else 'application/vnd.github+json',
                   'X-GitHub-Api-Version': '2022-11-28', 'User-Agent': 'pokemon-battle-arena-release'}
        if data is not None:
            headers['Content-Type'] = content_type
        req = urllib.request.Request(url, data=data, headers=headers, method=method)
        # Asset downloads redirect to storage hosts. Never forward credentials.
        req.add_unredirected_header('Authorization', 'Bearer ' + self.token)
        try:
            with urllib.request.urlopen(req, timeout=60) as response:
                body = response.read()
        except urllib.error.HTTPError as error:
            if missing and error.code == 404:
                return None
            raise RuntimeError(f'GitHub returned HTTP {error.code} for {method} {urllib.parse.urlparse(url).path}') from None
        return body if binary else json.loads(body) if body else None

    def find(self, tag):
        for page in range(1, 101):
            items = self.request(self.api + f'/releases?per_page=100&page={page}')
            found = [item for item in items if item['tag_name'] == tag]
            check(len(found) <= 1, 'Multiple releases use this tag')
            if found:
                return found[0]
            if len(items) < 100:
                return None
        raise ValueError('Release history requires manual inspection')

    def tag_commit(self, tag):
        item = self.request(self.api + '/git/ref/tags/' + urllib.parse.quote(tag, safe=''), missing=True)
        if item is None:
            return None
        obj = item['object']
        for _ in range(8):
            if obj['type'] == 'commit':
                return obj['sha']
            check(obj['type'] == 'tag', 'Tag does not identify a commit')
            obj = self.request(self.api + '/git/tags/' + obj['sha'])['object']
        raise ValueError('Tag nesting is too deep')

    def create(self, meta, target, body):
        return self.request(self.api + '/releases', 'POST', dict(tag_name=meta['tag'], target_commitish=target,
                            name=meta['title'], body=body, draft=True, prerelease=meta['prerelease']))

    def upload(self, release, name, data):
        url = 'https://uploads.github.com/repos/' + self.repository + '/releases/' + str(release['id']) + '/assets?name=' + urllib.parse.quote(name, safe='')
        return self.request(url, 'POST', data, 'application/octet-stream')

    def delete_asset(self, asset):
        self.request(self.api + '/releases/assets/' + str(asset['id']), 'DELETE')

    def download(self, asset):
        return self.request(self.api + '/releases/assets/' + str(asset['id']), binary=True)

    def finish(self, release, latest):
        return self.request(self.api + '/releases/' + str(release['id']), 'PATCH', dict(draft=False, make_latest='true' if latest else 'false'))


def verify_published(client, release, meta):
    assets = {asset['name']: asset for asset in release['assets']}
    check(all(name in assets for name in meta['assets']), 'Published release is missing an expected asset')
    sums = client.download(assets['SHA256SUMS.txt']).decode().splitlines()
    expected = {}
    for line in sums:
        sha, name = line.split('  ', 1)
        check(re.fullmatch(r'[0-9a-f]{64}', sha), 'Published checksum is invalid')
        expected[name] = sha
    check(expected.get('PokemonBattleArena.gba') == meta['rom_sha256'], 'Published version contains another ROM; use a new version')
    for name in meta['assets']:
        if name != 'SHA256SUMS.txt':
            check(name in expected and digest(client.download(assets[name])) == expected[name], 'Published asset checksum failed: ' + name)


def publish(client, directory, target):
    directory = Path(directory)
    meta = json.loads((directory / 'release.json').read_text())
    check(re.fullmatch(r'[0-9a-f]{40}', target), 'Release target must be a complete commit SHA')
    check(meta['assets'] == ['PokemonBattleArena.gba', 'PokemonBattleArena-v' + meta['version'] + '.zip', 'SHA256SUMS.txt'], 'Unexpected release assets')
    for name in meta['assets']:
        check(digest((directory / name).read_bytes()) == meta['asset_sha256'][name], 'Prepared asset changed: ' + name)
    check(meta['asset_sha256']['PokemonBattleArena.gba'] == meta['rom_sha256'], 'Prepared ROM hash is inconsistent')
    release = client.find(meta['tag'])
    if release and not release['draft']:
        verify_published(client, release, meta)
        return dict(url=release['html_url'], tag=meta['tag'], already_published=True)
    existing_tag = client.tag_commit(meta['tag'])
    check(existing_tag in (None, target), 'Tag already points at another commit; it will not be moved')
    body = (directory / 'release-notes.md').read_text()
    if release:
        check(meta['marker'] in release.get('body', '') and release['target_commitish'] == target,
              'An unrelated draft exists; it will not be overwritten')
    else:
        release = client.create(meta, target, body)
    existing = {asset['name']: asset for asset in release.get('assets', [])}
    for name in meta['assets']:
        data = (directory / name).read_bytes()
        if name in existing:
            if client.download(existing[name]) == data:
                continue
            client.delete_asset(existing[name])
        client.upload(release, name, data)
    # Re-fetch and verify every uploaded byte before making the release public.
    release = client.find(meta['tag'])
    for name in meta['assets']:
        asset = next((item for item in release['assets'] if item['name'] == name), None)
        check(asset is not None and client.download(asset) == (directory / name).read_bytes(), 'Draft upload verification failed: ' + name)
    release = client.finish(release, latest=not meta['prerelease'])
    check(client.tag_commit(meta['tag']) == target, 'Published tag target differs from the requested commit')
    return dict(url=release['html_url'], tag=meta['tag'], already_published=False)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--directory', type=Path, default=Path('dist/release'))
    parser.add_argument('--repository', default=os.environ.get('GITHUB_REPOSITORY'))
    parser.add_argument('--target', default=os.environ.get('GITHUB_SHA'))
    args = parser.parse_args()
    client = GitHub(args.repository or '', os.environ.get('GH_TOKEN') or os.environ.get('GITHUB_TOKEN'))
    print(json.dumps(publish(client, args.directory, args.target or ''), indent=2))
