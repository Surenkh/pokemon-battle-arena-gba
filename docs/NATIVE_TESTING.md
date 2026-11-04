# Optional native regression runner

On a Linux machine with mGBA development headers and library installed, compile the driver:

```sh
cc tests/mgba_driver.c -lmgba -o tests/mgba_driver
python3 tests/native_flow.py
```

Run a native build first, preserving `.gbastudio/PokemonBattleArena.elf`. The script discovers the telemetry symbol from that ELF, sends physical button inputs to the ROM, and reads telemetry. It does not write gameplay state. It creates its logs and captures under `tests/native_artifacts/`. Set `PKMN_MGBA_DRIVER` to override the driver executable and `LD_LIBRARY_PATH` if using a locally extracted mGBA library. The supplied mGBA adapter was tested with 0.10.2; API differences may require adaptation for other versions.

The driver now reads 32 telemetry words. Slots 25–26 are fighter frame indices, 27–28 are clip IDs and 29 is the live particle count. Run `tests/native_animation_flow.py` with the same driver and environment as `tests/native_flow.py`; it writes its report and native preview captures under `tests/native_artifacts/animation/`.

## Elemental effects

Run `python3 tests/native_effects_flow.py` with the same driver/environment. It uses real inputs to earn and fire supers for each fighter and validates normal travel phases, themed muzzle/hit effects, bounded live effects and frame timing. Word 30 is a seen-phase mask (normal bits 0–3, super bits 4–7). Word 31 records per-character hit, muzzle and guard effects at bit offsets 0, 8 and 16. Close supers may hit before displaying a full loop. All four super artwork frames are validated by tests/effects_assets_test.cjs.

## Combat feedback and audio

Run `python3 tests/native_feel_flow.py` with the same driver/environment and Pillow installed. It records stereo WAVs from mGBA's actual audio channels, tests the two-voice cap, exercises every fighter's shot/charge/release bank through real play, and checks pause, guard break, exhaustion and fixed HUD pixels. A translation check confirms frozen actors and scenery move together. Generated WAVs/PPMs are temporary test artifacts, not source assets.

The driver explicitly enables audio at 32,768 Hz, drains both emulator audio buffers every display frame, and finalizes WAV headers on close/exit. Its `b` replay policy creates distance before a full-SP super to exercise completed release sounds; the original `a` regression policy is preserved. No runner writes gameplay memory. General flow results must report zero missed display frames as well as passing functional checks.

A separate 16-word `pkmn_feel_debug` array leaves the original 32-word telemetry layout intact:

| Word | Value |
| --- | --- |
| 0 | Magic 0x4645454c |
| 1 | Seen combat-event mask |
| 2–3 | Signed screen-space camera displacement |
| 4 | Active combat voices (0–2) |
| 5–7 | Played character, charge and release bank masks |
| 8–11 | Guard breaks, rejected stamina inputs, landing-buffer jumps, shake frames |
| 12 | Active charge-owned voices |
| 13 | Last native CPU fraction in Q12; volatile even while paused |
| 14–15 | Per-fighter HUD status codes |

Status codes are 0 none, 1 low stamina, 2 block, 3 rejected stamina input, 4 guard break, 5 super ready. Reports are committed under docs/verification/v0.4.0/. These tests exercise the native ROM; host tests additionally cover input-buffer boundary cases that are difficult to arrange with external button timing.

## Fighter depth and practice

Run `python3 tests/native_depth_flow.py` with the same emulator driver. It enters practice through actual title/setup buttons, opens every fighter guide, lands A-to-B follow-ups for all six fighters, verifies Eevee's close-range A-to-A follow-up, observes native guard costs (23, 12 and 18 points in the tested pairings), evades an actual super, and performs Meowth's 11-damage recovery punish. It also verifies pause, reset, ordinary-mode restoration and a 12,000-display-frame practice replay with automatic KO restarts. No runner writes gameplay state.

Set `PKMN_PROFILE_FRAMES=1` for per-frame diagnostics in the depth runner outside its batched stress section. It records the current screen, character, simulation tick and CPU usage when a frame exceeds budget or the missed-frame count grows. This located the initial practice HUD and Eevee guide allocation spikes.

