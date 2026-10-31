# Game improvement roadmap

This is the current implementation order. Each direction should end in a playable, independently reviewable change. The editable native source and required assets are restored in Git.

| Priority | Direction | First deliverable | Evidence that it works |
| --- | --- | --- | --- |
| 1 | Character identity and readable attacks | Six elemental attack/impact/super identities; Pikachu and Squirtle first | Native-resolution footage; identical combat outcomes for the presentation-only pass |
| 2 | Impact and responsive combat | Sound cues, restrained shake, clear guard feedback, carefully specified hitstop and input buffering | Attack feels responsive; clocks and evasion remain predictable; CPU and player obey the same rules |
| 3 | Deeper fighter decisions | Distinct strengths, recovery/risk tradeoffs, counters, and a useful stamina economy | Matchup and seeded simulation evidence; each fighter supports a recognizable play style |
| 4 | Smarter and more varied opponents | CPU spacing, reaction limits, difficulty profiles, and reduced repetition | No input-reading advantage; measurable difficulty differences; exploitable patterns without trivial loops |
| 5 | A living arena and reasons to replay | Low-cost arena motion, round flow polish, short arcade sequence, rematch and progression targets | Faster return to battle; readable action; progression/save behavior tested if added |

Avoid introducing all five at once. Visual identity comes first because it makes later timing, balance, and opponent changes easier for the player to understand.

Priority 1 shipped in v0.3.0; see [the iteration design](ITERATION_01_ELEMENTAL_ATTACKS.md) and [its evidence](../ELEMENTAL_ATTACKS_TEST_REPORT.md). Priority 2 is implemented in v0.4.0: themed sounds, hit flash, a fixed-HUD camera shake, guard/stamina feedback, super warnings, and landing input buffering. See [the implementation contract](../COMBAT_FEEL.md) and [test evidence](../COMBAT_FEEL_TEST_REPORT.md).

Priority 3 is implemented in v0.5.0: six fighter traits, one hit-confirm follow-up, recovery punish feedback, a measured stamina delay adjustment and practice with three dummy behaviors. See [the implementation contract](../FIGHTER_DEPTH.md) and [validation evidence](../FIGHTER_DEPTH_TEST_REPORT.md). Automated baseline/candidate results guide tuning; human matchup playtesting remains outstanding.

Priority 4 is implemented in v0.6.0: fighter-specific spacing/attack preferences, public-only delayed observations, three distinct difficulty profiles, bounded visible-pattern memory and resource/stalemate policies. See [CPU tactics](../CPU_TACTICS.md) and [validation evidence](../CPU_TACTICS_TEST_REPORT.md). Priority 5 is next: low-cost arena life, sharper round flow and a short arcade sequence, with saves only after its progression contract is defined. Physical hardware profiling and human balance testing are still outstanding.

An additional update outside these five directions shipped in v0.7.0: original menu music and cues, separate audio controls, and an animated Poké Ball recall for normal defeats. See [audio/recall architecture](../AUDIO_AND_RECALL.md) and [evidence](../AUDIO_AND_RECALL_TEST_REPORT.md). Arena life and arcade progression remain the next roadmap direction.
