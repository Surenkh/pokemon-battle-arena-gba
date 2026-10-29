# Animation and movement implementation contract

Release 0.2.0, ruleset 0.3.0. This document supersedes the original architecture's static-sprite animation and immediate-velocity movement. Damage, resource costs, controls and super-dodge rules remain unchanged.

## Animation data and renderer

There are 408 packed source frames: Pikachu 68, Charmander 66, Squirtle 75, Bulbasaur 65, Eevee 67 and Meowth 67. These are imported authored animation frames, including repeated holds, not 408 newly drawn or all-unique poses. All original source sheets, XML, offsets and credits are included at `assets/source/animations/`.

Each fighter has Idle, Walk, Attack, Shoot, Hop, Charge, Guard, Hurt, Squeeze, Land, Victory, Faint and Down clips. Walk sequences have four to seven source frames and three distinct rendered poses each. `manifest.json` maps every clip's packed offset, count, source duration, impact and return frame. `pkmn_animation_data.h` is generated from it.

`fighter_pose()` is a pure renderer input and cannot change combat results. Its precedence is reaction, attack, defence, airborne pose, landing, walk, idle. Countdown and result screens override this with display-clock animation. After an outcome, bodies settle to the floor cosmetically without inflicting further damage.

| State | Animation rule |
| --- | --- |
| Idle | Authored timing loop on simulation ticks |
| Walking | Travelled distance advances a stride; one cycle per 12 pixels |
| Melee / fire | Anticipation before startup, impact on the active tick, then recovery |
| Airborne | Separate rising, apex and falling poses from vertical velocity |
| Landing | Five simulation ticks, immediately cancellable by an action |
| Block | Defensive pose and shield arc |
| Squeeze | Preparatory pose, compact ten-pixel-high active artwork, recovery |
| Super | Charge sequence, pulsing shield and ranged release |
| Hit | Recoil sequence, hitstop and invulnerability flash |
| Results | Victory sequence or faint followed by down pose |

Combat increments `tick` after resolving a step. Attack rendering therefore uses `tick - began - 1`, aligning the visible impact with the collision step just completed. Hitstop freezes simulation ticks, body frames and particles. Pause freezes the fight. Menu portraits and countdowns use display time.

Every frame is 64×64 with a 16-color palette and foot anchor `(32,58)`. Sprite center is `(fighter.x, fighter.y - 26)` before screen-center conversion. The source body-center marker aligns horizontal position; feet align at row 58. The right-facing source row is extracted and hardware-flipped for left-facing combat. Single-direction event poses retain their authored view. Nearest-neighbour scaling generally doubles source pixels, with a cap to prevent clipping; compact squeeze frames fit the actual active hurtbox.

Only the current frame occupies object tile memory; fighter tiles change only when their frame index changes. Two battle fighters, six selector portraits and one result portrait never coexist. Six bounded particle slots show dust and impact sparks. Ground shadows shrink with height. Effects have no hitboxes and consume no gameplay resources.

## Physics

The simulation uses signed Q8.8 at nominal 60 Hz. `config/balance.json` is authoritative and generates C++ constants.

| Parameter | Value | Effect |
| --- | ---: | --- |
| Ground acceleration | 96 Q8 | 0.375 pixels/tick² toward character speed |
| Release braking | 96 Q8 | Smooth stop without overshoot |
| Reversal acceleration | 144 Q8 | Stronger braking against momentum |
| Air acceleration | 32 Q8 | Gentler control toward 75% walk speed |
| Air drag | 8 Q8 | Gradual momentum loss without direction |
| Jump velocity | −1088 Q8 | Existing takeoff velocity |
| Gravity | 64 Q8 | Existing jump arc, unchanged apex |
| Terminal fall speed | 1280 Q8 | Five pixels per tick |
| Normal / super horizontal recoil | 512 / 768 Q8 | Existing knockback, decays by 64 per tick |
| Normal / super hit lift | −320 / −640 Q8 | Small hop / stronger launch |
| Landing pose | 5 ticks | Cosmetic and immediately cancellable |

Movement approaches its target rather than instantly assigning velocity. Releasing direction brakes; reversing brakes more strongly. Takeoff retains horizontal momentum and then uses air control. Grounded actions stop voluntary movement; airborne attacks preserve captured momentum. Hit reactions cancel voluntary velocity and apply horizontal and vertical recoil. Wall contact clears velocity pointing outside the stage; pushboxes and clamps remain authoritative. Fighter centers are constrained to x=32…208 so the new 64-pixel animation cells stay inside the screen, including wide jump and recoil poses.

`stride_q8`, `landed_at` and `took_off_at` are deterministic fighter fields included in replay checksums. Landing records its exact simulation tick and clamps to the floor without tunnelling. Up while L is held exits guard into a jump; guard cannot be re-entered on that takeoff tick. The super's height and the active squeeze window are unchanged.

## Rebuilding and verification

All inputs are included; regeneration needs no download:

```sh
node tools/prepare_animations.cjs
node tools/convert_assets.cjs
python tools/build.py --devkitpro /path/to/devkitPro --clean
```

The existing combat suite covers all 36 ordered matchups, super dodges, resources and deterministic replays. `movement_tests.cpp` covers acceleration, braking, reversal, walls, air momentum, landing cancellation, hit lift, attack-pose timing and hitstop. `assets_test.cjs` rejects blank or clipped cells, inconsistent anchors, invalid clip bounds, incorrect palette formats and non-distinct walk cycles. `native_animation_flow.py` checks actual rendered walk/jump frame selection for all six characters using button inputs and read-only telemetry. `native_flow.py` runs complete battles, controls and rematches. See `TEST_REPORT.md` for the delivered ROM's measured results.

Outstanding: human playtesting for movement feel and readability, optional character-specific tuning, background music and physical GBA hardware testing. The existing title illustration remains its original animated ensemble.