The separate 32-word `pkmn_depth_debug` array preserves both earlier telemetry layouts:

| Word | Value |
| --- | --- |
| 0 | Magic 0x44455054 |
| 1–4 | Practice flag, setup row, dummy mode, dummy character |
| 5–7 | Last damage, linked damaging hit count, best sequence |
| 8–11 | Per-fighter chain depth and clean-melee confirmation tick |
| 12–13 | Configured per-fighter melee reach |
| 14–18 | Player practice follow-up hits/punishes, presented follow-ups/punishes, practice hit count |
| 19–20 | Authoritative training flag; last native CPU usage in Q12 |
| 21–24 | Signed controlled velocity and knockback for each fighter |
| 25–28 | Action end ticks and start ticks for each fighter |
| 29–31 | Current event count, player HUD status, practice round count |

HUD statuses add 6 PUNISH, 7 FOLLOW B and 8 FOLLOW A/B. Signed values appear as unsigned words in the driver output. New screens append IDs 9 practice setup and 10 fighter guide; original screen IDs are preserved. Reports and balance evidence for this update are under verification/v0.5.0/.

## v0.6 CPU tactics

Run `python3 tests/native_cpu_flow.py` with the same native driver/environment. It observes all six opponent styles at each of the three actual selector difficulty settings, checks the public-observation delay, bounded memory/spacing, plan variation, sustained resource recovery, pause and completed rounds. No gameplay memory is written. Audio opportunity coverage in `native_feel_flow.py` now selects Easy through physical buttons; stronger AI can interrupt its external replay's supers. Difficulty behavior is covered independently by the CPU runner and controlled host comparisons.

A separate 32-word `pkmn_cpu_debug` array preserves all prior layouts:

| Word | Value |
| --- | --- |
| 0 | Magic 0x43505536 |
| 1 | Intent: 0 approach, 1 space, 2 pressure, 3 recover, 4 defend, 5 punish, 6 follow, 7 super, 8 hesitate |
| 2–3 | Target spacing in pixels; delayed observation tick |
| 4–6 | Last-six memory counts for melee, fire and super starts |
| 7–11 | Think decisions, defense requests, punish requests, follow-up requests, deliberate hesitations |
| 12–13 | Plan (0 pressure, 1 balanced, 2 space); resting flag |
| 14–16 | Reaction delay, think interval, stale/late-round closing flag |
| 17–31 | Reserved zeros |

Request counters record policy choices rather than guaranteed resolved actions. Combat events remain the source for actual hits, blocks and follow-up entries. ROM-bound reports are in verification/v0.6.0/.

## v0.7 audio and recall

Run `python3 tests/native_presentation_flow.py` with the same driver and environment. It captures over a complete title-music loop, observes all four music order positions, checks the measured level/no clipping, verifies actual digital silence for both muted menu and battle recordings, tests independent Music/Sound FX combinations and paused settings, and observes a real loss, win and recall skip. It checks frozen combat state, all seven visual phases and actual loser/winner sprite visibility. Sound effect mask coverage includes every new cue except the draw stinger; its asset is validated separately. No game-memory writes are used.

A new 32-word `pkmn_presentation_debug` array leaves prior layouts intact:

| Word | Value |
| --- | --- |
| 0 | Magic 0x50524537 |
| 1–5 | Music/Sound FX settings, music-playing flag, Q8 music gain, active presentation effect voice |
| 6–9 | Played cue mask/count, audio option row, return screen |
| 10–13 | Recall age, phase (0 impact, 1 arrive, 2 open, 3 absorb, 4 close, 5 fall, 6 seal, 7 done), loser (-1 none), enabled flag |
| 14–20 | Ball x/y Q8, fighter scale Q8, ball tile, model ball/beam/loser visibility |
| 21–24 | Last CPU Q12, music sequence position (-1 stopped), total effect voices, round presentation duration |
| 25 | Reserved |
| 26–30 | Actual renderer player/CPU/ball/beam/glint visibility |
| 31 | Reserved |

Audio screen ID 11 is appended. L opens it from menus/pause; the existing Change Pokémon pause entry stays at index 2. `PKMN_PROFILE_THRESHOLD=0.94 PKMN_PROFILE_FRAMES=1` on the depth runner can identify expensive transitions below the full frame limit. Source-generated PCM/MOD/4bpp art checks are in `tests/presentation_assets_test.py`.

