# Implementation status — 30 October 2026

Playable release and input ruleset: **0.6.0 — Smarter & More Varied Opponents**, developed on `feat/cpu-tactics` from the released v0.5.0 baseline.

Implemented foundations: six selectable fighters and traits; deterministic fixed-point combat; animated movement, melee, elemental fire, block, jump, squeeze and hit-earned unblockable supers; one hit-confirm follow-up; elemental graphics/audio; title, selector, controls, countdown, battle, pause, results and rematch; practice and fighter guides; editable GBA Studio native build; repeatable source asset generation, host/native tests and automated GitHub Releases.

The new release adds six CPU styles with fighter-aware spacing, ranged pressure, follow-ups and visible recovery punish preferences. Three difficulty profiles vary reaction delay, decision cadence, defense, follow-ups and deliberate mistakes. A compact public-only observation history excludes opponent inputs and hidden combat fields. Six observed attacks provide bounded habit memory. Resource reserves, limited disengagement and a stalled-round approach prevent passive loops. Practice dummy behavior remains independent. Fighter HP, damage, attack timing, movement and resource costs are unchanged.

Five host suites pass, including 1,221,407 dedicated CPU assertions and deterministic replay. Six native suites pass, including every CPU style at every difficulty, complete rounds, effects/audio, animation, practice, pause and reset, with zero missed display frames. The fresh clean build regenerates and matches 191 native input files and produces the identical tested ROM. Exact counts, ROM hash and limits are recorded in [CPU_TACTICS_TEST_REPORT.md](CPU_TACTICS_TEST_REPORT.md) and verification/v0.6.0/.

Commands are in [BUILDING.md](BUILDING.md). AI architecture, decision order, configuration and tradeoffs are in [CPU_TACTICS.md](CPU_TACTICS.md). Core fighter mechanics and practice remain specified in [FIGHTER_DEPTH.md](FIGHTER_DEPTH.md). The release workflow validates all five host suites and ROM-bound evidence, then publishes immutable ROM/ZIP/checksum assets from main.

Remaining work: human playtesting and matchup tuning, especially Squirtle's defensive value and fast-fighter pressure; physical hardware profiling; non-Linux build validation. The next roadmap direction is arena life and reasons to replay: low-cost background motion, round-flow polish and a short arcade sequence. Music, additional arenas, multiplayer and persistence remain later scope.
