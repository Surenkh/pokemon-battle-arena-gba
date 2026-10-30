# Distribution and GitHub Releases

Player downloads live at https://github.com/Surenkh/pokemon-battle-arena-gba/releases. Each published version is tied to a vX.Y.Z Git tag and includes:

- PokemonBattleArena.gba: the exact previously validated native ROM.
- PokemonBattleArena-vX.Y.Z.zip: the ROM, controls, credits, release notes and an internal ROM checksum.
- SHA256SUMS.txt: hashes for the standalone ROM and player ZIP.
- GitHub's automatically generated source ZIP/tar archives for that tag.

The generic ROM/checksum asset names keep the README's latest-download links stable. Earlier released versions stay available by their tag. GitHub Packages is optional infrastructure for reusable dependencies/containers and is not used for the game's player downloads.

## Publishing a version

Update package.json/package-lock.json, docs/build_versions.json, downloads/PokemonBattleArena.gba, downloads/SHA256SUMS.txt, release notes in docs/releases/vX.Y.Z.md and version-specific validation evidence under docs/verification/vX.Y.Z/. Keep the built ruleset consistent with config/balance.json and config/cpu.json. Native reports must identify the exact ROM hash and report passed checks with zero missed display frames. Preserve the required clean-build evidence.

Merge the validated change into main. The release workflow runs packaging/publication tests, verifies generated combat definitions, and runs all five host suites, including CPU tactics. Publication is limited to main after validation; pull requests receive read-only validation. Pushes touching version, download, release-note, verification or release-pipeline files trigger it. It can also be run manually from Actions → Validate and publish game release → Run workflow on main.

The publisher creates a draft, uploads all assets, downloads/verifies every uploaded byte, then publishes. A successful rerun verifies the existing public files without modifying them. A conflicting tag, different ROM under an existing version, unrelated draft or failed validation stops publication. An interrupted draft created by this workflow can resume when the same commit is rerun. Published releases and tags are not replaced; use a new version for changed game builds.

The workflow uses only GitHub's built-in repository token with contents write permission on its publication job. It needs no personal access token, stored password or external deployment service. Its checkout action is pinned to an official commit and does not persist credentials. It publishes the tracked, tested ROM; it does not rerun the GBA compiler/emulator in CI. Native validation remains a required development step before preparing a release.

For local packaging and tests, see BUILDING.md. tools/package_release.py prepares the files without credentials; tools/publish_release.py is intended to run inside the configured GitHub Actions job.

The cover is promotional art. .github/social-preview.jpg must be uploaded separately in repository Settings to configure the social preview.
