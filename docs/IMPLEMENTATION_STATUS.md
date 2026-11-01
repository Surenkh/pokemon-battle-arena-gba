# Implementation status — 1 November 2026

Playable candidate: **0.9.0 — Arenas & Fighter Mastery**, developed on
`feat/arenas-and-mastery` from the released nine-fighter v0.8.0 baseline.
The combat ruleset and fighter/CPU parameters remain v0.8.0.

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
