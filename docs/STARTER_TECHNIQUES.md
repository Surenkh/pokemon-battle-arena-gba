# Starter Mobility — v1.5.0

Give two familiar fighters a new spacing decision with the existing controls.
Pikachu commits to a quick approach/retreat. Squirtle trades eight extra stamina
for movement during an ordinary low dodge. Neither grants invulnerability,
extra damage, a recovery cancel, wall crossing or new progression rewards.

Canonical costs and timing live in config/balance.json and generate the native
technique header. The CPU supplies ordinary physical inputs using its existing
delayed public observations. New direction edges are limited to Pikachu. Other
fighters retain the previous policy and core behavior.

## Behavior

- Volt Step: two fresh same-direction presses within ten simulation ticks;
  neutral and grounded; twelve stamina; 2 startup + 6 movement + 8 recovery;
  four pixels per movement tick; twenty-four ticks of cooldown after recovery.
- Shell Roll: Down plus one horizontal direction; grounded and free to dodge;
  twenty total stamina; ten two-pixel movement ticks within the existing
  3 startup + 24 active + 9 recovery squeeze; ordinary cooldown retained.
- State and resources freeze during Pause and hitstop. Hitstop can queue a
  valid move once. Its latched direction is included in the checksum. Pause,
  round changes and retry clear a partial double-tap gesture.
- Ordinary damage interrupts movement. The low roll remains vulnerable to
  melee and wide bursts. Existing pushboxes and stage bounds apply.

## Learning and presentation

Practice Technique is available for Pikachu, Squirtle, Jigglypuff and Gastly.
Pikachu completes after a dash with room to move; a wall-blocked attempt asks
for retry. Squirtle completes only after a real rolling super evasion. Practice
and Versus award no solo mastery or Cup records. Existing save layouts remain
unchanged. Select keeps the short retry path.

Reuse the existing electric/water sprite banks and moderate-volume cues within
the current voice budget. Native captures show the committed body motion,
HUD label and lesson completion. Title version text derives from package.json.

## Delivery and next gate

The normal release workflow validates all host suites and ROM-bound native
reports. The dedicated release/v1.5.0 branch can publish this version before its
owner-controlled PR merge; other feature branches retain validation-only behavior.
The immutable v1.5.0 tag and downloads identify the tested source and ROM.
Human timing/readability, physical keyboard rollover, Qt/controller behavior
and matchup fairness remain open playtest work.
