# Mode menu, Arcade and records — v0.12.0

## Menu-first navigation

The title's Play button (A/Start) opens seven visible buttons: Quick Battle, Arcade, Practice, Records, Audio Settings, Controls and Fighter Mastery. D-pad movement and A/Start enter the selected mode; B returns. Quick/Arcade then present the nine-fighter grid and Easy/Normal/Hard row. Practice presents the roster and training/rival/arena options. Select in every setup opens settings directly for the chosen fighter. The visible Fighter Info row opens with A. B restores the row and then the chosen card. Legacy shortcuts may remain optional, but no primary menu function requires L/R.

The title displays v0.12.0 and the nine-fighter roster count. Keyboard letters belong to the emulator: GBA A/Start are the confirmation controls. Native GBA screens use controller navigation; mouse/touch behavior is provided by the emulator.

## Arcade contract

A seeded shuffle samples five distinct rivals from the other eight Pokémon, with no mirror matches. Fighter and difficulty remain fixed throughout the run. Wins advance after the Poké Ball recall; the fifth win records one clear and shows Champion. Losses retry the same rival only after spending one of two continues. Draws retry freely. Pause Rematch also costs a continue; changing Pokémon abandons the run. With zero continues, Pause dims and skips Rematch, and A on the final loss screen finishes the run. New rounds reset HP/stamina/SP, projectile and CPU state.

The clock counts active display ticks, including hitstop and unsuccessful attempts. Pause/options, countdown, recall and result screens are excluded. Records track clears, no-continue clears and fastest total battle ticks for all 27 fighter/difficulty combinations. Unfinished runs are not saved. Seconds use 60 ticks, versus native refresh near 59.73 Hz.

## Recoverable persistence

Two independent SRAM copies live at offsets 0 and 512. Schema 3 uses 420 bytes per copy, retaining the schema-2 record layout and appending two mastery counters per fighter. Schema 2 uses 348 bytes per copy: explicit 32-bit magic/schema/sequence/audio fields, 27 counter/time triples, reserved zero and FNV-1a checksum. Schema 1 uses the earlier 240-byte, 18-record layout. Loading validates each copy using its own schema and checksum, chooses the newest valid sequence with wraparound handling, and supplies empty records for new fighters when migrating schema 1. Unknown tail bytes from a legacy slot are ignored. Schema 3 keeps original records/audio preferences and derives the no-continue mastery medal from existing perfect-clear records.

Saving invalidates only the inactive copy, writes payload then checksum, and verifies it before changing the active slot/sequence. The previous valid copy remains available, including mixed schema-1/schema-2/schema-3 recovery after an interrupted migration. Settings save on leaving changed Audio Settings; completed runs save at Champion. Failures leave the in-memory records available and show a save warning on progression screens. Neither slot valid means defaults, without overwriting SRAM at boot.

## Rendering budget

Nine selector portraits upload over several display frames. Text and Arcade HUD uploads are separated from background/fighter setup. The mode menu uses baked grouped labels and a joined focus outline. Canopy sway is bounded to one pixel above the combat plane and stops in countdown/recall; it does not move the floor, HUD or hitboxes.

Arena rotation, mastery rules, safe save boundaries and v0.9 evidence are described in [ARENAS_AND_MASTERY.md](ARENAS_AND_MASTERY.md) and [ARENAS_AND_MASTERY_TEST_REPORT.md](ARENAS_AND_MASTERY_TEST_REPORT.md). Original v0.8 evidence is in ARCADE_TEST_REPORT.md. Publishing remains a separate owner-approved step.

## Match goals and rival scouting

Every completed regular fight snapshots one relevant mastery goal from that
fighter's progress at match start to progress at results. New no-continue,
follow-up or evasion medals take priority, followed by a real technique increment.
With no increment, show the next unfinished goal, in follow-up/evasion/Arcade
order. A full set has a completed message. Results never award progress themselves.
Practice and abandoned fights do not show a mastery result. Each Rematch, paid
continue or next rival takes a fresh baseline.

On the existing Arcade result panel, a win identifies the next rival after the
normal stage advance. A usable loss or draw identifies the retry rival. Each
gets its own authored trait and one counter cue; no private AI intent is exposed.
Champion and exhausted runs have no scouting cue. Confirm paths, continues,
run time and save boundaries retain their existing behavior.

[Design, validation and open pilot](MATCH_GOALS_TEST_REPORT.md).
