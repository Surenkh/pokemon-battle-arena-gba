# Development and version control

Use this Git repository as the project history. `main` is the playable, reviewed baseline. Work on named feature branches, keep commits focused, and merge through a pull request with validation evidence.

Current branch: `feat/elemental-attack-identity`. This branch prepares the design; editable game source is still needed for implementation.

```bash
git clone https://github.com/Surenkh/pokemon-battle-arena-gba.git
cd pokemon-battle-arena-gba
git switch feat/elemental-attack-identity
```

Restore the original source and preserve engine licenses, build tools, patches, and required generated inputs. First commit that restoration separately from gameplay changes. Record the actual toolchain and a clean-checkout build command. Do not invent a new build system from the ROM or commit credentials, machine-specific tools, or dependency caches.

For each playable change, record what changed, why, validation results, and remaining limitations in the PR. Run checks relevant to the change. Host tests cannot prove native frame time or palette/VRAM capacity. Keep performance evidence and short native-resolution captures with changes to the renderer.

Do not update `downloads/PokemonBattleArena.gba` for design-only commits. Update the ROM and checksum together only after validation. Use a new version/tag for a released build; do not silently replace an already distributed version or rewrite shared history.

Suggested sequence: `Restore editable project and baseline build`, `Add elemental effect resources`, `Wire effect identities into rendering`, `Validate and package v0.3.0`. The release version is provisional until the scope and checks are complete.
