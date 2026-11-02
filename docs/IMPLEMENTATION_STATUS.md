# Implementation status — 2 November 2026

Playable candidate: **0.12.0 — Match Goals & Rival Scouting**, developed on
`feat/post-match-goals` from merged/released v0.11.0.

Regular results show one relevant mastery increment from the selected fighter's
actual per-match baseline. A new medal takes priority. With no increment, show
the next unfinished goal or completion of all three. Results only read progress;
Practice remains excluded, and save boundaries/schema 3 are unchanged.

Arcade's existing result panel scouts the actual next or retry rival using its
authored trait and a short counter cue. All nine have their own cue. Champion
and exhausted runs have no impossible next rival. Confirmation counts,
continues, clocks, arena rotation and normal combat rules are unchanged.

All thirteen native suites pass with 15,639 checks and zero missed display
frames. Eleven host suites pass, including 373,567 new goal checks. Native tests
earn real follow-up/evasion medals and a no-continue clear, render every rival
cue and compare exact glyph masks. The new journey peaks at 0.8135 of a frame;
the busiest Arcade frame is 0.99609375, below the recorded v0.9.0 baseline.

The next-goal observation and replay-motivation pilot remain open; v0.12.0 is a
playable prototype for that decision. See [goal/scouting validation and pilot](MATCH_GOALS_TEST_REPORT.md).
The earlier uncoached navigation/Practice pilots, technique transfer into Quick
Battle and physical-device testing also remain open.

v0.11.0 retains Free, Block and Attack plus optional Follow-up/Evade Super drills.
Manual Practice reset accepts a fresh action on display frame 8, and an automatic
KO restart on frame 14. The full native regression verifies these timings and
mastery exclusion again. See [Practice validation](PRACTICE_DRILLS_TEST_REPORT.md).

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
