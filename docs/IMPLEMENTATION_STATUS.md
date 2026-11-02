# Implementation status — 2 November 2026

Playable candidate: **0.11.0 — Practice & Drills**, developed on
`feat/practice-drills` over the validated v0.10.0 navigation branch.

Practice offers Free, Block and Attack, plus optional Follow-up and Evade Super
drills through its Training row. A damaging B follow-up completes the first.
The second uses one normally charged rival super and verifies a nearby jump or
squeeze through its full passage. Feedback comes from resolved events. The
read-only super-evasion observer is shared with mastery, while Practice remains
excluded from every mastery counter.

Manual reset accepts a fresh action on display frame 8. Practice KO restarts
accept one on frame 14 and retain statistics. Both paths clear old inputs and
projectiles, restore training resources and retain fighter/rival/arena choices.
The normal match countdown is 180 frames and normal loss recall is 156.

All twelve native suites pass with 14,939 checks and zero missed display frames.
Ten host suites pass, including 37,206 new Practice checks. The learner pilot and
physical-device testing remain open. See
[Practice validation](PRACTICE_DRILLS_TEST_REPORT.md).

The native game includes nine animated Pokémon, elemental attacks and sounds,
stamina, hit-earned supers, follow-ups and fighter traits, three CPU difficulties,
Practice, Quick Battle, five-rival Arcade, two continues, champion/records screens,
original menu music, audio controls and an animated Poké Ball recall.

v0.9 adds Volcanic and Shoreline alongside Woodland. Quick Battle and Practice
select an arena; Arcade rotates through all three with a consistent arena on
retries. Arena motion stays above the combat plane and stops outside fighting.
Floor, HUD geometry, hitboxes and combat randomness remain consistent.

Fighter Mastery is a seventh visible mode button. Each fighter earns cosmetic
medals for 10 damaging follow-ups, 5 successful super evasions and a no-continue
Arcade clear at any difficulty. Roster badges and result feedback show progress.
Practice cannot award mastery. The observer never mutates CombatCore.

Recoverable dual-copy SRAM schema 3 stores two bounded mastery counters per
fighter in 420-byte copies at the existing offsets. Earlier schema-1 and schema-2
records/audio settings migrate, and old no-continue clears count immediately.
Steps save at round end, Rematch or leaving a fight; abrupt power-off can lose
unsaved current-match steps. Mid-run resume remains outside scope.

The source contract is [ARENAS_AND_MASTERY.md](ARENAS_AND_MASTERY.md). Validation is
recorded in [ARENAS_AND_MASTERY_TEST_REPORT.md](ARENAS_AND_MASTERY_TEST_REPORT.md)
and verification/v0.9.0/. See [BUILDING.md](BUILDING.md) for reproduction commands.
Historical documents and ROM-bound reports remain under their original versions.

Human matchup testing, physical GBA hardware, non-Linux builds and graphical
editor open/save behavior remain unverified. No human balance claim is made.
GitHub publication follows the existing feature-branch/PR workflow.
