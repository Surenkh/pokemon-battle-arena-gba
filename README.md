# Pokémon Battle Arena

**Pick your Pokémon. Read your rival. Earn your super.**

![Promotional cover](docs/media/cover.jpg)

A native Game Boy Advance fan-made fighter by **Surenkh**.

[**Download v1.1 ROM**](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/download/v1.1.0/PokemonBattleArena.gba) · [**Download v1.1 ZIP**](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/download/v1.1.0/PokemonBattleArena-v1.1.0.zip) · [v1.1 release notes](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/tag/v1.1.0) · [Latest stable release](https://github.com/Surenkh/pokemon-battle-arena-gba/releases/latest) · [Release history](https://github.com/Surenkh/pokemon-battle-arena-gba/releases)

Version **1.1.0 — Selection & Power**. All thirteen Pokémon now appear on one screen. Explicit **Difficulty** and **Location** rows, a full selected-name header and a matching arena preview make setup clearer. Supers have eight-frame charge, energy-ribbon and impact effects; A attacks have a six-frame power-punch animation. Combat rules and existing saves stay compatible. Open the ROM in a GBA emulator such as mGBA. [Validation report](docs/V1_1_TEST_REPORT.md) · [Build and controls](docs/BUILDING.md).

![Native v1.1 super shots and power punches](docs/media/v1-1-power-native.gif)

*Eight-frame energy supers and power punches — captured from the v1.1 GBA ROM.*

Press **A/Start on the title** to open the visible mode menu: **Quick Battle, Arcade, Practice, Records, Audio Settings, Controls and Fighter Mastery**. Choose a mode first, then select a Pokémon and difficulty or practice options. Menu selection uses the D-pad and A/Start, so entering Arcade does not require a shoulder button. The title displays **v1.1** and **13 fighters**.

**Psyduck, Jigglypuff and Machop** join the original six fighters, each with configured stats, a distinct trait, 13 runtime animation states, themed projectile/super/impact effects, combat sounds and a CPU style. Psyduck pushes rivals away with psychic attacks; Jigglypuff has lighter gravity and longer jumps; Machop drains extra guard stamina with melee. See [full fighter data](docs/NINE_FIGHTERS.md).

Arcade samples five unique opponents from the other twelve fighters at the selected difficulty. It grants two continues and restores resources between fights. Clears, no-continue clears and fastest total battle times save separately for all 39 fighter/difficulty pairs. Saves from the earlier six-fighter v0.8 build migrate automatically.

![Native mode menu](docs/media/navigation-menu-native.png)

![All thirteen Pokémon on one native screen](docs/media/selection-one-page-native.png)

The title and menus now play an original looping tune, with new navigation, countdown, pause, result and recall cues. Press **L** on the title or pause screen for independent **Music** and **Sound FX** controls. Both default to On with a moderate mix; choices save when you leave Audio Settings.

Normal fights end with a Poké Ball recalling the defeated Pokémon through an animated beam. The winner stays visible, and the ball closes before results. Tap A/Start after closing to continue sooner; practice keeps its quick reset.

CPU rivals now use fighter-specific spacing and attack preferences, varied tactical plans and stamina reserves. Easy leaves more openings; Normal mixes plans; Hard reacts and follows up more reliably. The AI reacts to delayed visible actions and remembers only a short history of repeated attacks.

Thirteen fighter traits change movement, guard pressure, reach and counterattacks. A clean melee hit can lead into one projectile follow-up; Eevee can also follow with melee at close range. Follow-ups spend normal stamina and remain blockable. B on the title opens practice, with idle, blocking and attacking opponents, damage feedback and quick arena reset.

Normal shots now show an immediate windup for Pikachu, Squirtle, Meowth, Psyduck and Machop. The firing cue appears by the second emulator frame after a sampled tap. See [response measurements](docs/RESPONSIVE_FIRING_TEST_REPORT.md).

## Five arenas and fighter mastery

Choose **Woodland, Volcanic, Shoreline, Frostpeak or Ancient Ruins** in Quick Battle or Practice. Press
**Select** from any fighter card to open settings directly. Up/Down chooses a
row and Left/Right changes its value. A check marks the chosen Pokémon while a
gold outline marks the focused row. **B** returns to the same card.

**Fighter Info** is a visible settings action. A opens the guide and B restores
its row. A/Start plays from the grid or a value row; Start also plays from Info.
All thirteen fighters stay visible. Select opens Difficulty and Location directly for the highlighted Pokémon; the preview follows the actual chosen arena. Quick Battle, Arcade and Practice share these controls. See [navigation validation](docs/NAVIGATION_TEST_REPORT.md).

Arcade rotates through all five arenas; retries preserve the current
stage. The arenas share the same floor, hitboxes and combat rules.

**Fighter Mastery** is a visible main-menu button. Each Pokémon has three cosmetic
medals: **10 damaging follow-up hits**, **5 successful super evasions**, and a
**no-continue Arcade clear** at any difficulty. Practice does not count. Roster
badges show earned medals, and Mastery/Records link with A. Progress saves at round
end, Rematch or leaving a fight. Schema-1/2/3 saves migrate to separate schema-4 slots;
existing no-continue records already count toward their medal.

Quick Battle and Arcade results now show one relevant mastery change from the
fight, such as **FOLLOW-UP HITS 9>10/10**. A newly earned medal takes priority.
If no counter advanced, the line names the next unfinished goal; a full set
shows **ALL 3 MASTERY MEDALS!**. In Arcade, the upcoming or retry rival has one
short trait/counter cue, for example **HEAVY HANDS: LEAVE SPACE**. A/Start still
continues directly. These additions are a playable prototype; player motivation
and the pilot retention gate remain unverified. See [goal/scouting validation](docs/MATCH_GOALS_TEST_REPORT.md).

![Native post-match progress](docs/media/match-goal-medal-native.png)

![Native Arcade scouting](docs/media/rival-scout-native.png)

![Native Frostpeak](docs/media/frostpeak-native.png)

![Native Ancient Ruins](docs/media/ancient-ruins-native.png)

![Native fighter mastery](docs/media/fighter-mastery-native.png)

## Thirteen fighters, thirteen playstyles

| Fighter | Signature behavior | Attack visuals |
| --- | --- | --- |
| Pikachu | Quick Step advances grounded melee startup | Crackling lightning |
| Charmander | Ember Pressure drains more guard stamina | Flames and embers |
| Squirtle | Efficient guard impacts and stronger water pushback | Water pulses and splashes |
| Bulbasaur | Longer melee reach with a vine cue | Spinning leaves and vines |
| Eevee | Can follow a clean melee hit with A or B | Glowing stars |
| Meowth | Extra melee damage when catching attack recovery | Claw waves and scratches |
| Psyduck | Mind Push gives strong projectile knockback | Psychic rings and pulses |
| Jigglypuff | Float Step lowers gravity and extends jumps | Musical notes and crescendos |
| Machop | Heavy Hands drains extra melee guard stamina | Fists and impact bursts |
| Vulpix | Fox Step advances melee startup | Twin flames |
| Gastly | Hover lowers gravity for longer airtime | Shadow orbs |
| Riolu | Aura Punish adds damage against recovery | Aura diamonds |
| Totodile | Jaw Pressure taxes guard stamina | Water fangs |

Normal shots retain their four-frame animation. Supers now use eight frames with energy ribbons, orbiting charge effects and expanding impact bursts. Melee uses a six-frame power-punch overlay synchronized to its real startup and active frames. Muzzle flashes, hit effects, and blocked-hit effects match the attacker. Movement animations, momentum, stamina, CPU opponents, and charged supers remain part of the game. Supers bypass block and can be evaded with a jump or timed squeeze.

[Watch the native music and recall preview](docs/media/sound-and-recall-native.mp4)

## Controls

| GBA control | Action |
| --- | --- |
| D-pad in menus | Select a mode button, Pokémon or option |
| A / Start in menus | Confirm / play |
| B in menus | Back to the originating row, fighter grid or mode |
| Select in any fighter setup | Jump between the fighter grid and settings |
| A on Fighter Info | Open the guide |
| Left / Right in battle | Move |
| Up / Down in battle | Jump / timed squeeze |
| A / B in battle | Melee / fire |
| L held in battle | Block |
| R in battle | Super at 100 SP and 25 stamina |
| Start in battle | Pause |
| Select in practice battle | Reset Free Practice or retry the drill |

The button labels describe GBA controls. Keyboard letters depend on the emulator's mappings; use its mapped D-pad and A/Start or its on-screen controls. All primary mode/menu functions are reachable without L/R shortcuts.

Emulator key bindings depend on your setup. Clean hits earn super power; blocks and misses do not. For a follow-up, tap B when FOLLOW B appears after a clean A hit. Eevee may use A when FOLLOW A/B appears; stay close enough for the second melee hit. One follow-up is allowed per attack sequence, and the defender can block it.

Practice uses the same combat rules, starts with full SP and freezes the timeout.
Its **Training** row offers **Free, Block, Attack, Follow-up and Evade Super**.
Free retains the idle rival and damage/chain feedback. Block and Attack keep their
normal dummy behaviors. Choose a rival and arena with the existing settings.

**Follow-up** starts in melee range and coaches a clean A hit into a damaging B
follow-up. **Evade Super** lets the rival charge one real super, then asks for a
nearby jump or timed squeeze. Short messages report a landed follow-up, an evade,
a real hit or an interrupted charge. Both lessons work with all thirteen fighters.
Practice progress stays separate from mastery.

**Select** retries with restored resources and positions. A fresh action works on
emulator frame **8** after a manual reset. A Practice KO holds for six frames and
restarts automatically, accepting a fresh action on frame **14** while keeping
statistics. Start still opens Pause. See [Practice validation](docs/PRACTICE_DRILLS_TEST_REPORT.md).

![Follow-up drill](docs/media/practice-followup-native.png)

![Super evasion drill](docs/media/practice-evade-native.png)


## Build and development

The editable GBA Studio project, C++ runtime, engine sources/notices, required source assets, tools, and tests are included. See [BUILDING.md](docs/BUILDING.md), [arenas and mastery](docs/ARENAS_AND_MASTERY.md), [modes and persistence](docs/ARCADE_AND_RECORDS.md), [nine-fighter details](docs/NINE_FIGHTERS.md), [v0.8 validation](docs/ARCADE_TEST_REPORT.md), [new audio/recall evidence](docs/AUDIO_AND_RECALL_TEST_REPORT.md), [audio and animation decisions](docs/AUDIO_AND_RECALL.md), [CPU architecture](docs/CPU_TACTICS.md), [fighter mechanics](docs/FIGHTER_DEPTH.md), [roadmap](docs/development/ROADMAP.md), and [Git workflow](CONTRIBUTING.md).

Gameplay updates use feature branches and pull requests. Validated versions on `main` are published through the [release workflow](.github/workflows/release.yml). Keep changes in feature branches and update the ROM and checksum together after validation. Compiler binaries are not bundled. Historical browser-preview exports and unused optional artwork from the original pack are omitted; native build inputs and their provenance are retained.

## Limits and feedback

Physical GBA hardware and non-Linux rebuilding have not been tested. There are five arenas and no multiplayer or mid-run resume. Mastery, completed Arcade records and audio settings use SRAM; retain your emulator’s save file. For bugs, include version, emulator/device, fighters, reproduction steps, and expected versus actual behavior.

## Credits

Unofficial fan project, not affiliated with or endorsed by Nintendo, Game Freak, or The Pokémon Company. Pokémon belongs to its respective owners. Preserve the artwork credits in assets/source/ and the engine and third-party license notices in engine/.