## v0.9.1 input response

Run `python3 tests/native_response_flow.py` with the same driver and mGBA
environment. It measures 90 one-frame inputs, compares the player region with a
matched no-input framebuffer replay, and verifies accepted actions on F1 and
visible cues on F2. It checks projectile/super release ticks, all nine fighters
with held and rejected B, and side margins for all seven menu descriptions.
The runner uses existing telemetry and never writes game memory. Its report is
`tests/native_artifacts/response/report.json`. See
[the validation report](RESPONSIVE_FIRING_TEST_REPORT.md) for frame definitions.

## v0.10 navigation

Run `python3 tests/native_navigation_flow.py`. The runner checks the two roadmap
setup tasks, 27 fighter guide round trips, held/rapid confirmation, joined focus
borders, unique result highlighting and exhausted-continue routes. It sends GBA
buttons and reads telemetry/framebuffer pixels without gameplay-memory writes.
`pkmn_arcade_debug[20]` now exposes the focused pause/result choice. Other existing
telemetry indices keep their meaning. Reports live in verification/v0.10.0/.

## v0.11 Practice and drills

Run `python3 tests/native_practice_flow.py`. It completes a damaging B follow-up
and jump/squeeze super evasions for all nine fighters using normal GBA inputs.
It measures manual retries and a real KO restart, clears stale inputs, freezes a
charge during Pause and verifies Practice mastery exclusion. The new
`pkmn_practice_debug` array has 16 read-only words.

| Word | Value |
| --- | --- |
| 0 | Magic 0x50524143 |
| 1–4 | Coaching feedback, completion, rival super started and released |
| 5 | Remaining Practice Ready frames |
| 6–8 | Incoming super active flag, x and attack identity |
| 9 | A drill is active in Practice |
| 10 | Four-bit mask of active or reserved projectile slots in Practice |
| 11–12 | Player and rival pending action |
| 13–15 | Reserved zeros |

Reports live in verification/v0.11.0/. See
[Practice validation](PRACTICE_DRILLS_TEST_REPORT.md) for frame definitions and
coaching rules.

## v0.12 match goals and rival scouting

Run `python3 tests/native_goals_flow.py`. This uses real controls, read-only
telemetry and exact framebuffer/font-mask comparisons. Schema-3 SRAM fixtures
are prepared offline before boot to establish counter boundaries. Follow-up,
super-evade and no-continue increments are then actually earned through play.
The runner covers all nine fighters/cues, unchanged counts, capped medals,
fresh Rematch baselines, power cycles, exhausted runs and one-confirm paths.
No test writes live gameplay memory. The 32-word `pkmn_goals_debug` array is
separate from earlier telemetry:

| Word | Value |
| --- | --- |
| 0 | Magic 0x474f414c |
| 1 | A regular result goal is available on the current screen |
| 2 | Goal: 0 none, 1 follow-ups, 2 evades, 3 no-continue, 4 all medals |
| 3–5 | Chosen goal's before, after and target |
| 6 | This match advanced the chosen goal |
| 7 | Fighter whose baseline was captured |
| 8 | Actual scouting rival index plus one, or zero for none |
| 9 | Scouting is available on this screen |
| 10 | Rendered text bits: 1 goal, 2 scouting, 4 medal notice, 8 save warning |
| 11–31 | Reserved zeros |

Goal text uploads on the third result frame, the retained save/medal notice on
the fourth, and the scouting cue on the fifth. Those labels do not delay input
or add a screen. Reports live in verification/v0.12.0/. See
[goal/scouting validation](MATCH_GOALS_TEST_REPORT.md) for the open human pilot.

## v1.0 RC1 performance and save regression

Run `python3 tests/native_performance_flow.py`. It sends normal buttons and
samples existing per-frame CPU/missed-frame telemetry. It checks all nine
fighters and three difficulties with audio on/off, 180 one-frame direction taps,
Mastery round trips, maximum bounded counters and the maximum 32-bit saved time.
Exact native glyph masks verify right-aligned values and side margins. An
offline valid schema-3 fixture establishes those boundary values. The runner
then changes audio preferences through the UI, saves, restarts the emulator and
verifies every record and mastery counter again. It never writes live gameplay
memory. The Records/Mastery/audio journey must stay below 90% of a display frame
with zero missed frames.

