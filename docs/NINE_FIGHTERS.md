# Nine-fighter roster — v0.8.0

These are this fan game's tuned fighting-game values, not canonical RPG base stats. Type labels are descriptive; there are no elemental damage multipliers. Clean attacks earn super power; supers require 100 SP and 25 stamina.

| Fighter | HP | Stamina | Regen/second | Speed (px/tick) | Melee | Projectile | Super | Trait |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| Pikachu | 95 | 105 | 26 | 1.50 | 8 | 12 | 32 | QUICK STEP |
| Charmander | 100 | 90 | 24 | 1.20 | 10 | 14 | 36 | EMBER PRESSURE |
| Squirtle | 115 | 110 | 22 | 1.05 | 7 | 11 | 30 | SHELL CONTROL |
| Bulbasaur | 120 | 100 | 20 | 0.95 | 9 | 12 | 34 | VINE REACH |
| Eevee | 100 | 115 | 28 | 1.40 | 7 | 10 | 31 | ADAPTABLE |
| Meowth | 90 | 100 | 25 | 1.55 | 8 | 11 | 33 | PUNISH |
| Psyduck | 105 | 105 | 23 | 1.15 | 8 | 13 | 34 | MIND PUSH |
| Jigglypuff | 125 | 100 | 21 | 1.00 | 7 | 10 | 30 | FLOAT STEP |
| Machop | 115 | 95 | 24 | 1.10 | 11 | 9 | 37 | HEAVY HANDS |

## New playstyles

- **Psyduck — Mind Push:** 896 Q8 projectile knockback, versus ordinary 512 Q8. Confusion Jab / Psy Pulse / Psychic Surge. A ranged-control CPU style values space and resource recovery.
- **Jigglypuff — Float Step:** 75% gravity and 105% jump impulse. Its native jump lasts 47 ticks versus 33 for the standard jump. Pound / Melody Note / Crescendo. A midrange CPU style mixes melee and melody projectiles.
- **Machop — Heavy Hands:** 135% outgoing melee guard pressure, 25px melee reach and deliberate recovery. Karate Chop / Focus Fist / Seismic Rush. A close-range CPU favors melee and recovery punishment.

The original six keep their previous per-fighter movement/guard/damage tuning. All nine use the same CPU reaction limits, collision rules, stamina economy and input path; difficulty does not grant stat bonuses.

## Visuals, animations and audio

All nine use 64×64 native sprite cells with 13 runtime clips: Idle, Walk, Attack, Shoot, Hop, Charge, Guard, Hurt, Squeeze, Land, Victory, Faint and Down. The expanded atlas contains 611 frames, including authored holds. Each fighter has four-frame normal/super projectiles plus themed cast, impact and guard effects: 45 sheets / 180 frames in total. Nine distinct normal-projectile silhouettes are verified.

The added body artwork comes from the existing credited PMDCollab/SpriteCollab revision `18c8a218537b984839803b95d588382624a8301a`: Psyduck `sprite/0054`, Jigglypuff `sprite/0039`, Machop `sprite/0066`. Original PNG sheets, offsets, XML and individual credits are retained. Each new fighter has original synthesized shot, hit, charge and release sounds; all 39 combat PCM files and 14 presentation cues use the existing moderate mix and voice limits.

Psyduck's upstream set lacks some event-specific poses. Its Guard/Faint use the authored Hurt animation, Land uses Idle with the game's landing squash, and Down uses Sleep. These mappings populate all runtime states without inventing source artwork or labeling generated body poses as original. The mapping is explicit in source_manifest.json.

Actual emulator GIFs for the new fighters are included in docs/media. Static PNGs and GBA BMP/import files remain reproducible from the tracked source inputs.
