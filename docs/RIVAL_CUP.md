# Rival Cup implementation contract

v1.4.0-rc.1 adds a short solo progression loop after the two-fighter technique
preview. The product goal is to make a return session purposeful: choose a
route, read a rival, complete three fights, then chase a style/perfect clear
or improve the time with another Pokémon. No new combat system or stat
bonuses are introduced.

The vertical slice has three authored Cups, nine named rivals, three fights
per run, one continue, fixed arenas and an optional resolved-event style
goal. All content is available immediately. `config/cups.json` generates the
native route definitions and feeds the asset painter. Menu mode ID 8 and
screens 18/19 are appended; previous mode/screen IDs stay stable.

`RivalCupRun` reads combat events once per active display frame. A follow-up
requires a damaging player EV_HIT with followup; Guard requires incoming
EV_BLOCK; Mirage requires a damaging player super EV_HIT. Counts stop at
their goal and survive retries. The observer never changes combat, input,
random state or CPU tuning. A round's event cursor is reset explicitly.
Completion and award guards prevent repeated advancement or duplicate awards.

`CupProgress` holds nine Cup/difficulty records, independent of chosen
fighter. Clears/style/perfect cap at 9999; style/perfect never exceed clears.
Fastest cumulative active display time uses uint32 ticks. `CupStore` writes
two 160-byte schema-1 copies at SRAM offsets 4096 and 5120. Each includes
magic, schema, sequence, all nine records and bytewise FNV checksum. Writes
invalidate the target, write payload/checksum last, then verify every word.
Sequence changes only after verified success. Loading selects the newest
valid copy with wraparound and performs no writes. Invalid Cup data cannot
invalidate schema-4 progress at 1024/2048 or legacy data at 0/512.

Cup completion writes only its journal. Ordinary solo mastery can update the
base store through the existing checkpoints; no Cup result calls Arcade's
complete function. Abandoning a run earns no Cup record. Practice/Versus do
not observe or write the Cup run. There is no running-Cup serialization.

The board shows three animated portraits; creation is staged across frames.
Scouting has an opaque countdown card to keep text legible over every arena
and fighter. Results stage heading, goal/scouting and footer uploads on
successive frames without delaying confirmation. Mirror rivals have
independent fighter palettes so a hit/recall tint cannot recolor both bodies.

Release gates: all fighters and all route/difficulty setups launch correctly;
three actual native clears; a real saved style achievement; native glyph
checks; loss/exhaustion/pause/abandon transitions; independent power-cycle,
corrupt-copy and interrupted-write recovery; all existing regressions;
zero missed display frames and a byte-identical clean build. Keep this as
an RC until a human pilot assesses pace, difficulty and whether players
voluntarily rematch for the optional goal. Native automation cannot establish
human fun or matchup fairness.

Pilot: observe five players who already know the controls. Ask them to choose
a Cup and explain its goal unaided, then play two runs. Record time to first
fight, failed retries, ability to name the next rival's counter, voluntary
rematches, and clear/style rates by difficulty. Target 4/5 understanding the
goal and 3/5 choosing a second run. If style goals discourage play, adjust
their targets independently of combat; if scouting is ignored, improve its
wording/presentation before adding more Cups or a large unlock system.
