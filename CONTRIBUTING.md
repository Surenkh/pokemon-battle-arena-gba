# Development and version control

Use this Git repository as the project history. `main` is the playable, reviewed baseline. Work on named feature branches, keep commits focused, and merge through a pull request with validation evidence.

The restored native source and v0.3.0 elemental attacks are on `main`. The v0.4.0 combat feedback update uses `feat/combat-feel`.

```bash
git clone https://github.com/Surenkh/pokemon-battle-arena-gba.git
cd pokemon-battle-arena-gba
git switch feat/combat-feel
```

Preserve engine licenses, build tools, patches and required generated inputs. Source restoration is recorded separately from gameplay changes. See docs/BUILDING.md for build and validation commands. Do not commit credentials, machine-specific tools or dependency caches.

For each playable change, record what changed, why, validation results, and remaining limitations in the PR. Run checks relevant to the change. Host tests cannot prove native frame time or palette/VRAM capacity. Keep performance evidence and short native-resolution captures with changes to the renderer.

Do not update `downloads/PokemonBattleArena.gba` for design-only commits. Update the ROM and checksum together only after validation. Use a new version/tag for a released build; do not silently replace an already distributed version or rewrite shared history.

Keep historical release evidence in docs/verification/. For v0.4.0, regenerate audio with `npm run audio`, validate with `npm run test:audio`, and run the additional host/native feedback checks in docs/BUILDING.md. A test pass must also report zero missed native display frames; a valid outcome alone does not establish performance.
