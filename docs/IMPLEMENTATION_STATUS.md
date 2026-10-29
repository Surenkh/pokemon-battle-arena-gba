# Implementation status — 29 October 2026

Playable release and input ruleset: **0.4.0 — Combat Feel**. The v0.3.0 baseline is merged in Git; the new update is isolated on `feat/combat-feel`.

Implemented: six selectable fighters; deterministic fixed-point combat; melee, fire, jump, squeeze, block and earned unblockable supers; momentum and authored movement animations; animated elemental attacks; delayed-observation CPU with three difficulties; title, controls, selector, countdown, pause, results and rematch; native GBA Studio build; source asset regeneration; host and native tests.

This update adds bounded source-specific combat events; 27 original synthesized sounds; a two-voice combat audio dispatcher; hit flashes; decaying camera shake with a fixed HUD; block, guard-break and insufficient-stamina feedback; themed super startup glyphs and charging/release sounds; and a stable super-evasion warning. Presentation consumes no gameplay randomness and cannot alter collision or damage results.

The intended gameplay change accepts an Up press during the last two descending simulation ticks before touchdown, using the existing three-tick queue. It creates no extra air jump or held-button repeat. Damage, costs, hitstop, jump trajectory, attack timings and CPU decision code retain their prior values.

The renderer keeps one original arena background and uses two HBlank scroll effects to shake only world rows. Their allocation is spread over early countdown frames to avoid a screen-entry spike. Edge padding prevents scroll wrap artifacts. All world sprites share the displacement; HUD elements stay fixed. Combat audio stops on pause/menu transitions, uses priority-based voice replacement, and never loops charge samples.

Commands are in [BUILDING.md](BUILDING.md). Results, ROM checksum and clean-build evidence are in [COMBAT_FEEL_TEST_REPORT.md](COMBAT_FEEL_TEST_REPORT.md); machine-readable reports are under verification/v0.4.0/. Architecture and tradeoffs are in [COMBAT_FEEL.md](COMBAT_FEEL.md).

Remaining work: human testing and measured matchup tuning, background music, physical hardware profiling and non-Linux build validation. Additional arenas, campaign, progression and multiplayer remain future work. The next roadmap direction is deeper fighter decisions, informed by playtesting.