The v0.12.0 Arcade trace locates the prior peak at Records (screen 14) during
fighter browsing. RC1's like-for-like full Arcade replay is recorded separately
from the maximum-value stress journey. The candidate report distinguishes those
measurements. Reports live in verification/v1.0.0-rc.1/.

## v1.2 one-window Local Versus

The older native runners above use mGBA 0.10.2. The additional Versus runner
requires upstream **mGBA `a67176cd3e6bf60a5016d7adc94357142dd12617`** with its
0.11 raw-key input API and Lua enabled. It creates one GBA core, loads the
shipping `tools/local_versus.lua`, and fires actual host-key events through
`mScriptContextFireEvent`. The script alone writes its dedicated input mailbox.
The harness has no combat-state write command. `i` reports the linked upstream
revision. The GUI and a physical keyboard are not part of this automated test.

Use the upstream mGBA repository and Lua repository at these revisions:

```sh
git clone https://github.com/mgba-emu/mgba.git .tooling/mgba-dev-src
git -C .tooling/mgba-dev-src checkout a67176cd3e6bf60a5016d7adc94357142dd12617
git clone --branch v5.4.8 --depth 1 https://github.com/lua/lua.git .tooling/lua-5.4.8
git -C .tooling/lua-5.4.8 rev-parse HEAD
# Expected Lua revision: 6e22fedb74cf0c9b6656e9fce8b7331db847c605
```

Build Lua as a static PIC library. The official Git repository includes test
and amalgamation entry points which must be excluded:

```sh
mkdir -p .tooling/lua-5.4.8/objects
python3 - <<'PY'
from pathlib import Path
import subprocess
root = Path('.tooling/lua-5.4.8').resolve()
objects = []
for source in sorted(root.glob('*.c')):
    if source.name in {'lua.c', 'luac.c', 'onelua.c', 'ltests.c'}:
        continue
    target = root / 'objects' / (source.stem + '.o')
    subprocess.run(['cc', '-O2', '-fPIC', '-DLUA_USE_LINUX', '-I' + str(root),
                    '-c', str(source), '-o', str(target)], check=True)
    objects.append(str(target))
subprocess.run(['ar', 'rcs', str(root / 'liblua.a'), *objects], check=True)
PY
cmake -S .tooling/mgba-dev-src -B .tooling/mgba-dev-build \
  -DCMAKE_BUILD_TYPE=Release -DLIBMGBA_ONLY=OFF \
  -DBUILD_QT=OFF -DBUILD_SDL=OFF -DBUILD_GL=OFF -DBUILD_GLES2=OFF -DBUILD_GLES3=OFF \
  -DBUILD_SHARED=OFF -DBUILD_STATIC=ON -DBUILD_LTO=OFF \
  -DM_CORE_GBA=ON -DM_CORE_GB=OFF -DENABLE_SCRIPTING=ON -DENABLE_DEBUGGERS=OFF \
  -DUSE_LUA=5.4 -DLUA_INCLUDE_DIR="$PWD/.tooling/lua-5.4.8" \
  -DLUA_LIBRARY="$PWD/.tooling/lua-5.4.8/liblua.a" \
  -DUSE_PNG=OFF -DUSE_ZLIB=OFF -DUSE_FFMPEG=OFF -DUSE_EDITLINE=OFF \
  -DUSE_LIBZIP=OFF -DUSE_MINIZIP=OFF -DUSE_ELF=OFF -DUSE_SQLITE3=OFF \
  -DUSE_JSON_C=OFF -DUSE_FREETYPE=OFF -DUSE_DISCORD_RPC=OFF -DUSE_LZMA=OFF
cmake --build .tooling/mgba-dev-build -j2
python3 tools/build_versus_test_driver.py \
  --mgba-source .tooling/mgba-dev-src --mgba-build .tooling/mgba-dev-build
python3 tests/native_versus_flow.py
```

