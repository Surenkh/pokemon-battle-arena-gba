# Pokémon Battle Arena — movement update (release 0.2.0)

Open **out/PokemonBattleArena.gba** in a Game Boy Advance emulator such as [mGBA](https://mgba.io/). Press the emulator's **Start** or **A** button on the title screen. You do not need GBA Studio, Node or Python to play the supplied ROM.

This native release includes: six characters, an animated title, character selection, three difficulty levels, one woodland arena, a CPU opponent, stamina, hit-earned super attacks, pause, victory/defeat/draw, sound effects and rematches. All six fighters now have distinct frame-by-frame movement and combat animations, animated selector/result portraits, ground shadows, dust and hit effects. Movement uses acceleration, braking, air momentum and physical recoil.

## Controls

| GBA button | Action |
| --- | --- |
| Left / Right | Move |
| Up | Jump |
| Down | Squeeze: a timed crouch dodge |
| A | Melee attack |
| B | Fire a themed projectile |
| L held | Block ordinary attacks while facing them |
| R | Super: requires 100 SP and 25 stamina |
| Start | Pause / resume |
| Select on title/selector | Show controls |

Keyboard/controller bindings belong to your emulator. If a key does nothing, check which key it maps to the named GBA button.

Clean melee hits earn 18 SP; clean projectile hits earn 22 SP. Blocks and misses earn none. A super bypasses block. Jump above it or time Down so the low squeeze pose is active when it arrives. Squeeze has startup, recovery and cooldown; melee can still hit it. Retreat or stop attacking to recover stamina when you need a super.

On the selector, use the D-pad to highlight a character and A/Start to fight. Down from the bottom row focuses the difficulty setting; left/right changes it and Up returns to the characters. B returns to the title. Results offer Rematch and Change. The timer pauses during hitstop and pause.

## Editing and rebuilding

`PokemonBattleArena.gbasproj`, `content/` and `graphics/` are the editable GBA Studio asset project. The combat and screen logic live in `src/` and `include/`. `config/balance.json` generates the character definitions; edit it and rerun the build. Original artwork is preserved in `assets/source/`.

The project includes GBA Studio 0.5.0's build CLI, native templates and required Butano sources in `engine/studio_resources/`. Its bootstrap is patched only within this project. The regular build button in an unmodified global editor does not install that patch; use the wrapper below for this custom C++ runtime.

Rebuild requires Python 3.12+, Node.js 22+, GNU Make and the compiler bundled with GBA Studio 0.5.0. Locate your installed application's **resources** directory, which contains `public/vendors/devkitPro/`, then run:

```sh
python tools/build.py --studio-resources "/path/to/GBA Studio/resources" --clean
```

Or point to an existing devkitPro directory containing `devkitARM/` and `tools/`:

```sh
python tools/build.py --devkitpro "/path/to/devkitPro" --clean
```

The first route extracts the official compiler archive into this project's `.toolchain/`; it leaves the installed editor unchanged. Build output is `out/PokemonBattleArena.gba`. Linux x86_64 is the verified build platform. Windows/macOS rebuilds have not been tested.

Run the host combat checks with a standard C++17 compiler:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/combat_tests.cpp -o combat_tests
./combat_tests
```

Native graphics are already converted. To regenerate them, install the pinned Node dependencies, run `node tools/prepare_animations.cjs`, then `node tools/convert_assets.cjs`; then clean-build. Included source assets allow regeneration without downloads. See `docs/ASSET_CONVERSION_REPORT.md`.

## Verification and current limits

The ROM was built through GBA Studio's CLI and tested in mGBA 0.10.2. Tests exercised all six characters, native controls, pause/resume, difficulty selection, real match outcomes and repeated rematches. See `docs/TEST_REPORT.md` for exact results.

This version has drawn frame animation, sound effects and one arena. It has no campaign, multiplayer, save system or background music. Character balance is an initial proposal. Physical GBA hardware and graphical editor open/save behavior were not tested.

`docs/ARCHITECTURE.md` is the design reference. `docs/IMPLEMENTATION_STATUS.md` records implemented behavior and remaining work. Retain `assets/source/CREDITS.md` and the engine license notices with redistributed source.

## Movement update

Walking accelerates over a few ticks; releasing direction brakes smoothly. Reversing on the ground brakes more strongly. Jumping retains horizontal momentum, with gentler steering in the air. Landing can immediately be cancelled into another action. Normal hits lift the target slightly; supers give stronger recoil. Damage, stamina costs, SP gain, jump height and super-dodge rules remain unchanged.

The runtime uses 408 packed animation frames across 13 clips per fighter, including repeated hold frames. The existing Pokémon Mystery Dungeon artwork and source credits are preserved in `assets/source/animations/`. These are distinct authored poses, not only transformations of the original static sprites. See `docs/ANIMATION_AND_PHYSICS.md` and `docs/animation_preview.gif`.

Extra checks:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/movement_tests.cpp -o movement_tests
./movement_tests
node tests/assets_test.cjs
```
