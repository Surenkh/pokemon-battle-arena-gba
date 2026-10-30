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
