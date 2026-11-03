# Selection & Power — v1.1.0

All thirteen Pokémon appear on one screen in a five-column grid. Every card
has its portrait and full compact name; the selected name also appears at full
native font size in the header. The selected portrait animates. A gold outline
marks input focus and the check marks the fighter who will play.

The background holds the static cards and only the selected body uses a live
portrait sprite. Shared badges, a selected-card backplate and one preview keep
palette, object and upload costs bounded without hiding any fighters.

| Control | Result |
| --- | --- |
| Left/Right on cards | Previous/next fighter; wrap at roster ends |
| Up/Down on cards | Valid cards in the current column; Down below its last card opens options |
| Select | Switch directly between chosen fighter and options |
| Left/Right on a value | Change exactly one difficulty, location, training mode or rival |
| B | Return to the previous row/grid/mode with the choice intact |
| A on Fighter Info | Open the guide and return to the same row |
| A/Start on a card/value | Start with the displayed fighter and settings |

Quick Battle explicitly labels Difficulty and Location. The preview and full
location name follow the actual selection. Practice shares the layout with
Training, Rival and Location. Arcade displays rotation rather than a misleading
manual location, and skips that informational row. Long names fit between the
value arrows. Held directions do not repeat settings.

Super shots use eight 64×32 frames, retaining each fighter's elemental motif
inside larger moving ribbons and glints. Eight-frame charge rings and impact
bursts make preparation and contact more readable. Their bright core follows
the existing projectile center; outer ribbons are decoration, not a larger hitbox.

A attacks use the existing authored body poses with a six-frame energy fist:
anticipation, extension, contact and recovery. It follows the fighter's real
startup/active window and facing direction, and disappears when that action ends.
The charge and punch effects use each fighter's existing palette. Effects
follow the simulation tick, freeze with hitstop and reset at scene boundaries.
The bounded decorative particle pool remains twelve.

This release changes presentation and setup. Damage, stamina, meter, hitboxes,
input buffering, attack timings, CPU data, roster, arenas and schema-4 saves
remain unchanged. Existing Records and Mastery carry over. Human matchup review
and physical GBA/controller testing remain unverified.
