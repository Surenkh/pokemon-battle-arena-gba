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

For native tests install mGBA 0.10.2 development headers/library, compile tests/mgba_driver.c, and run tests/native_flow.py, tests/native_animation_flow.py, tests/native_effects_flow.py, tests/native_feel_flow.py tests/native_depth_flow.py tests/native_cpu_flow.py tests/native_presentation_flow.py tests/native_arcade_flow.py and tests/native_mastery_flow.py. The feedback runner also needs Pillow (`python3 -m pip install Pillow`). See [NATIVE_TESTING.md](NATIVE_TESTING.md). These scripts use physical inputs and read-only telemetry, not gameplay-memory writes.

## Prepare release downloads

After native validation and updating the version/build evidence, run:

```sh
python3 -m unittest discover -s tests -p release_tests.py -v
python3 tools/package_release.py
```

The packager verifies ROM/header/checksum/version consistency, matching passed native evidence (including CPU and audio/recall coverage), zero reported missed frames and an identical clean build. It creates the ROM, deterministic player ZIP, checksums and publishing metadata under dist/release/. The player ZIP contains controls, release notes and credits. No account token is needed for local packaging.

The GitHub release workflow runs packaging tests, canonical-data checks and host combat regressions. On validated main pushes it publishes with the repository's built-in token; pull requests only validate. It publishes the existing tested ROM rather than recompiling or claiming a new emulator run. See [PUBLISHING.md](PUBLISHING.md).

Additional host checks for this update:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/arcade_tests.cpp -o /tmp/pkmn_arcade_tests
/tmp/pkmn_arcade_tests
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/roster_tests.cpp -o /tmp/pkmn_roster_tests
/tmp/pkmn_roster_tests
```

The Arcade native suite also checks every visible main-menu entry with no L/R inputs, new-fighter guides and practice play, actual new attack/charge/release audio dispatch, schema-1 save migration, corruption recovery and a completed five-rival run. The original nine-fighter art inputs are pinned to the same credited PMDCollab revision. No compiler or commercial ROM/BIOS is bundled.


v0.9 mastery/arena host checks:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/mastery_tests.cpp -o /tmp/pkmn_mastery_tests
/tmp/pkmn_mastery_tests
```

The new native suite checks all three Quick/Practice arenas, menu paths, a mastery
medal earned from zero in regular fights, actual CPU super evasion, practice
exclusion, legacy nine-fighter migration, mixed-schema recovery and power cycles.
Release packaging additionally requires ROM-bound native-mastery evidence.

For the v0.9.1 response gate, also run:

```sh
python3 tests/native_response_flow.py
```

This uses actual one-frame taps, matched idle framebuffer replays, held/rejected
shots and native menu text bounds. Release packaging requires its ROM-bound
report alongside the other nine native suites.

The v0.10 navigation gate additionally requires:

```sh
python3 tests/native_navigation_flow.py
```

This covers both product setup tasks, all fighter guide round trips, focus
pixels and exhausted Arcade continues. Package with all eleven ROM-bound native
reports. See [navigation validation](NAVIGATION_TEST_REPORT.md).

For the v0.11 Practice gate, also run:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/practice_tests.cpp -o /tmp/pkmn_practice_tests
/tmp/pkmn_practice_tests
python3 tests/native_practice_flow.py
```

The native runner verifies all fighters, both drills, five training modes, retry
latency, stale inputs, mastery exclusion and a real Practice KO. Release packaging
requires all twelve ROM-bound native reports.

For the v0.12 goal/scouting gate, also run:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/goals_tests.cpp -o /tmp/pkmn_goals_tests
/tmp/pkmn_goals_tests
python3 tests/native_goals_flow.py
```

The native runner validates actual earned increments, all nine scouting cues,
exact rendered glyph masks, no-continue Champion, exhausted runs and unchanged
confirm paths. It creates valid SRAM fixtures offline before boot; fights and
new increments use actual buttons. Release packaging requires all thirteen
ROM-bound native reports and the independent clean rebuild.

For v1.0 RC1, also run:

```sh
python3 tests/native_performance_flow.py
```

This checks all 27 Records entries with audio on/off, extreme valid values,
180 rapid direction taps, native glyph masks, Mastery links and an actual SRAM
save/power cycle. It enforces a 90% frame budget for that journey. Package RC1
with all fourteen ROM-bound native reports. The publisher marks semver versions
such as `1.0.0-rc.1` as prereleases and leaves the stable latest-download links
on the stable release.

## RC2 expansion checks

Run the previous fourteen native suites plus `python3 tests/native_expansion_flow.py`.
Host coverage adds `tests/expansion_tests.cpp` to the previous eleven suites.
All fifteen ROM-bound reports are required for packaging. The full balance probe
plays 10,140 rounds; `balance_probe legacy-nine --legacy-nine` replays the old
4,860 rounds and must retain digest 1172440822. Included animation sources are
pinned and credited; `python3 tools/import_expansion_assets.py` optionally repeats
the import. Normal offline builds use the included art.

## v1.1 Selection & Power

The version is `1.1.0`; the combat ruleset and schema 4 remain unchanged.
`config/selection.json` drives the native layout header and bitmap painter.
`tools/selection_art.cjs` bakes all thirteen portrait cards into the background;
only the selected body animates as a sprite. `tools/elemental_fx.cjs` generates
eight-frame super/aura/impact sheets and six-frame punch sheets.

Run `tests/selection_power_tests.cpp` alongside the previous twelve host suites,
and `python3 tests/native_v11_flow.py` alongside the previous fifteen native
suites. The new runner verifies all card labels, selected names and location
values against real framebuffer glyphs, actual battles at each location, and
all effect phases/damage for every fighter. Release packaging requires all
sixteen ROM-bound native reports. Its GIF uses actual emulator frames.
