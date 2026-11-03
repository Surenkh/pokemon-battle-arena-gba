# Roster & Arena Expansion — v1.0.0-rc.2

The requested four Pokémon and two levels extend the game to thirteen fighters
and five arenas. Levels means battle stages; Arcade remains five rivals and two
continues. All new content is available immediately.

| Pokémon | Trait | Moves | HP / stamina |
| --- | --- | --- | --- |
| Vulpix | Fox Step advances melee startup | Ember Paw / Fire Spin / Flame Wheel | 95 / 100 |
| Gastly | Hover lowers gravity to 80% | Lick / Shadow Orb / Shadow Wave | 90 / 95 |
| Riolu | Aura Punish adds 4 damage against attack recovery | Quick Palm / Aura Pulse / Aura Storm | 100 / 110 |
| Totodile | Jaw Pressure adds melee guard cost | Bite / Water Fang / Tidal Chomp | 115 / 105 |

Each has thirteen runtime animation clips, distinct effects/audio, a guide,
CPU style, scouting cue, records and three mastery medals. Both Practice lessons
work for every fighter and cannot award mastery. Original-nine tuning remains
unchanged; the input buffer, combat rules and difficulty stat fairness remain.

Frostpeak has snowy ridges, pines and ice. Ancient Ruins has moonlit columns,
torches and stone flooring. Both support Quick Battle and Practice. Arcade
rotates all five arenas; retries preserve the current stage. Floor y=120,
collision geometry, bounds and combat rules are shared. Scenery motion stays
above the combat plane and stops outside active combat. Static bounds avoid
repeated array copies in the scanline calculation.

Selection uses two pages: the previous nine Pokémon and the four additions.
Left/Right crosses page boundaries and wraps; Up/Down stays on valid current-page
cards or opens settings below a column's last card. Select opens settings for
the chosen fighter. B and Info retain context. A header counter marks the page.
At most nine portraits allocate, two per display frame; old pages release first.

Schema 4 stores 39 record triples and thirteen mastery pairs in 596-byte copies
at offsets 1024 and 2048, each within an independent 1 KiB slot. Loading validates
current copies first, then falls back to legacy copies at 0 and 512. Legacy sizes
are frozen: 240/348/420 bytes for schemas 1/2/3. Headers are checked before reading
their exact payload length. Sequence wraparound, bounds and checksums are checked.

Migration retains old records, mastery and audio; new fighters start empty.
Saving writes the target new slot and checksum, verifies it, then advances the
active copy. Both legacy copies remain byte-identical. A damaged newest copy can
use its predecessor; two invalid new copies can recover the original snapshot.
Keep the full emulator SRAM file. Older ROMs only read their old snapshots and
cannot see new progress. Mid-run resume remains outside scope.

The assets contain 925 body frames, 65 elemental sheets/260 frames, 55 combat PCM
cues, fourteen presentation cues and one original music track. All new body art
uses the same pinned, credited CHUNSOFT source; the provenance manifest records
hashes and clip mappings. Native art and audio generation remains deterministic.
CPU probe results are regression evidence, not proof of human matchup balance.
Uncoached player sessions, expanded-roster matchup review and physical GBA checks
remain open before stable v1.0.
