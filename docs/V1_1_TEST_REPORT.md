# v1.1.0 Selection & Power — validation

Validated 3 November 2026 against the included ROM.

| Gate | Evidence |
| --- | --- |
| Native regression | 16 suites, 24,361 checks; zero missed display frames |
| Host regression | 13 suites, including single-screen geometry/navigation and effect bounds |
| Roster | All 13 cards visible in all 3 setups; exact full card-name and selected-header glyph masks; only 1 animated portrait |
| Settings | All 3 difficulty values and all 5 location labels; preview follows selection; actual Quick/Practice launches retain the exact fighter and arena |
| Navigation | First-frame value changes, held-input suppression, wrap, sparse columns, direct Select, Info/Back context and accurate Arcade rotation |
| Power presentation | All 13 fighters render all 8 aura/shot/impact frames and all 6 punch frames; actual HP changes retain base melee/super damage |
| Input response | All 130 one-frame trials retain F1 acceptance, F2 visible response and original projectile release ticks |
| Existing features | Both drills for every fighter; 8-frame manual/14-frame KO retries; records, mastery, schema-1/2/3 migration, schema-4 corruption recovery |
| Combat regression | 10,140 CPU rounds retain digest 1880322669; legacy-nine 4,860 rounds retain 1172440822; canonical data/core/save sources are byte-identical to RC2 |
| Assets | 925 body frames, 104 effect sheets / 598 frames, 69 PCM cues and one original music track |
| Reproducibility | Fresh pinned dependencies, regenerated art/audio/config and independent clean build from 2,241 source files produce the identical ROM |
| Distribution | 10 release-integrity checks; all 16 ROM-bound native reports required |

The highest measured native CPU peak is 91.94% of a frame
(depth suite). The chooser/power test peaks at
86.77%; Records/Mastery/audio stress peaks at
85.18%. All native suites report zero missed frames.
These are emulator CPU/frame measurements, not physical button-to-display latency.

The new native runner uses physical GBA buttons, exact framebuffer glyph masks,
real HP changes and per-owner read-only effect-frame telemetry. It exercises
full-meter supers through ordinary Practice, never writes live game memory, and
captures the real frames used in the README GIF. The particle limit remains 12.
The larger ribbons are decoration around the existing projectile center and do
not expand hitboxes or change damage, startup, recovery, stamina or power costs.

Version: `1.1.0`. Combat ruleset: `1.0.0-rc.2`. Save schema: 4 (596-byte copies).
ROM: 3,369,564 bytes; SHA-256:
`d4933cd8718b2711a7ca4eef3590e7a7314315f75ad4ba1939dc089ed7ee01b7`.

[Implementation](SELECTION_AND_POWER.md) · [Machine-readable evidence](verification/v1.1.0/)
· [Build instructions](BUILDING.md).

Physical GBA/controller sessions, non-Linux rebuilds, uncoached player pilots and
human matchup review remain unverified. Emulator evidence does not complete
those checks.
