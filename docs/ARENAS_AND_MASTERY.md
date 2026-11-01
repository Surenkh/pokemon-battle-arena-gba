# v0.9.0 — Arenas & Fighter Mastery

## Three arenas

Woodland, Volcanic and Shoreline use the same flat floor, world bounds, spawn
positions, hitboxes, movement and combat rules. Quick Battle has an Arena row
below Level; Practice adds Arena as its fourth setup option. A/Start still plays
from every setup row. Return to the grid to choose any fighter after configuring
the options. The chosen Quick/Practice arena stays selected during this session.

Arcade chooses a seeded starting arena and rotates through all three across five
rivals. Adjacent rivals use different arenas. Losses, draws and pause Rematch
retain the current arena; continuing to the next rival advances the rotation.
Arena selection never consumes combat/CPU randomness.

Each stage has a bounded one-pixel scanline motion region above the combat plane:
canopy at y=50–83, volcanic smoke at y=46–68, shoreline water at y=78–95. Motion
stops outside active fighting. The HUD and floor stay fixed except for the
existing intentional world-space impact shake. New scenery and badges are
original deterministic artwork authored in tools/arena_art.cjs, converted into
indexed native assets by tools/convert_assets.cjs. No external asset is downloaded.

## Three medals per fighter

| Challenge | Goal | What counts |
| --- | --- | --- |
| Follow-up hits | 10 | A resolved, damaging player hit marked as a follow-up |
| Super evasion | 5 | An incoming CPU super passes completely beyond the player after a nearby jump or active squeeze |
| No-continue Arcade | 1 | Any completed Arcade run with no continues used, on any difficulty |

Quick Battle and Arcade contribute to the first two counters, including attempts
that end in defeat. Practice never contributes. Counters stop at their goals;
re-reading events during hitstop cannot award them twice. Blocked follow-ups,
whiffs, enemy hits, invulnerability contacts and supers that expire without
passing the player do not count. The no-continue medal derives from existing
Arcade records, so legacy achievements receive credit immediately.

The observer in include/pkmn_mastery.h accepts a const CombatCore and never
changes simulation state. Tests compare its before/after combat checksum. It
tracks a super's unique attack id, whether it approached the player, a nearby
evasive pose and its full passage beyond both hitbox half-widths. A consumed
projectile cannot earn an evade. Each projectile can award once.

Fighter Mastery is the seventh visible mode button. Left/Right browses all nine
fighters; A switches between Mastery and Records. Roster badges show 0–3 earned
medals using dim, bronze, silver and gold variants. Round results announce a new
medal. Medals are cosmetic. All nine fighters and every arena remain available.

## Save compatibility

Schema 3 retains the original record offsets and appends two 32-bit mastery
counters per fighter. Each checked SRAM copy is 420 bytes at the existing offsets
0 and 512. Schema 1 (six fighters, 240 bytes) and schema 2 (nine fighters, 348
bytes) validate using their own original length and checksum. Unknown legacy
tail bytes are ignored. New counters begin at zero; records, audio flags and
existing no-continue medals remain intact.

Writes invalidate and replace only the inactive copy, verify it, then advance
the sequence. A valid older copy remains available after an interrupted write,
including mixed-schema migration. Host checks cover every single-bit corruption
and every interrupted payload length; native checks include actual SRAM loads,
offline corruption and power cycles using frozen legacy fixtures.

Mastery steps are accumulated in RAM during a battle and saved during the recall,
on Rematch, or on leaving a fight through Change Pokémon. Completed Arcade records
still save at Champion; audio saves when leaving changed settings. Save failures
retain RAM progress, show a warning and retry at the next save boundary. Abrupt
power-off can lose unsaved steps from the current battle. Mid-run resume remains
outside this version.

## Balance and validation

The combat ruleset and all nine fighter/CPU parameters remain v0.8.0. Automated
ordered-matchup, deterministic replay and CPU/difficulty checks cover this build.
No parameter is tuned on CPU results alone. Human matchup playtesting and physical
GBA profiling remain outstanding; emulator evidence does not establish either.

Native tests send real buttons and read telemetry, without gameplay-memory
writes. The mastery runner plays all three stages, earns a complete follow-up
medal from zero in regular matches, evades an actual CPU super, verifies training
exclusion, restarts SRAM and checks migration/corruption recovery. Static text in
the mastery background avoids allocating a full new text screen on each fighter
switch, keeping native display frames within budget.
