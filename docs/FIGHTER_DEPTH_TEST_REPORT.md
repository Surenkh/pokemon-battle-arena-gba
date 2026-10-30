# v0.5.0 validation — Fighter Depth & Practice

Tested 30 October 2026 using GBA Studio 0.5.0, bundled devkitARM 16.1.0, Linux x86_64 and mGBA 0.10.2. The restored v0.4.0 source first rebuilt its distributed ROM byte for byte before any edits.

The clean v0.5.0 ROM is **1,521,252 bytes**. SHA-256: `534783d13f68d6ec4d9928391f94b5710027617d844577a1834414706a9b0a06`.

## Functional and native results

| Check | Result |
| --- | --- |
| Combat host suite | 7,113,919 assertions; 720 seeded matches; all 36 ordered matchups |
| Movement host suite | 18,904 checks |
| Feedback/input host suite | 159,479 checks |
| Fighter depth/practice host suite | 22,995 checks; both sides, guard thresholds, reach edges, punish phases, confirms, defensive gaps, bounded chains and practice ownership |
| Existing assets | 408 character frames, 30 effect sheets / 120 frames and 27 original PCM imports validated |
| General native flow | 339 checks; 42 completed rounds; 36,989 battle display frames (10.27 nominal minutes); 40 player super activations |
| Native character animation | 1260 checks across six fighters |
| Native elemental effects | 378 checks; 6 rounds; all six identities |
| Native sound/feedback | 137 checks; 10 stress rounds; all six shot, charge and release banks played and actual stereo PCM captured |
| Native fighter depth/practice | 241 checks; every fighter lands A-to-B; Eevee lands close A-to-A; Meowth lands an 11-damage recovery punish |
| Native defensive/practice cases | Real guard costs of 23, 12 and 18 points; timed jump evades actual super; all dummy behaviors, guides, pause/reset and ordinary-mode restoration |
| Long practice replay | 12,000 display frames; 20 practice rounds including 19 automatic restarts; best sequence retained |
| Missed native display frames | 0 across all tested suites |
| Highest CPU cost in tested replays | 90.26% of a display frame |
| Static EWRAM / IWRAM | 34.9 KB / 26.8 KB; approximately 5.2 KB of IWRAM left for stack |

Native checks use physical button input and read-only telemetry, not game-memory writes. Guard and damage checks observe actual resource changes. Practice sequence counts require a damaging follow-up, and held buttons cannot create repeat attacks. Reports are under [verification/v0.5.0/](verification/v0.5.0/), bound to the tested ROM hash.

The initial practice HUD setup and the longer Eevee guide exceeded frame budget. Per-frame diagnostics isolated their allocation spikes. HUD name/stat caching, staged setup and deferred guide text removed the misses. The final tested peak leaves about 9.7% of its frame budget free; future cosmetic/audio expansion still needs profiling.

## Automated balance evidence

The same 2,160-seed schedule covers every ordered matchup at all three difficulties for both baseline and candidate. Median duration changes from 980 to 1002 active ticks; timeout rounds change from 10/2,160 to 0/2,160. The candidate includes traits, follow-ups, the 24-tick regeneration delay and basic CPU integration, so this is an aggregate comparison rather than an isolated test of one parameter.

| Fighter | CPU win rate: baseline → candidate | Active ticks below 20 stamina: baseline → candidate |
| --- | --- | --- |
| Pikachu | 45.42% → 66.94% | 77.72% → 48.29% |
| Charmander | 56.81% → 51.81% | 77.06% → 50.53% |
| Squirtle | 41.11% → 28.33% | 76.34% → 49.36% |
| Bulbasaur | 27.36% → 41.81% | 79.57% → 54.69% |
| Eevee | 63.19% → 54.17% | 76.92% → 45.74% |
| Meowth | 61.11% → 50.56% | 79.77% → 49.51% |

These results show a substantial reduction in low-stamina time while retaining similar match duration. They do not establish human fairness. The shared CPU policy can favor particular styles; Squirtle's defensive value and Pikachu pressure need human playtesting before further stat tuning. No HP, base damage or action-cost rebalance is claimed.

## Reproduction and limits

See [BUILDING.md](BUILDING.md) and [NATIVE_TESTING.md](NATIVE_TESTING.md). The source specification is [FIGHTER_DEPTH.md](FIGHTER_DEPTH.md). A fresh tracked-source snapshot passed offline npm ci, full graphics/UI/audio regeneration, asset validation and a clean native build. All 190 compared graphics/audio/header/content files matched the working project. The resulting ROM was byte-identical to the packaged download. See verification/v0.5.0/clean-build.json.

Physical GBA hardware, non-Linux rebuilding and human matchup testing remain unverified. Practice has no persistence, mirror opponents or input recording. There is no multiplayer, campaign or music in this release. Native test footage is an emulator capture, and the GIF is sampled from the real ROM rather than a browser reconstruction.
