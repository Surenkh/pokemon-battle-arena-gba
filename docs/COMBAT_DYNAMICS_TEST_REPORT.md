# v1.9 Combat Dynamics — validation and balance notes

## What changed

| Mechanic | v1.8 | v1.9 |
| --- | --- | --- |
| Opposing normal shots | Both disappear on contact | Both continue at 50% of their current damage, rounded up to at least 1 per shot |
| Jump stamina | 18 | 10 |
| Stationary Down dodge stamina | 12 | 6 |
| Squirtle directional Shell Roll stamina | 20 | 14 total |
| Blocked melee | Guard stops damage | Guard also pushes the defender 6 pixels, or the attacker for the distance blocked by an arena edge |
| Missed or blocked melee | Normal recovery | Four extra simulation ticks of recovery |
| Backstep | Unavailable | Hold L, tap away; 6 stamina, 2 startup + 6 travel + 8 recovery ticks, 15 pixels; no invulnerability |

Backstep is not available during ordinary hitstun. It can be queued near the
end of blockstun. Its cooldown starts after the action ends. Breakaway retains
its separate 60 SP hitstun escape. Super projectile interactions and Gastly's
armed Shadow Orb keep their existing behavior.

## Automated checks

- Twenty host suites include 3,805 combat-dynamics checks across all ordered
  fighter pairs and both seats. They cover clash pairs, later clashes with new
  projectiles, super and armed-orb interactions, resource costs, action
  priority, buffered inputs, wall spacing, interruptions, CPU backsteps and
  recovery penalties.
- Twenty-four native emulator flows exercise the built ROM, with zero missed
  display frames in their reported scenarios. The new two-player flow checks
  the shipped adapter, both weakened shots hitting, costs, both backsteps,
  guard push and save isolation. The depth flow includes 12,000 practice display
  frames and a real recovery punish after stepping back into range.
- A separate clean source snapshot regenerated art, audio and configuration and
  built a byte-identical ROM with the pinned devkitARM compiler.
- Asset, audio and publication-integrity tests passed. Detailed ROM-bound
  reports and native captures are in `verification/v1.9.0/`.

## Balance signals and remaining tests

For a repeatable regression signal, the same CPU policies played 10,140 rounds
on each ruleset, covering every ordered matchup, all three difficulties and 20
fixed seeds. Neither set timed out. Median active ticks changed from 855 to 853.
Psyduck's CPU win rate rose from 50.32% to 57.82%; Jigglypuff remained at
30.64% and Gastly at 33.27% in the new simulation. These are observations of a
particular CPU policy, not estimates of human win rates or evidence that the
roster is balanced. Full per-fighter results are in the accompanying baseline
and candidate JSON files.

Human play should check whether players recognize weakened shots, can find and
punish backsteps, can escape close pressure without creating a dominant defense,
and still use the super meter. Prioritize matchups involving Psyduck,
Jigglypuff and Gastly. Physical hardware, keyboards, GUI input and controller
routing remain unverified.
