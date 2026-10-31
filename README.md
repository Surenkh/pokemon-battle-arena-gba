# Pokémon Battle Arena

**Pick your Pokémon. Read your rival. Earn your super.**

![Promotional cover](docs/media/cover.jpg)

A native Game Boy Advance fan-made fighter by **Surenkh**.

[**Download the GBA game**](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/latest/download/PokemonBattleArena.gba) · [Player ZIP](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/latest) · [Checksums](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/latest/download/SHA256SUMS.txt) · [Release history](https://github.com/Surenkh/pokemon-battle-arena-gba/releases)

Version **0.7.0 — Sound & Poké Ball Recall**. Open the ROM in a GBA emulator such as mGBA. The release includes the ROM, a player ZIP with controls/credits, checksums and source archives. No commercial ROM or BIOS is included. A [repository copy](downloads/PokemonBattleArena.gba?raw=true) is also available.

The title and menus now play an original looping tune, with new navigation, countdown, pause, result and recall cues. Press **L** on the title or pause screen for independent **Music** and **Sound FX** controls. Both default to On with a moderate mix; choices last for the current run.

Normal fights end with a Poké Ball recalling the defeated Pokémon through an animated beam. The winner stays visible, and the ball closes before results. Tap A/Start after closing to continue sooner; practice keeps its quick reset.

CPU rivals now use fighter-specific spacing and attack preferences, varied tactical plans and stamina reserves. Easy leaves more openings; Normal mixes plans; Hard reacts and follows up more reliably. The AI reacts to delayed visible actions and remembers only a short history of repeated attacks.

Six fighter traits change movement, guard pressure, reach and counterattacks. A clean melee hit can lead into one projectile follow-up; Eevee can also follow with melee at close range. Follow-ups spend normal stamina and remain blockable. B on the title opens practice, with idle, blocking and attacking opponents, damage feedback and quick arena reset.

## Six fighters, six playstyles

| Fighter | Signature behavior | Attack visuals |
| --- | --- | --- |
| Pikachu | Quick Step advances grounded melee startup | Crackling lightning |
| Charmander | Ember Pressure drains more guard stamina | Flames and embers |
| Squirtle | Efficient guard impacts and stronger water pushback | Water pulses and splashes |
| Bulbasaur | Longer melee reach with a vine cue | Spinning leaves and vines |
| Eevee | Can follow a clean melee hit with A or B | Glowing stars |
| Meowth | Extra melee damage when catching attack recovery | Claw waves and scratches |

Normal projectiles and supers have four-frame animations. Muzzle flashes, hit effects, and blocked-hit effects match the attacker. Movement animations, momentum, stamina, CPU opponents, and charged supers remain part of the game. Supers bypass block and can be evaded with a jump or timed squeeze.

![Native Poké Ball recall capture](docs/media/pokeball-recall-native.gif)

[Watch the native music and recall preview](docs/media/sound-and-recall-native.mp4)

## Controls

| GBA button | Action |
| --- | --- |
| Left / Right | Move |
| Up | Jump |
| Down | Timed squeeze dodge |
| A | Melee |
| B | Fire |
| L held | Block |
| R | Super: 100 SP and 25 stamina |
| Start | Pause |
| Select on title/selector | Controls |
| B on title | Practice setup |
| R in either selector | Fighter guide |
| Select in practice setup | Cycle fighter / behavior / rival options |
| Select during practice | Reset arena, resources and practice statistics |
| L in menus/pause | Audio options |
| A / Start after recall closes | Skip to results |

Emulator key bindings depend on your setup. Clean hits earn super power; blocks and misses do not. For a follow-up, tap B when FOLLOW B appears after a clean A hit. Eevee may use A when FOLLOW A/B appears; stay close enough for the second melee hit. One follow-up is allowed per attack sequence, and the defender can block it.

Practice uses the same combat rules, starts with full SP and freezes the timeout. Choose IDLE, BLOCK or ATTACK behavior and a different rival Pokémon. KOs automatically restart practice while retaining the best sequence; Select manually resets it. Start opens the normal pause menu.

## Build and development

The editable GBA Studio project, C++ runtime, engine sources/notices, required source assets, tools, and tests are included. See [BUILDING.md](docs/BUILDING.md), [new audio/recall evidence](docs/AUDIO_AND_RECALL_TEST_REPORT.md), [audio and animation decisions](docs/AUDIO_AND_RECALL.md), [CPU architecture](docs/CPU_TACTICS.md), [fighter mechanics](docs/FIGHTER_DEPTH.md), [roadmap](docs/development/ROADMAP.md), and [Git workflow](CONTRIBUTING.md).

Gameplay updates use feature branches and pull requests. Validated versions on `main` are published through the [release workflow](.github/workflows/release.yml). Keep changes in feature branches and update the ROM and checksum together after validation. Compiler binaries are not bundled. Historical browser-preview exports and unused optional artwork from the original pack are omitted; native build inputs and their provenance are retained.

## Limits and feedback

Physical GBA hardware and non-Linux rebuilding have not been tested. There is one arena, no multiplayer, campaign, save system, or background music. For bugs, include version, emulator/device, fighters, reproduction steps, and expected versus actual behavior.

## Credits

Unofficial fan project, not affiliated with or endorsed by Nintendo, Game Freak, or The Pokémon Company. Pokémon belongs to its respective owners. Preserve the artwork credits in assets/source/ and the engine and third-party license notices in engine/.
