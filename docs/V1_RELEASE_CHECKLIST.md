# v1.0 release checklist

Updated 3 November 2026. RC2 is `1.0.0-rc.2`; v0.12.0 remains the stable release.
The automated candidate is ready for the sessions below. A pending row has not
been completed by emulator tests.

| Gate | RC2 evidence / remaining decision | Status |
| --- | --- | --- |
| Automated performance | Records/Mastery/audio stress below 90% of a frame; full native regression has zero missed frames | Passed |
| Input and navigation regression | All thirteen fighters, tap-response trials, setup/focus/Back routes and Practice retry boundaries | Passed |
| Progress and saves | Schema-1/2/3 migration, schema-4 power cycles, corruption recovery, interrupted writes, records/mastery retention | Passed |
| Combat regression | All 169 ordered matchups, three difficulties and unchanged legacy-nine replay | Passed |
| Reproducible distribution | Exact tested ROM, regenerated inputs, identical clean rebuild, verified package/checksums and RC publication behavior | Passed |
| Uncoached navigation | Five-player setup tasks below | Open |
| Practice understanding and transfer | Both techniques without verbal coaching, then use a technique in Quick Battle | Open |
| Match goals / scouting | Goal comprehension and voluntary replay pilot below | Open |
| Human matchup review | Representative styles, readable defeats and usable counters | Open |
| Physical GBA/controller | Device response, full Arcade run, audio, save persistence and power cycles | Open |

The machine-readable native and host evidence is in
[verification/v1.0.0-rc.2](verification/v1.0.0-rc.2/). The
[expansion report](ROSTER_EXPANSION_TEST_REPORT.md) explains performance scope.

## Five-player sessions

Use the same controller/emulator settings across comparable sessions. Record
build, setup, task order, elapsed time, backtracks, wrong-fighter launches and
requests for help. Do not explain a control before an uncoached task.

1. Ask the player to start as Pikachu in Volcanic after reading its guide, then
   set up Machop against a blocking Squirtle in Shoreline Practice. At least 4 of
   5 should complete both tasks without help, with no wrong-fighter launches or
   double activations.
2. Ask the player to complete Follow-up and Evade Super in Practice, then try one
   technique in Quick Battle. At least 4 of 5 should perform both dummy goals
   without verbal coaching. Record transfer separately; do not infer it from a
   completed drill.
3. Compare v0.11.0 with RC1 in ten-minute free-play periods, counterbalancing
   build order and using equivalent separate save states. Observe what players
   do after results. Ask what they would try next and why. Keep/revise the goal
   and scouting design if at least 4 of 5 can name a concrete next goal and at
   least 3 voluntarily start another fight. Rework it if the gate fails.
4. Review rushdown against guard pressure, ranged pushback, and floaty jumping
   against long reach. Alternate fighter choices and record the exact rival and
   difficulty. Ask whether the player recognized what beat them and could try
   a counter. Log reproducible issues before tuning numbers.

Record outcomes in a session log. The small pilot selects work to investigate;
it does not establish population retention or universal matchup balance.

## Physical-device session

Record the GBA/compatible device, flashcart, controls and audio setup. With a
test save, check startup, every menu, representative one-tap attacks, block,
jump/squeeze, Practice reset and pause/resume. Finish a five-rival Arcade run,
return to Records/Mastery, power-cycle and verify the saved outcome and audio
preferences. Check a legacy save migration if that device supports importing it.
Log stalls, visual corruption, missing/doubled actions and reproduction steps.

## Final v1.0 decision

Resolve reproducible control, save, progression and performance blockers, and
record the player/device findings above. Then prepare a separate `1.0.0` ROM,
versioned evidence and immutable release. RC1 remains a prerelease; its tag is
not moved or renamed. Every code change after this candidate needs the relevant
regression and updated ROM-bound evidence.
