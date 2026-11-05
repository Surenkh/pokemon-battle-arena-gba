# Rival Cup implementation

The v1.6.0 bracket gives each Cup three eligible challengers and one Captain.
The player chooses the first challenger and then picks from the two remaining
rivals after a win. Two challenger wins unlock the Captain. The run keeps the
chosen rival and authored arena through each retry. The three Cups share the
same three-fight length and one paid continue.

`config/cups.json` is the source for rivals, arena IDs, scouting cues, style
targets and Captain tactics. `tools/generate_config.py` generates the native
definitions and the asset painter uses the same source. The picker is screen
20 and follows the existing Cup board and result screens 18 and 19. The
picker staggers portraits and text across display frames and highlights
the current challenger. A defeated challenger remains visible with a
**DEFEATED** label and is skipped during navigation.

The final fight sets a Captain tactic on the ordinary `CpuController`. Rex
seeks melee range and favors follow-ups. Brick favors guards and visible
recovery punishes, with short guard windows at close range. Ren seeks ranged
pressure and retreats when the player approaches. Each tactic uses legal
button inputs, the existing fighter stats, the selected difficulty's
observation delay, and normal costs and recovery. The board and scouting
cards tell the player what to expect and how to respond.

`RivalCupRun` records defeated challengers with a bit mask and observes combat
events once per active display frame. Tempo counts damaging player follow-ups,
Guard counts successful incoming blocks, and Mirage counts damaging player
supers. The goal count caps at its target and carries across retries. Completion
and award guards prevent duplicate advancement or records.

The Captain win awards a clear and shows the Cup's crest. The board derives the
crest from the saved clear count for that Cup and difficulty, including clears
from earlier ROMs. `CupStore` keeps nine records in two 160-byte schema-1 copies
at SRAM offsets 4096 and 5120. It writes to the older slot, verifies the
checksum and payload, and selects the newest valid copy at startup. The base
save uses separate offsets for Arcade, mastery and audio. A Cup round may
earn normal mastery through the existing checkpoints. The Cup journal
writes at a completed run, and the next power-on starts at the title.

## Verification

Native runs exercise both choices, the defeated-card skip, locked retries,
all three Captain tactics, a crest per Cup, a real style clear and save recovery.
Host coverage runs every challenger order with every fighter and difficulty.
The package checks the exact ROM hash, zero missed native frames, and
byte-identical clean-build evidence. [v1.6.0 evidence](verification/v1.6.0/)

## Playtest

Observe five players who know the controls. Ask them to pick a matchup and
explain the Captain's cue before the fight. Record which challengers they pick,
which counter they try, whether they recognize the crest, and whether they
start a second run. Human pacing, difficulty, and cue comprehension are still
open for playtesting.
