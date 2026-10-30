# v0.6.0 — Smarter & More Varied Opponents

The CPU now uses each fighter's strengths, short tactical plans, resource reserves, and delayed observations. The player and CPU still share the same combat rules, costs, collision boxes, input queue and damage. Combat attributes, movement, follow-up timing and super rules are unchanged from v0.5.0.

## Ownership and configuration

`config/cpu.json` owns three difficulty profiles and six fighter styles. `tools/generate_config.py` generates `include/pkmn_cpu_defs.h` alongside the existing combat definitions. The generator validates roster order, bounds, history capacity and version agreement. Do not edit generated headers independently.

`CpuController` remains a fixed-size policy in `src/pkmn_combat_core.cpp`. It returns ordinary `Input` buttons; it cannot mutate the `const CombatCore&`. The native application calls it once per active simulation tick. Practice retains its separate, predictable dummy behaviors. There is no runtime network service, pathfinding engine or dynamic allocation.

## Public perception boundary

The 32-slot history stores only the opponent's character, visible position/velocity, facing, action, action start tick and attack identity; visible projectiles store position, direction, identity, owner and super appearance. This is a compact internal observation type, rather than a copy of `Fighter` or `Projectile`.

Opponent input buffers, action end tick, invulnerability expiry, stamina, HP, SP, resource timestamps, hit-confirm flags, projectile reservations, damage and birth tick are absent. The policy does not inspect the opponent's current state after constructing that snapshot. It reads its own resources, hit-confirm eligibility, projectile slots and squeeze cooldown directly, as a player can track their own actions. It reads the shared round clock.

Known action timings identify recovery and interruptible super startup from a delayed visible action. Observed movement is extrapolated for at most six ticks, clamped to arena boundaries. Projectile travel is extrapolated from its already observed velocity. These estimates can be wrong after a turn, landing or new action. No future player input is sampled.

Snapshots are recorded every active tick, including between decisions. Repeated queries in one tick return the same cached input and do not re-roll choices. Hitstop returns held buttons with no fresh press and does not advance perception, memory or decisions. Pauses do not call the policy. Round reset clears memory, decisions and random state.

## Difficulty profiles

All times below are active simulation ticks at nominal 60 Hz. Decision cadence adds up to eight/five/three ticks to observing a newly visible action.

| Profile | Observation delay | Think interval | Ordinary defense base | Super evade base | Follow-up chance | Deliberate hesitation |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| Easy | 20 | 9 | 40% | 45% | 25% | 22% of eligible decisions; 12 ticks |
| Normal | 12 | 6 | 72% | 72% | 72% | 10%; 6 ticks |
| Hard | 8 | 4 | 90% | 88% | 90% | 4%; 4 ticks |

Recovery-punish base chances are 25/65/90%. Fighter bonuses and recent habits can adjust eligible choices; all probability rolls cap at 95%. These percentages describe decisions, not guaranteed successful defenses or hits. Missing resources, locked actions, distance and late observations can prevent an action. Hard receives no stat bonuses.

## Six styles

| Fighter | Base spacing | Behavior |
| --- | ---: | --- |
| Pikachu | 24 px | Closes quickly, favors melee pressure and uses Quick Step to sustain a threat. |
| Charmander | 72 px | Prefers fire at a useful range; especially willing to use Ember Pressure against a visible guard. |
| Squirtle | 60 px | Favors affordable blocks, keeps room when possible and uses water knockback to reset spacing. |
| Bulbasaur | 34 px | Holds the edge of Vine Reach and uses longer recovery opportunities for counterattacks. |
| Eevee | 25 px | Favors close hit confirms; chooses A-to-A when the observed target remains close, otherwise A-to-B. |
| Meowth | 32 px | Looks for visible whiffs and interruptible charges, with an increased recovery-punish preference. |

