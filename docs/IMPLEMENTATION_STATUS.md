# Implementation status — 28 October 2026

Playable release 0.2.0; combat ruleset 0.3.0.

Implemented: GBA Studio native build and bootstrap; six canonical character definitions; deterministic fixed-point combat; integer HP/stamina/SP; melee/fire/block/jump/squeeze/super; hitstop, guard break and post-hit protection; collision ordering and projectile clashes; seeded delayed-observation CPU with three difficulties; title animation; native selector and difficulty row; countdown; battle HUD and super-ready cue; pause/controls; results/rematch/change; original sound effects; source/native asset packaging; host and native tests.

Milestones 0–4 have working implementations. Milestone 5 now has authored frame animations for all six fighters, animated selector/result portraits, dust, hit sparks and ground shadows. Milestone 6 has emulator performance measurements, sound effects and deterministic stress tests; human balance tuning and hardware profiling are outstanding. Milestone 7 includes a clean-built ROM, editable source, engine patch, compiler instructions and reports.

Implementation choices: gameplay state is mutated directly by CombatCore; compact feedback flags request sound and graphics rather than a general semantic-event queue. Contact storage is bounded at 16 and traps on overflow; the current pools permit at most nine candidate pairings. CPU observations use a 32-entry ring. Native input and host-test input share the same rules.

The initial character statistics are retained. CPU behavior additionally retreats to recover stamina when SP is full but a super is unaffordable. Replays hash explicit combat fields; they do not hash C++ padding or renderer handles. The native application exports bounded read-only telemetry for emulator tests.

Remaining polish: background music, more human playtesting and matchup balance measurements, and physical hardware validation. Additional arenas, multiplayer, saves, evolution and campaign remain outside this version's scope.

Build/test commands and outcomes are recorded in `TEST_REPORT.md`, `clean_build.log`, `host_tests.log`, `native_test_report.json` and `build_versions.json`. Next development task: playtest super readability and stamina recovery with humans, then tune only the canonical configuration with corresponding test evidence.

Movement now uses acceleration, braking, air steering and physical hit lift. See `ANIMATION_AND_PHYSICS.md` for the implementation contract.
