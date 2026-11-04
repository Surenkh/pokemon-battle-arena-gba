# Implementation status — 4 November 2026

**v1.3.0-rc.1 — Fighter Techniques Preview** adds Jigglypuff Air Brake and Gastly Shadow Burst, with stamina costs, readable cues, counters and Technique lessons in Practice. Both work on either side in Local Versus. The keyboard adapter adds P1 O for Select/options/retry. Save schema 4 and base fighter stats are preserved.

[Player guide](TECHNIQUES_PLAYER_GUIDE.md) · [Validation](TECHNIQUES_TEST_REPORT.md). Human readability, timing and matchup testing remain open.

## Previous iteration

**v1.2.0-rc.1 — Local Versus Preview** adds two keyboard players to one running
GBA game. An mGBA 0.11 development Lua adapter sends independent button streams
through a versioned 16-byte input mailbox. Both players choose from all thirteen
Pokémon on one page, can play mirrors and share any of the five locations.
Ready locks each choice; Player 1 controls location before either player readies.

Matches use first-to-two scoring, neutral P1/P2 results, Next Round, Rematch Set
and Change Pokémon. Either player can pause. Missing input packets pause the
match, discard pending actions and require explicit resume after reconnect.
Solo records/mastery remain excluded and save schema 4 is unchanged.

The one-core native test runs the shipping adapter through upstream mGBA's real
raw-key/Lua APIs. Physical keyboards, Qt focus/shortcuts and gamepads still need
human testing. This is a preview, while **v1.1.0 remains the stable release**.
[Local Versus setup](LOCAL_VERSUS.md) · [Validation](LOCAL_VERSUS_TEST_REPORT.md).

## Previous stable release

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
