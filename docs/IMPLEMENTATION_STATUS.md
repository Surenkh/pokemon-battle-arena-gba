# Implementation status — 30 October 2026

Playable release and input ruleset: **0.5.0 — Fighter Depth & Practice**, developed on `feat/fighter-depth` from the merged v0.4.0 baseline.

Implemented foundations: six selectable fighters; deterministic fixed-point combat; movement, melee, fire, block, jump, squeeze and hit-earned unblockable supers; authored movement animations; elemental graphics/audio; delayed-observation CPU with three difficulties; title, selector, controls, countdown, battle, pause, results and rematch; editable GBA Studio native build; repeatable source asset generation and host/native tests.

The new release adds six configured traits, source-specific hit-confirm follow-ups with a one-cancel limit, recovery punish feedback, a 24-tick stamina regeneration delay and a CPU resource recovery policy. Both sides use actual melee reach and the same input queue. Base HP/damage/costs, jump physics, super evasion and difficulty observation delays retain their prior values; Meowth has an explicit conditional melee damage bonus.

B on the title opens practice setup. Idle, blocking and attacking dummies generate ordinary inputs. R opens fighter guides. Practice shares damage/resources/collisions, freezes timeout, starts with full SP, displays actual hit feedback, resets through Select, and automatically restarts after KO. Pause and ordinary-mode restoration are covered by native tests. Practice statistics remain outside combat checksums.

Renderer changes cache static HUD names and practice text, defer arena/guide allocations, and keep the practice board outside camera movement. Character guides and three new backgrounds use the existing indexed-asset pipeline. Effect pools and the two-voice combat mixer remain bounded.

Commands are in [BUILDING.md](BUILDING.md). Architecture and decisions are in [FIGHTER_DEPTH.md](FIGHTER_DEPTH.md); measured results and limitations are in [FIGHTER_DEPTH_TEST_REPORT.md](FIGHTER_DEPTH_TEST_REPORT.md). Reports are under verification/v0.5.0/. Baseline and candidate balance probes each cover 2,160 seeded CPU-versus-CPU rounds. These do not replace human balance testing.

Remaining work: human playtesting, especially defensive Squirtle value and Pikachu pressure; more varied opponent behavior; physical hardware profiling; non-Linux build validation. Music, more arenas, multiplayer, campaign and persistence remain later scope. The next roadmap direction is smarter and more varied opponents.
