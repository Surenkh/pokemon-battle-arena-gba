# Mode menu, Arcade and records — v1.0 RC2

## Menu-first navigation

The title's Play button (A/Start) opens seven visible buttons: Quick Battle, Arcade, Practice, Records, Audio Settings, Controls and Fighter Mastery. D-pad movement and A/Start enter the selected mode; B returns. Quick/Arcade then present the thirteen-fighter grid across two pages and Easy/Normal/Hard row. Practice presents the roster and training/rival/arena options. Select in every setup opens settings directly for the chosen fighter. The visible Fighter Info row opens with A. B restores the row and then the chosen card. Legacy shortcuts may remain optional, but no primary menu function requires L/R.

The title displays v1.0 RC2 and the thirteen-fighter roster count. Keyboard letters belong to the emulator: GBA A/Start are the confirmation controls. Native GBA screens use controller navigation; mouse/touch behavior is provided by the emulator.

## Arcade contract

A seeded shuffle samples five distinct rivals from the other twelve Pokémon, with no mirror matches. Fighter and difficulty remain fixed throughout the run. Wins advance after the Poké Ball recall; the fifth win records one clear and shows Champion. Losses retry the same rival only after spending one of two continues. Draws retry freely. Pause Rematch also costs a continue; changing Pokémon abandons the run. With zero continues, Pause dims and skips Rematch, and A on the final loss screen finishes the run. New rounds reset HP/stamina/SP, projectile and CPU state.

The clock counts active display ticks, including hitstop and unsuccessful attempts. Pause/options, countdown, recall and result screens are excluded. Records track clears, no-continue clears and fastest total battle ticks for all 39 fighter/difficulty combinations. Unfinished runs are not saved. Seconds use 60 ticks, versus native refresh near 59.73 Hz.

## Recoverable persistence

Schema 4 uses 596-byte copies at offsets 1024 and 2048 for 39 record triples,
thirteen mastery pairs and audio preferences. The legacy copies at 0 and 512
remain untouched. Their schema-1/2/3 sizes are fixed at 240/348/420 bytes. Loading
validates the new copies first, falling back to old copies if neither is valid.
Existing records, audio and mastery migrate; new fighters start empty. Saving
writes payload/checksum to a new slot and verifies before advancing the active
copy. Keep the full SRAM file. [Migration details](ROSTER_EXPANSION.md).

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

## RC1 Records performance

Records uses a dedicated native background with fixed heading, row labels and
controls. Changing fighter or difficulty replaces only the name, level and
three values. Values align to the right at x=224; the longest valid 32-bit time
and six-digit counter fit. Direction taps still take effect on their first
eligible frame. A links to the same fighter's Mastery and returns to the same
level. See [the candidate evidence](V1_RELEASE_CANDIDATE_TEST_REPORT.md).