The helper obtains conditional ABI defines and Lua paths from the actual mGBA
build. Retain the ELF from the same game build as the tested ROM. The runner
checks every player's roster choices, all locations, simultaneous F1 movement
and attacks, held-input edges, pause, adapter loss/reload/reconnect, mirror
matches, real wins for both players, first-to-two scoring, quick rematch and
byte-identical solo SRAM. Captures and a ROM/script-bound report are written to
`tests/native_artifacts/versus/`. Host mailbox and set-state checks run with:

```sh
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp \
  tests/local_versus_tests.cpp -o .tooling/local-versus-tests
.tooling/local-versus-tests
```

Use [the player guide](LOCAL_VERSUS.md) for desktop setup and the remaining
physical-keyboard/Qt pilot. Stable mGBA 0.10.x cannot run this input adapter.

## v1.3 fighter techniques

Use the pinned upstream mGBA/Lua build described above and compile the current
`tests/mgba_versus_driver.c` with `tools/build_versus_test_driver.py`. P1 O now maps
to Select for options and Practice retry.

```sh
python3 tests/native_techniques_flow.py
g++ -std=c++17 -O2 -Iinclude src/pkmn_combat_core.cpp tests/techniques_tests.cpp -o .tooling/techniques-tests
.tooling/techniques-tests
node tests/techniques_assets_test.cjs
```

The runner completes both lessons with real raw-key inputs, tests both player
sides and all advertised counters, and verifies all eight actual warning/blast
frames with exact palette masks. It observes pause, reconnect, fast retry and
SRAM isolation. The harness has no combat-state write command. Results are in
`tests/native_artifacts/techniques/`.

The 32-word `pkmn_technique_debug` array preserves prior layouts. Word 0 is magic
0x54454333; 1–2 are technique IDs, 3–4 used flags, 5–6 brake expiry, 7–8 signed
vertical velocities, 9–12 input opportunities, 13–14 orb activity, 15–16 burst
start (-1 unarmed), 17–20 orb x/y Q8, 21–22 visual phase, 23 event mask, 24–29
per-round brake/arm/hit counters, 30 renderer float bits and 31 packed requested
burst phase plus one for P1/P2.

Compile `tools/technique_probe.cpp` with `-DPKMN_TECHNIQUE_PROBE_BASELINE` against
the baseline core/include directory, then against candidate source without the
macro. Each runs 10,140 seeded CPU matches. Compare the 121 pairings excluding
fighters 7 and 10. Baseline: a2df69db08eb4503789dc0a82e979cb9c1069595.
`tools/balance_probe.cpp` provides broader per-fighter diagnostics. These do not
establish human balance; physical keyboard/Qt and player pilots remain open.


## Rival Cup (v1.4.0-rc.1)

Add `tests/cup_tests.cpp` to the host suites and run
`python3 tests/native_cup_flow.py` with the ordinary mGBA driver. The driver
`g` command supplies the same visible-state button policy as `b` plus fresh
B attempts during melee recovery; it never writes combat state. The native
runner checks all thirteen fighters, all Cup/difficulty launches, real clears
and style events, pause/continue/exhaustion, glyphs, maximum legal records,
power cycles and independent corrupt/interrupted-save recovery. Local Versus
also carries a valid populated Cup journal through its unchanged-save test.

The appended 32-word `pkmn_cup_debug` layout is: 0 magic 0x43555034; 1 Cup
selector context; 2 active run; 3 selected Cup; 4 wins/stage; 5 continues;
6 continues used; 7 cumulative active display ticks; 8 style count; 9 finished;
10 awarded; 11 last outcome; 12 current run rival fighter; 13 its arena;
14–17 selected Cup/difficulty clears, style, perfect and best ticks;
18 journal write failed; 19 active/last run's goal target; 20 last fought stage;
21 new-best flag; 22 copy bytes (160); 23–24 journal offsets (4096/5120).
25–31 are reserved. Existing telemetry layouts remain stable.

Menu mode ID 8 is Rival Cup. Its board is screen 18 and result is screen 19;
all earlier IDs retain their meaning. `config/cups.json` is the route source.
A build with `rival_cup: true` requires its own ROM-bound `native-cup.json`
evidence at packaging time. See [contract and pilot](RIVAL_CUP.md).
