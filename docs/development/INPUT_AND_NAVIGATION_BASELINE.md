# v0.9.0 input and navigation baseline

**Measured 2 November 2026.** mGBA 0.10.2, software framebuffer, released ROM SHA-256 `0ac4480de00ad3cdbed6df59e54195e46dd0d160ef1cd90e61ab0b449c7d29ff`. The driver injected GBA buttons for one emulated display frame and read existing telemetry. The image comparison used the actual 240 × 160 framebuffer. Hardware button, controller transport, host display latency and player perception require separate measurements. [Release assets](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/tag/v0.9.0) · [Driver and framebuffer capture](../../tests/mgba_driver.c) · [Native test setup](../NATIVE_TESTING.md)

## What happens after a one-frame tap

**F1** is the first completed emulator frame after the driver sets the button. **State** is the first observed change in the fighter's action, position or jump height. **Visible** is the first frame in which pixels around that fighter differ from a matched run with no tap. A difference can be a small pose change or the muzzle effect. A person still has to judge whether the move is readable. **Release** is the combat event that spawns the projectile.

In neutral Practice with an idle dummy, seven inputs across all nine fighters gave 63 trials. A, B, Up, Down, one frame of L, R with full meter, and Right all changed state on F1. Melee, jump, squeeze, guard, super and rightward movement changed local pixels on F2 for every fighter at the starting phase tested. The super projectile released on F31 in all nine cases. These traces recorded zero missed display frames.

### B press by fighter

The four visible columns use identical setups with 0, 4, 8 or 12 extra idle frames before the press. There are 36 B trials, including the nine in the initial sweep. All 36 entered FIRE on F1. The final column records release in the initial sweep.

| Fighter | Visible +0 | Visible +4 | Visible +8 | Visible +12 | Shot release +0 |
| --- | ---: | ---: | ---: | ---: | ---: |
| Pikachu | 10 | 10 | 10 | 6 | 9 |
| Charmander | 2 | 2 | 2 | 2 | 11 |
| Squirtle | 7 | 6 | 2 | 2 | 11 |
| Bulbasaur | 2 | 2 | 2 | 2 | 12 |
| Eevee | 2 | 2 | 2 | 2 | 9 |
| Meowth | 10 | 10 | 6 | 2 | 9 |
| Psyduck | 7 | 7 | 6 | 2 | 11 |
| Jigglypuff | 2 | 2 | 2 | 2 | 10 |
| Machop | 14 | 14 | 10 | 6 | 13 |

For Pikachu and Machop in the +0 trial, the first local visual difference appeared on the frame after projectile release. Charmander visibly moved on F2 with a projectile release on F11. That is a working reference for a fast acknowledgement and a readable windup in the same game.

The first Shoot tile in the packed artwork matches at least one idle tile exactly for Pikachu, Squirtle, Meowth, Psyduck and Machop. The other four have distinct first Shoot tiles. The pose renderer enters the Shoot clip as soon as FIRE is active. A shared pose can therefore hide acceptance until another tile or effect appears. These observations identify a cause worth fixing. The player's particular device and input sequence still merit reproduction. [Pose selection](../../include/pkmn_animation.h) · [Packed clip indices](../../assets/source/animations/manifest.json) · [Fighter timing definitions](../../include/pkmn_character_defs.h)

### Recovery and Practice reset

With Pikachu at starting range, whiff A and tap B once around the end of recovery. B at the first legal simulation tick, one tick before, or two ticks before starts FIRE on that legal tick. B three, four, five or six ticks before expires. This is one reproducible way a deliberate press can disappear. It occurs on a different part of the input path from the long visible firing windup. [Combat action and buffering](../../src/pkmn_combat_core.cpp)

From active Practice, tap Select once. On F1 the game begins its 180-frame countdown. On F181 it reports FIGHTING. A fresh A on F182 starts melee. A short retry here would let a player practice the same input many more times. [Screen transitions](../../src/pkmn_app.cpp)

## What happens in the menus

| Test | Observed on the released ROM |
| --- | --- |
| On the default Quick Battle menu row, tap Down | Highlighted choice changes to Arcade in telemetry on F1. The cursor region visibly changes on F2. Holding Down for 24 frames moves one item. |
| Open Quick Battle as Pikachu and tap Down four times | The selected fighter goes Pikachu → Bulbasaur → Psyduck. The third Down focuses Level and the fourth focuses Arena. A starts the fight as Psyduck. |
| Find Fighter Info in two setups | Select opens the guide in Quick/Arcade. Select cycles setup rows in Practice, where R opens the guide. The README omits Arena from its Select-in-Practice control row. |
| Read the largest main-menu description | The Fighter Mastery description is 31 characters in the fixed eight-pixel runtime font. The centered line spans 248 pixels in a 240-pixel viewport. Its bright pixels reach both edges in the released capture. |
| Move between Rematch and Change | The result background draws Rematch in a focused style. The runtime draws a second moving focus outline. Highlighting Change can leave two visual signals competing. |

Sources for the menu observations include the [native mode menu](../media/mode-menu-native.png), [nine-fighter selector](../media/nine-fighter-selector-native.png), [screen artwork generator](../../tools/convert_assets.cjs), [input routing](../../src/pkmn_app.cpp) and [player controls](../../README.md).

## Repeat the comparison on a candidate

1. Match the published ROM to its checksum. Use the ELF from the matching build to resolve `pkmn_debug`, `pkmn_depth_debug`, `pkmn_feel_debug` and `pkmn_arcade_debug`. The driver accepts `f <count> <keys>` for frames, `s` or `v` for read-only telemetry and `d` for a framebuffer dump.
2. Boot for 90 frames. On the title, tap B for Practice. Advance Right from Pikachu to the desired fighter and tap A to start. A navigation tap here is one pressed frame followed by three released frames. Advance 200 additional released frames, plus the chosen phase offset. Leave the default idle dummy and the players at their starting positions.
3. From that deterministic setup, run 36 frames with one button on F1 and release on F2. Run the same sequence again with no button. Compare the player region x = 32–96, y = 62–124 against the corresponding frame of the no-button replay. Read action or position each frame and the first SHOT or RELEASE event from the same run. A fresh boot for each case controls idle animation, seed and screen timing.
4. For recovery, use the end tick in `pkmn_depth_debug[25]` after Pikachu's whiffed A. On separate runs, inject one B at each offset and record the next FIRE start tick. For the menu test, boot for 90 frames, tap Start, settle 10 frames, and compare Down with no input in x = 12–229, y = 26–56.
5. Repeat the matched run on the candidate. Retain the ROM hash, all per-fighter timings, clips with sound off, and the worst observed frame. Film a button and its display together on the reported device to measure the player's complete path.

[Return to the product roadmap](ROADMAP.md)