Each CPU draws a short pressure, balanced or space plan for 90/72/60 ticks, with at most five pixels of spacing variation. Plans change movement and attack weights without changing attack startup or damage. Range thresholds and hysteresis prevent per-frame direction switching. Shot cooldowns encourage repositioning between ranged attempts. Airborne targets and active squeeze poses discourage wasteful straight projectiles.

Squirtle keeps room across all three plans when there is space; the corner prevents continuous retreat. Bulbasaur's actual reach is used for melee decisions. These preferences do not guarantee favorable matchups.

## Decision order and counterplay

1. Queue an eligible own hit-confirm follow-up through the unchanged three-tick buffer, with one probability roll per confirmed attack. Every follow-up retains full cost/startup and the one-cancel limit.
2. At a think tick, choose a short plan and target distance.
3. Consider a reachable visible recovery punish or melee interruption of super startup.
4. React to the nearest visible projectile, charge or sufficiently slow melee. Choose a response once per attack identity in independent normal-shot, super and melee slots, rather than re-rolling every tick. Normal attacks can be blocked if the estimated guard cost and a reserve are affordable. Supers never select block as their response; a jump or timed squeeze must fit the arrival window.
5. Recover resources, allow an intentional hesitation, then choose a super, melee, ranged attack or movement from the current plan.

Ordinary attack guard cost estimates use the same configured damage, Charmander pressure and Squirtle efficiency as combat resolution. Dodges can start too late, and attack commitments can be punished. Close fast melee is often faster than the observation delay, so it cannot be perfectly reacted to.

Higher levels remember only the last six observed offensive starts, expiring after 240 ticks without a new observed attack. Four repeated projectiles slightly increase defense/dodge preference. Four repeated melees permit a brief, fallible guard prediction during a space plan. This memory counts visible starts once by attack ID; it never predicts from buffered input. Switching patterns or waiting out the short memory defeats that habit.

Each style enters recovery below 18–24 stamina and resumes at 38–44. It may disengage for at most 24 ticks, then regenerates in place. Defense can still spend stamina during recovery. Four seconds without a change to its own HP or SP triggers a close approach; so do the last ten seconds of the ordinary round. This breaks ranged/guard stalemates. Defenses, stun, insufficient stamina and intentional mistakes can still delay an approach.

## Tradeoffs

A small, inspectable rules policy fits the GBA budget and allows reproducible seeded replays. It will have exploitable patterns. Visible velocity extrapolation and fixed cadence can misjudge a turn or recovery. Bounded randomness varies plans while retaining startup cues and reaction limits. More cautious spacing helped Squirtle in the measured policy matchups, but human balance is not established; no damage or HP changes were made to force a 50% result.

The complete CPU occupies 4,408 bytes on the host ABI. The native build uses about 19.9 KB static IWRAM (62.3%) and 34.9 KB static EWRAM (13.6%), leaving about 12.1 KB IWRAM for the stack. The compact history reduces static IWRAM compared with the 26.8 KB v0.5 build. Peak frame use is measured separately in the native reports.

## Verification

`tests/cpu_tests.cpp` checks hidden-state independence, observation delays, idempotent queries, bounded/expiring memory, resource recovery, super-defense choices, stalled zoning and deterministic games across all levels. Existing combat, movement, feedback and practice suites remain mandatory.

`tools/cpu_probe.cpp` compares the candidate against the frozen v0.5 Normal policy from `tests/support/v05_cpu.h`, using the current combat core, identical stats and both sides. Seeds 1–10 are the tuning set; 31–40 are a separate holdout. `tools/balance_probe.cpp` separately measures self-play. Report actual wins, timeouts and action mix; do not treat self-play as proof of human fairness.

`tests/native_cpu_flow.py` exercises all six opponent styles at all three difficulties with physical button input and read-only diagnostics. Existing native runners also verify the ROM. Audio opportunity coverage uses an actual Easy opponent so the external replay can earn and release every themed super; CPU difficulty coverage remains separate. The release pipeline runs the CPU host suite and requires a matching passed native CPU report.
