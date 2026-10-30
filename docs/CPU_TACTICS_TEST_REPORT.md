# v0.6.0 CPU tactics — test evidence

Tested on 30 October 2026 with the pinned GBA Studio 0.5.0 CLI/Butano, devkitARM 16.1.0 (Linux x86-64), and mGBA 0.10.2 software rendering/audio. Native runners send physical buttons and read telemetry; they do not write game memory. The downloadable ROM is the exact validated build.

## Host checks

| Suite | Passing checks | Coverage |
| --- | ---: | --- |
| Combat | 5,990,944 | 720 seeded matches, all 36 ordered matchups, replay and combat invariants |
| Movement | 18,904 | Momentum, airborne control, landing queue and animation selection |
| Feedback | 139,443 | Events, pause, resource feedback and input buffering |
| Fighter depth | 23,621 | Traits, fair follow-ups, recovery damage and practice |
| CPU tactics | 1,221,407 | 144 replayed games across levels, hidden-state independence, delayed perception, idempotent queries, bounded/expiring memory, recovery, super responses and stalled zoning |

Total: **7,394,319 passing host checks**. Assertion counts depend on how long seeded games run; they are not a performance score. Asset validation also passes for 408 character frames, 30 elemental sheets/120 frames and 27 original non-silent PCM effects. Nine release integrity tests pass.

The hidden-state test perturbs opponent buffers, resource values, action end ticks, invulnerability, cooldowns, confirmations and invisible projectile metadata on every tick. Inputs remain identical for matched public observations. These properties are supplemented by the observation type's explicit public field list; the policy never stores a full opponent Fighter.

## Controlled opponent comparisons

Each candidate level plays the **same frozen v0.5 Normal CPU**, using the current combat core and identical stats. Every set covers all 36 ordered matchups, ten fixed seeds and both sides: 720 rounds per candidate level, 2,160 per set. Seeds 1–10 were used for tuning; seeds 31–40 were kept separate for confirmation. The frozen policy is committed under tests/support/v05_cpu.h. Reports include configuration/source hashes and replay digests.

| Candidate | Wins against reference, tuning seeds | Wins against reference, holdout seeds |
| --- | ---: | ---: |
| Easy | 13.19% | 13.33% |
| Normal | 52.78% | 47.78% |
| Hard | 68.33% | 68.33% |

All 4,320 comparison rounds finish without a timeout. Difficulty changes are measurable against a fixed opponent. These measurements do not predict a particular human player's win rate. Easy deliberately leaves substantial openings; Normal is near the old Normal reference in this test; Hard is stronger without stat bonuses.

In candidate self-play, all 2,160 rounds finish without timeouts; median length is 760 active ticks. Per-fighter low-stamina time is 11.19–16.09%, compared with 45.74–54.69% in the recorded v0.5 self-play probe. Changed opponents and action choices affect those percentages: they show less resource exhaustion in this policy set, not a controlled human stamina experiment.

| Fighter | v0.5 self-play wins | v0.6 self-play wins |
| --- | ---: | ---: |
| Pikachu | 66.94% | 53.06% |
| Charmander | 51.81% | 46.53% |
| Squirtle | 28.33% | 36.81% |
| Bulbasaur | 41.81% | 51.81% |
| Eevee | 54.17% | 57.08% |
| Meowth | 50.56% | 52.08% |

Squirtle's Normal policy wins 38.33% against the frozen reference on tuning seeds and 36.67% on the holdout set. This supports keeping its more defensive spacing; Squirtle still trails the fast fighters in automated play. No HP/damage buffs or nerfs were introduced to equalize this table.

## Native ROM checks

| Runner | Passing checks | Result |
| --- | ---: | --- |
| General | 387 | Six-fighter controls, pause, difficulty, results; 50 complete replay rounds and over 36,000 battle display frames |
| Animation | 1,260 | Walk/jump/combat clips, valid frame indices and frozen poses on pause |
| Effects | 378 | All six shot identities, four normal travel phases, earned supers, bounded effect pool |
| Feedback/audio | 127 | All six shot/charge/release banks, actual non-silent stereo captures, two-voice limit, fixed-HUD shake, charge pause, guard break and exhaustion |
| Fighter depth/practice | 241 | Six guides/follow-ups, Eevee A-to-A, actual guard costs, Meowth recovery punish, pause/reset, 12,000-frame practice replay with 19 automatic KO restarts |
| CPU tactics | 4,534 | All six opponent styles at each of three levels, delayed observations, memory/spacing bounds, tactical plans, sustained recovery, pause and 18 completion stress rounds |

Every suite reports **zero missed display frames**. Highest measured frame cost across the suites is **90.36%** of the native budget (practice flow); CPU-specific coverage peaks at **85.82%**. These are measured emulator scenarios, not a proof for every possible hardware situation. The general runner exceeds ten nominal battle minutes. Audio opportunity coverage selects Easy with actual menu input because the stronger CPU can interrupt the old external replay's super startup; the separate CPU runner covers all difficulties.

A fresh staged Git snapshot installs cached Node dependencies offline, regenerates graphics/character frames/UI/PCM and configuration, matches **191 native input files**, then performs a clean native build. The resulting ROM is byte-identical to the tested download. Static memory is about 19.9 KB IWRAM and 34.9 KB EWRAM, compared with v0.5's 26.8 KB IWRAM; compact public perception frees about 6.9 KB for the stack.

ROM: **1,518,188 bytes**. SHA-256:

```text
8c721aefac079a036ea909a44e5220ab13e84df0f79ac94f75cbf6d606b23f53
```

## Limits and next work

Physical GBA hardware, non-Linux rebuilding and human matchup balance remain unverified. A deterministic rules policy has exploitable patterns; visible prediction can misjudge turns, jumps and recovery. The reference CPU and automated human replay are limited opponents. Preserve these fixed comparisons for later changes and add human feedback before altering fighter stats. The next roadmap direction is arena life and replay progression.

Machine-readable reports are in verification/v0.6.0/. Commands are in BUILDING.md; behavior and tradeoffs are specified in CPU_TACTICS.md.
