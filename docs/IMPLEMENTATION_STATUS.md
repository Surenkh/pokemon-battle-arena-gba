# Implementation status — 3 November 2026

Playable candidate: **1.0.0-rc.1 — Release Candidate 1**, developed on
`feat/v1-release-candidate` from the merged v0.12.0 release and GIF update.

Profiling located the previous 99.609375% Arcade-suite spike on Records while
browsing fighters. RC1 bakes its fixed text into a dedicated background and
redraws only the fighter, difficulty and three right-aligned values. The same
Arcade replay now peaks at 86.4013671875%. The Records/Mastery/audio stress
journey peaks at 75.7080078125%, with Records redraws at 41.50390625%.

All fourteen native suites pass with 16,554 checks and zero missed display
frames. Eleven host suites, art/audio checks, the unchanged 4,860-match probe,
10 release integrity checks and an independent identical clean build pass.
The new native stress test covers all 27 record entries with audio on/off,
maximum values, rapid taps, Mastery links and a real SRAM save/power cycle.

RC1 is a prerelease. Latest-stable URLs remain on v0.12.0. The player pilots,
technique transfer, goal/scouting retention decision, human matchup review and
physical GBA/controller sessions remain open. Final v1.0 depends on those
findings. See [candidate evidence](V1_RELEASE_CANDIDATE_TEST_REPORT.md) and
[remaining release tasks](V1_RELEASE_CHECKLIST.md).

Practice retains both optional drills, 8-frame manual retry and 14-frame KO
restart. Regular results retain actual mastery deltas and all nine Arcade
scouting cues. Save schema 3 and all normal combat/progression rules remain
unchanged. The full native regression verifies the previous response,
navigation, Practice, goal and persistence behavior again.

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
