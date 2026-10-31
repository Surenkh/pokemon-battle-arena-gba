# Build and verify

The project uses the vendored GBA Studio 0.5.0 CLI/templates, Butano, and devkitARM 16.1.0 from the official GBA Studio 0.5.0 Linux x64 release. Linux x86_64 is the tested platform. Install Node.js 22+, Python 3.12+, GNU Make, and that compiler (or an existing matching devkitPro tree).

The converted native graphics are tracked. Build the ROM with:

```sh
python3 tools/build.py --devkitpro /path/to/devkitPro --clean
```

Or use `--studio-resources /path/to/GBA-Studio/resources`; see [the baseline build recipe](BASELINE_BUILD.md). Machine-specific settings are restored on exit rather than left in the tracked project. Output is `out/PokemonBattleArena.gba`.

To regenerate artwork using only tracked source inputs:

```sh
npm ci --no-audit --no-fund
npm run assets:prepare
npm run assets
npm run test:assets
npm run audio
npm run test:audio
```

The original character artwork and credits are in assets/source/animations/. Screen layers and bitmap UI helpers are in assets/source/screens/. Elemental graphics are authored in tools/elemental_fx.cjs. Original combat sounds are synthesized deterministically by tools/generate_combat_audio.py using Python's standard library. The v0.7 presentation generator additionally creates an original three-voice MOD title loop and fourteen PCM cues. Generated MOD/WAV files and their GBA Studio content imports are tracked, so audio regeneration is optional for a normal build. No asset downloads are needed for regeneration.

Host checks:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/combat_tests.cpp -o combat_tests
./combat_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/movement_tests.cpp -o movement_tests
./movement_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/feel_tests.cpp -o /tmp/pkmn_feel_tests
/tmp/pkmn_feel_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/depth_tests.cpp -o /tmp/pkmn_depth_tests
/tmp/pkmn_depth_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/cpu_tests.cpp -o /tmp/pkmn_cpu_tests
/tmp/pkmn_cpu_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/presentation_tests.cpp -o /tmp/pkmn_presentation_tests
/tmp/pkmn_presentation_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tools/cpu_probe.cpp -o /tmp/pkmn_cpu_probe
/tmp/pkmn_cpu_probe > /tmp/pkmn_cpu_fixed.json
/tmp/pkmn_cpu_probe 31 > /tmp/pkmn_cpu_holdout.json
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tools/balance_probe.cpp -o /tmp/pkmn_balance_probe
/tmp/pkmn_balance_probe candidate > /tmp/pkmn_balance.json
```

For native tests install mGBA 0.10.2 development headers/library, compile tests/mgba_driver.c, and run tests/native_flow.py, tests/native_animation_flow.py, tests/native_effects_flow.py, tests/native_feel_flow.py tests/native_depth_flow.py tests/native_cpu_flow.py and tests/native_presentation_flow.py. The feedback runner also needs Pillow (`python3 -m pip install Pillow`). See [NATIVE_TESTING.md](NATIVE_TESTING.md). These scripts use physical inputs and read-only telemetry, not gameplay-memory writes.

## Prepare release downloads

After native validation and updating the version/build evidence, run:

```sh
python3 -m unittest discover -s tests -p release_tests.py -v
python3 tools/package_release.py
```

The packager verifies ROM/header/checksum/version consistency, matching passed native evidence (including CPU and audio/recall coverage), zero reported missed frames and an identical clean build. It creates the ROM, deterministic player ZIP, checksums and publishing metadata under dist/release/. The player ZIP contains controls, release notes and credits. No account token is needed for local packaging.

The GitHub release workflow runs packaging tests, canonical-data checks and host combat regressions. On validated main pushes it publishes with the repository's built-in token; pull requests only validate. It publishes the existing tested ROM rather than recompiling or claiming a new emulator run. See [PUBLISHING.md](PUBLISHING.md).
