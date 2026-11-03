# Implementation status — 3 November 2026

**v1.1.0 — Selection & Power** replaces the paged chooser with all thirteen
Pokémon visible in a compact grid. Full selected names, Difficulty and Location
rows, a real arena preview and retained Back/Info context make setup explicit.
Only the highlighted body animates, keeping the native resource budget bounded.

Supers now use eight-frame 64×32 energy ribbons, charge rings and impact bursts.
A attacks gain a six-frame power-punch overlay tied to their actual action phase.
The twelve-particle limit and simulation-driven animation clock remain.

Combat data and save schema 4 are unchanged from RC2: thirteen fighters, five
arenas, thirty-nine record entries and thirteen mastery pairs. Earlier saves,
Practice exclusions, fast retries and no-continue rules remain compatible.

[Validation](V1_1_TEST_REPORT.md) records the exact ROM, regression, glyph/frame
checks, probe comparison and clean rebuild. Physical hardware, non-Linux builds,
uncoached player pilots and human matchup review remain unverified.
