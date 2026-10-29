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

The original character artwork and credits are in assets/source/animations/. Screen layers and bitmap UI helpers are in assets/source/screens/. Elemental graphics are authored in tools/elemental_fx.cjs. Original combat sounds are synthesized deterministically by tools/generate_combat_audio.py using Python's standard library. Generated WAV files and their GBA Studio content imports are tracked, so audio regeneration is optional for a normal build. No asset downloads are needed for regeneration.

Host checks:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/combat_tests.cpp -o combat_tests
./combat_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/movement_tests.cpp -o movement_tests
./movement_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/feel_tests.cpp -o /tmp/pkmn_feel_tests
/tmp/pkmn_feel_tests
```

For native tests install mGBA 0.10.2 development headers/library, compile tests/mgba_driver.c, and run tests/native_flow.py, tests/native_animation_flow.py, tests/native_effects_flow.py and tests/native_feel_flow.py. The feedback runner also needs Pillow (`python3 -m pip install Pillow`). See [NATIVE_TESTING.md](NATIVE_TESTING.md). These scripts use physical inputs and read-only telemetry, not gameplay-memory writes.
