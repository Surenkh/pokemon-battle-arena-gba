# Game improvement roadmap

This is the current implementation order. Each direction should end in a playable, independently reviewable change. The original five-priority document is not present in this repository; reconcile this working roadmap with it when the editable project is restored.

| Priority | Direction | First deliverable | Evidence that it works |
| --- | --- | --- | --- |
| 1 | Character identity and readable attacks | Six elemental attack/impact/super identities; Pikachu and Squirtle first | Native-resolution footage; identical combat outcomes for the presentation-only pass |
| 2 | Impact and responsive combat | Sound cues, restrained shake, clear guard feedback, carefully specified hitstop and input buffering | Attack feels responsive; clocks and evasion remain predictable; CPU and player obey the same rules |
| 3 | Deeper fighter decisions | Distinct strengths, recovery/risk tradeoffs, counters, and a useful stamina economy | Matchup and seeded simulation evidence; each fighter supports a recognizable play style |
| 4 | Smarter and more varied opponents | CPU spacing, reaction limits, difficulty profiles, and reduced repetition | No input-reading advantage; measurable difficulty differences; exploitable patterns without trivial loops |
| 5 | A living arena and reasons to replay | Low-cost arena motion, round flow polish, short arcade sequence, rematch and progression targets | Faster return to battle; readable action; progression/save behavior tested if added |

Avoid introducing all five at once. Visual identity comes first because it makes later timing, balance, and opponent changes easier for the player to understand.

For priority 1, see [the iteration design](ITERATION_01_ELEMENTAL_ATTACKS.md). Runtime integration is blocked until editable source and build dependencies are restored. No new ROM is published by this roadmap.
