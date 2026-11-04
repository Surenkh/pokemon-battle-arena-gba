# v1.3.0-rc.1 Fighter Techniques — validation

Validated 4 November 2026 against the included ROM and keyboard adapter.

| Gate | Evidence |
| --- | --- |
| Native regression | 18 suites, 24,660 checks; zero missed display frames |
| Host regression | 15 suites, including 314 technique checks on both sides |
| Air Brake | F1 acceptance, exact 8-stamina cost, bounded falling speed, changed landing timing, one use per jump, no extra height or invulnerability |
| Brake boundaries | Held/fresh repeat suppression, landing reset, near-landing jump buffer, hitstop queue, attack/stun rejection and hit interruption |
| Shadow Burst | F1 arming, exact 8 extra stamina, fixed position, 12 harmless warning ticks, 4 active ticks, one 10-damage hit, original firing recovery retained |
| Counterplay | Native guard, jump, retreat and shot-destruction trials prevent damage for either caster side. Passive defenders take 10. Host checks cover guard from both directions and the burst catching squeeze |
| Presentation | Exact framebuffer/palette masks verify all 8 warning/blast phases; native captures show both techniques |
| Practice | Both lessons complete through real inputs; ordinary hits and misses do not clear burst training; fast retries clear technique state; unsupported fighters show Free |
| Local Versus | Both players use techniques independently; Pause freezes state; pending inputs clear; reconnect requires explicit resume |
| Persistence | Practice/Versus leave the existing SRAM fixture byte-identical; save-schema migration and recovery suites pass |
| Reproducibility | Recovered source rebuilds the exact previously tested ROM; fresh regeneration and clean-build evidence are included |
| Distribution | 12 release-integrity tests; complete player ZIP includes both guides and the tested Lua adapter |

The dedicated technique runner performs 145 native checks through one upstream
mGBA core, actual raw-key/Lua APIs and the shipping script. The harness writes no
combat state; the script writes only its dedicated input mailbox. The ordinary
solo regression uses mGBA 0.10.2. Keyboard journeys use upstream development
revision a67176cd3e6bf60a5016d7adc94357142dd12617.

The technique journey peaks at 78.47% of a display frame. The highest measured
peak across the full regression is 93.58%. Records/Mastery/audio stress passes
its 90% gate. All recorded missed-frame counters are zero. These are emulator
CPU/frame measurements, not physical input-to-display latency.

The earlier ten-tick warning could still clip a responding jump. The shipped
twelve-tick warning passes that counter on both sides. Warning and active phases
are distinguished by native pixel masks.

The paired CPU probe runs 10,140 rounds per build: 20 seeds for every ordered
pair at all three difficulty levels. All 121 pairings between the other eleven
fighters (7,260 rounds per build) retain baseline result/timing summaries and
checksum digests. The candidate records 23 Air Brakes, 91 armed orbs and 30 burst
hits. CPU win rates are policy-dependent diagnostics: Jigglypuff changes from
27.95% to 27.31%, Gastly from 36.99% to 35.96%. Base stats are preserved; these
figures do not establish human balance.

Version/ruleset: 1.3.0-rc.1. Save schema: 4 (596-byte copies).
ROM: 3,434,140 bytes; SHA-256:
`19cbbce67727b2709d1da5977bbea966a653f0d721fe88e7d0afd23d7d6d726b`.

[Player guide](TECHNIQUES_PLAYER_GUIDE.md) · [Design](DISTINCTIVE_TECHNIQUES.md)
· [Machine-readable evidence](verification/v1.3.0-rc.1/).

Physical keyboards, Qt focus/shortcuts, gamepads, human timing/readability and
human matchup balance remain unverified. Two people should swap sides and test
landing traps, orb placement, every counter and rematches before expanding the
techniques to additional fighters.
