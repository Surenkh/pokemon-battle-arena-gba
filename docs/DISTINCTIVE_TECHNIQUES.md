# v1.3.0-rc.1 — More distinctive fighter techniques

The roadmap's two-fighter pilot adds choices about landing timing and projectile
placement. Test those decisions and their counters before extending the system.

| Fighter | Input and benefit | Commitment and counters |
| --- | --- | --- |
| Jigglypuff — Air Brake | Tap Up again during descent. Cap falling speed at 0.5 px/tick for 12 simulation ticks. | Costs 8 stamina, once per jump, neutral action only. No extra height or invulnerability; a hit ends it. Wait for landing or use an aerial hit. |
| Gastly — Shadow Burst | B casts the normal orb; fresh B arms it at its current position. | Costs 8 extra stamina. Stationary 12-tick warning, then 4 active ticks in a 28×20 px area. Deals 10 damage rather than the ordinary orb's 12. Block, jump, leave the area or shoot it. |

Near-landing Up retains the normal jump buffer. Held Up cannot repeat Air Brake.
Landing restores one use; attacks and stun prevent it. Burst arming is allowed
from neutral or the released orb's firing recovery and does not cancel recovery.
It cannot cancel melee, guard, super or stun. The warning is harmless to fighters
and remains vulnerable to opposing shots. A burst can be blocked from either
side and hits only once. It has no tracking or repeated damage. Repeated B
cannot extend the warning or the normal projectile's lifetime.

Both techniques use the existing press queue, resource rules and GBA controls.
Pause freezes simulation and clears pending inputs. They work for either player
in Local Versus. Save schema 4 and solo progression exclusions are preserved.

Each fighter has a guide, HUD hints and a Technique training option. Air Brake's
lesson requires a braked descent and landing. Gastly's lesson requires a real
burst hit; a normal hit does not clear it. Select retains the fast retry. With
the keyboard adapter loaded, P1 O maps to Select for options and retries.

Air Brake reuses a musical-note effect. Shadow Burst has an outlined warning and
filled blast sharing its footprint. The native pixel test checks every phase.
Audio reuses fighter themes within the two-voice budget. CPU requests use the
same buttons, their own resources and delayed public opponent observations.

Validation covers costs, timing, held/fresh inputs, interruptions, hitstop,
reset, counterplay, drills, both players, persistence and frame budgets. Compare
untouched fighter pairings separately in the seeded matchup probe. CPU win
rates do not establish human fairness. Human timing, counterplay readability
and fun remain pilot gates; Rival Cup follows this iteration.
