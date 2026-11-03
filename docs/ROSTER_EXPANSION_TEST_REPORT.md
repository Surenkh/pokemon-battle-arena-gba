# Roster & Arena Expansion validation — v1.0.0-rc.2

Validated 3 November 2026. The tested ROM adds four Pokémon and two arenas.

| Gate | Result |
| --- | --- |
| Native regression | 15 suites, 23,472 checks; zero missed display frames |
| Host regression | 12 suites, including frozen-save migration, interruptions, single-bit corruption and actual fighter traits |
| Input response | 130 trials across 13 fighters; accepted on F1 and visible on F2; original release ticks retained |
| Selection | All 3 setups, both pages, sparse columns, wrap and 120 rapid page crossings; at most 9 portraits |
| Practice | Both lessons for all 13; manual retry frame 8 and automatic KO restart frame 14; mastery excluded |
| Stages | All 5 in Quick Battle and Practice; actual Vulpix five-win Arcade run visits all 5 |
| Progression | 39 records and 13 mastery pairs checked; actual new-fighter mastery and Arcade record survive power cycle |
| Saves | Independent frozen schema-1/2/3 fixtures; 596-byte schema-4 copies at 1024/2048; legacy bytes untouched |
| Recovery | Interrupted first/later saves; corrupt newest new copy; both new copies invalid with original legacy fallback |
| Matchups | 10,140 seeded CPU rounds across 169 ordered matchups and 3 difficulties |
| Original roster | Unchanged tuning and 4,860-round replay digest 1172440822 |
| Assets | 925 body frames, 65 elemental sheets/260 frames, 55 combat PCM and 14 presentation cues |
| Reproducibility | Fresh pinned dependencies, all source assets regenerated, independent clean build from 2,100 files; identical ROM |
| Packaging | 10 release-integrity checks and required ROM-bound evidence |

The maximum native CPU peak is 90.43% of a frame
(depth suite). The Records/Mastery/audio journey peaks at
84.18%; the expansion migration/play journey at
84.30%. Every suite has zero missed frames. Emulated frame
measurements do not establish physical button-to-display latency on every device.
Scenery bounds use static data to avoid per-scanline array copies; the bounded
motion remains unchanged. The floor, HUD and gameplay geometry stay fixed.

Save tests use actual buttons and read-only telemetry. The nine-fighter fixture
comes from the previous RC1 ROM; corruption is applied only while stopped.
New progress is earned by real play. CPU win rates are regression evidence,
not human matchup-balance claims.

ROM size: 3,221,768 bytes. SHA-256:
`ce79a2006b0fb5218794f12997eae1ccd790609c1b8b81fe5572ae8f4c8f1d28`.

[Machine-readable reports](verification/v1.0.0-rc.2/) ·
[Build instructions](BUILDING.md) · [Implementation contract](ROSTER_EXPANSION.md).

This is a prerelease. Uncoached player pilots, technique transfer, goal/scouting
retention, human matchup review including the additions, physical GBA/controller
sessions and non-Linux rebuilding remain open before stable v1.0.
