# Movement update verification — 28 October 2026

Release 0.2.0; combat ruleset 0.3.0.

## Delivered build

Built with the project-scoped GBA Studio 0.5.0 CLI, native bootstrap and Butano sources, using devkitARM 16.1.0 on Linux x86_64. ROM size: 1,313,656 bytes. SHA-256: `d13c7fd6642aaa6a0fe582b9ee8b479f0ce3aae10e503ec70f7d78ec82843845`. The native importer accepted all fighter sheets and effects. The compiled ROM was then loaded into mGBA for the checks below.

## Host and asset checks

- PASS 7195540 assertions; 720 seeded matches; all 36 ordered matchups.
- PASS 18904 movement and animation checks.
- All 408 sprite frames passed alpha bounds, foot-anchor, clip, distinct-walk-pose and 4bpp palette checks.

Movement tests cover acceleration, braking, reversing, walls, takeoff momentum, air drag, floor contact, immediate landing cancellation, hit lift, guard-to-jump transition, authored impact frame timing, hitstop freezing and clip bounds. Existing tests retain all six characters and both sides for super-versus-standing/block/jump/squeeze, resource costs, guard edges, deterministic replays, KO and timeout rules.

## Compiled ROM in mGBA 0.10.2

- 297 general native checks and 1260 native animation checks passed.
- Every character rendered all its walk-cycle indices and distinct rising/falling jump poses.
- All six characters exercised selection, HP/STA/SP initialization, jump, squeeze, fire, block, pause, resume and return to selection.
- Difficulty settings and rematch resource/choice reset checks passed.
- Pause preserved simulation and animation telemetry.
- 35 complete rounds, 36,678 battle display frames (10.19 nominal minutes), and 36 observed player super activations.
- Peak reported CPU workload: 86.57% of a display frame, including scene-entry costs observed by the runner.
- Missed display frames observed: 0.

Drivers send physical button masks and read telemetry; they do not write gameplay memory. The native animation preview contains 121 captured frames sampled every two display frames. Current native screenshots and the GIF come from this ROM. Automated outcomes do not establish character win rates or human balance.

## Limits

Physical GBA hardware, flash cartridges, graphical editor open/save and Windows/macOS rebuilding have not been verified. Human playtesting is still needed for movement feel and balance. No campaign, multiplayer, music or save system was added.

Static memory reported by the native linker: EWRAM 34.9 KB / 256 KB; IWRAM 23.1 KB / 32 KB, leaving approximately 8.9 KB for stack.
