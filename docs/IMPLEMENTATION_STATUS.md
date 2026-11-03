# Implementation status — 3 November 2026

Candidate **1.0.0-rc.2** adds Vulpix, Gastly, Riolu and Totodile, Frostpeak and
Ancient Ruins, paged selection and schema-4 migration. Thirteen fighters support
guides, CPU styles, both drills, scouting, records and mastery. Five stages share
the same gameplay geometry and rotate through a five-rival Arcade run.

The original nine fighters retain their tuning. Thirty-nine records and thirteen
mastery pairs use 596-byte copies at 1024/2048. Schemas 1/2/3 migrate without
changing either legacy copy. The tested ROM, source-generated assets and
versioned reports ship together. See [validation](ROSTER_EXPANSION_TEST_REPORT.md).

This remains a prerelease. Uncoached pilots, technique transfer, motivation and
human matchup review, physical GBA/controller sessions and non-Linux rebuilding
remain unverified. Stable v1.0 is a separate decision; v0.12.0 remains latest stable.
