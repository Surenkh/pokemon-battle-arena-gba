> **Implemented contracts:** `ANIMATION_AND_PHYSICS.md` supersedes the original static-sprite and immediate-velocity proposal below. `COMBAT_FEEL.md` describes v0.4 presentation events, audio, camera and landing input buffering. `config/balance.json` is the implemented numerical source of truth. The runtime uses 32 presentation events and 12 decorative effects; older proposed capacities below are historical. Character stats and super evasion rules remain unchanged. `FIGHTER_DEPTH.md` specifies v0.5 traits, follow-ups, resource recovery and practice; its contracts supersede older conflicting proposals below.

# Pokémon Battle Arena — GBA Architecture and Implementation Plan

**Document version:** 1.1  
**Reviewed:** 28 October 2026  
**Prepared:** 27 October 2026  
**Implementation agent:** DeepSeek running inside the user's coding Harness  
**Authoring/build tool:** GBA Studio  
**Runtime target:** Game Boy Advance ROM, using the Butano-based GBA Studio runtime  
**Source asset package:** pokemon_fighter_assets.zip  
**Status:** Detailed implementation specification; proposed starting balance, not a completed or playtested game

> Read this document before writing game code. Sections marked **MUST** define required behavior. The embedded JSON in Section 19 is the initial source of truth for combat values. All character statistics are custom values for this fighting game, not official Pokémon RPG statistics.

## 0. Architecture review and implementation priorities

**Original plan rating: 8/10.** This is an architectural assessment, not a measured game-quality score. The native integration, performance and balance remain unproved until implementation.

**What is good:** explicit resource arithmetic; character statistics and frame timing; geometry-based super evasion; deterministic simulation separated from presentation; bounded pools; an early native-build feasibility gate; honest asset limitations; and concrete acceptance tests.

**What needed improvement:** several combat edge cases permitted incompatible implementations, difficulty lacked a selection flow, CPU probabilities could accidentally be retried every decision tick, and prototype-versus-release art requirements were ambiguous. The rules below close these gaps without changing the six characters' initial balance.

### 0.1 Binding clarifications for version 1.1

These clarifications take precedence over conflicting examples later in this document. Section 19 remains authoritative for numerical balance. When extracting configuration, add the policies below as named constants and corresponding tests; do not invent alternate behavior.

| Area | Required behavior | Acceptance test |
| --- | --- | --- |
| Guard during blockstun | BLOCKSTUN retains its captured guard direction, even if L is released. Further front-facing ordinary impacts can be blocked and refresh the six-tick expiry. There is no holding drain in BLOCKSTUN. | Two ordinary impacts within six ticks both spend guard stamina and cause no HP loss when affordable. |
| Guard depletion | An impact with exactly enough stamina is successfully blocked, then causes 45-tick guard break at zero stamina without HP damage from that impact. An unaffordable impact causes full damage and guard break. Once guard breaks, later contacts in the same tick cannot use it. | Test impact costs minus one subunit, exactly equal, and plus one subunit. |
| Leaving block | When L is released outside BLOCKSTUN, end BLOCK before action arbitration. Attacking is then eligible in the same tick. Neutral/BLOCK auto-facing is determined before new actions from the previous completed positions; equal x retains facing. | Release L and press A together; one affordable melee starts. |
| Ordinary projectile clashes | Opposing ordinary shots destroy each other. Same-owner projectiles ignore each other. Opposing super/ordinary contact destroys only the ordinary shot; opposing supers pass through. | Verify all four pairings and no SP gain from clashes. |
| Contact ordering | Include projectile clashes and fighter contacts in one chronological list. Sort by contact time; exact ties resolve projectile clashes first, then stable instance id. Skip contacts involving already-consumed projectiles. Opposing melee contacts collected from the same snapshot may still trade. | A shot destroyed before reaching a fighter cannot damage that fighter later in the tick. |
| Projectile birth | Newly spawned projectiles test overlap at their spawn position at the end of this tick, with no movement sweep before birth. They first move next tick. A projectile born at T with lifetime L is removed before collision at T+L. | No immediate extra five-pixel movement on super birth; lifetime boundary checked. |
| Swept collision | Use current action-phase dimensions throughout the tick's relative sweep; previous kinematics supply positions, not an obsolete standing/squeezed shape. Use exact integer/rational contact comparison with sufficiently wide intermediates. | Active squeeze does not falsely collide because the previous tick was standing. |
| Repeated melee contact | Mark the target contacted on first geometric contact, including blocks and invulnerability rejection. It cannot be retried later in the same active phase. | Invulnerability expiring during the active phase does not create a delayed second contact. |
| Reaction aggregation | Apply the longest accepted stun duration, maximum hitstop, and strongest knockback magnitude. Equal-strength knockback uses the earliest accepted damaging contact. Do not sum hitstop durations. | Two same-tick hits cannot produce order-dependent reaction duration. |
| Input buffering | A press sampled at active tick T has attempts at T, T+1 and T+2. During hitstop it receives three attempts beginning with the next active tick. Buffer only temporary action lockouts; reject insufficient resources immediately. R at less than 100 SP is discarded, even if a hit fills SP later that tick. | R pressed at 99 SP cannot become an automatic super after meter gain. |
| Menu transitions | Clear combat buffers on pause, resume countdown, selection and match restart. Resample held buttons on entry and require release before a new press edge. | Start/A used to confirm cannot accidentally trigger a combat action. |
| Movement | Keep controlled horizontal velocity and knockback separate. Ground attacks zero controlled velocity; air attacks preserve the captured air velocity. Hit reactions clear controlled velocity but retain gravity. Apply knockback before its decay. | No residual walking drift during grounded attacks or hitstun. |

A successful ordinary block's optional two-pixel presentation push is **disabled for the initial ruleset**. Use the existing mandatory damage knockback; do not introduce an unspecified displacement.

### 0.2 CPU and menu completion

- Store a 32-entry ring of public simulation observations. At simulation boundary T, the CPU reads the snapshot from T minus its configured observation delay, clamped to the initial snapshot. Predict movement forward using only that delayed information; do not read current opponent state to correct predictions.
- Roll the super-evasion attempt probability **once per observed super instance**, including its visible startup. Cache the decision. Repeated six-tick thinking must not turn a 45% attempt rate into near-certain evasion. Assign the instance id when the super action is accepted.
- Self-resource/action eligibility can be checked against the CPU's own current state. Random decisions occur only on active simulation ticks. Reset observation history and threat decisions on rematch.
- Selector adds a difficulty row below the six cards. Left/right on cards follows the existing index behavior. Down from the bottom row focuses difficulty; Up from difficulty returns to the last highlighted card. Left/right on difficulty cycles Easy/Normal/Hard. A/Start starts with the highlighted character and difficulty; B returns to title. This replaces wraparound Down navigation from the bottom row.
- Preserve difficulty and both fighter choices on rematch. Pause includes Resume, Rematch, Change Pokémon and a compact control legend. Add the missing Paused → Countdown transition for Rematch.

### 0.3 Runtime capacity and delivery gates

**Separate collision candidates from presentation events.** Replace the combined 16-entry contact/event buffer with 16 contact candidates and 64 semantic events. Under the specified pools there are at most two melee-target pairs, four projectile-target pairs and three relevant opposing projectile pairs per tick. Recalculate this bound before increasing pool sizes. Apply HP/resource changes directly in CombatResolver; a missing cosmetic event must never suppress gameplay. Assert candidate overflow in debug builds; treat it as a release-blocking defect, not a reason to drop contacts. Add the CPU observation ring and all transient snapshots to measured RAM usage.

**GBA Studio integration remains the highest risk.** A source-level extension seam is not proof of a working editor integration. Milestone 0 must pin editor, Butano and compiler versions; prove the project's native entry, representative graphics import and clean rebuild; and record the actual commands. Keep any bootstrap patch in a project-scoped pinned engine checkout, not as an unexplained modification of the user's global installation. A custom C++ combat runtime will not automatically become editable through visual scene events. Record which assets/settings the editor owns and which logic is code-owned.

**Art completion has three explicit levels:** prototype may use static sprites and documented transforms; playable MVP requires visibly distinct block, squeeze, attack, hit and KO presentation for every character, with validated anchors/palettes; the 17 drawn frames per character are a polish target. An MVP may use explicitly declared procedural animation mappings. A release validator must reject missing required mappings or silent fallback, but should not reject an intentionally supported procedural mapping merely because it lacks a separate PNG. State the achieved level in release notes.

**Measure native performance:** account for BG tile maps as well as tile data, object palettes, sprite scanline pressure, audio and VBlank transfers. A valid object count alone does not guarantee correct rendering. Profile a ten-minute worst-case battle and repeated screen transitions with effects/audio enabled. Record peak frame cost and allocations; do not label estimates as measurements. Defer cosmetic expansion until the existing frame and memory budgets pass.

**Balance remains provisional.** Keep the initial stats unchanged until tests exist. In addition to win rates, record median match duration, timeout frequency, time spent unable to afford an action, super opportunities/uses, and successful super evasions. Compare mirrored sides with the same seeds, and test with humans; shared CPU policy can favor a fighting style and cannot establish fairness alone.

### 0.4 DeepSeek handoff contract

For each milestone, maintain docs/IMPLEMENTATION_STATUS.md with: completed requirement/test ids, changed files, exact build/test commands, actual results, unresolved blockers, and the next task. Generate character definitions from config/balance.json; never maintain an independently edited C++ copy. Replay fixtures must record ruleset version, configuration hash, initial seed, both character ids, difficulty and input frames. Serialize named fields in a fixed order for checksums; do not hash raw C++ struct padding or graphics handles.

Milestone 1 must include tests for every row in Section 0.1. Milestone 3 adds delayed-observation and one-roll-per-threat CPU tests. Milestone 4 tests reachable difficulty selection and all pause transitions. Release requires native boot and a complete playable round; this document and the browser preview are not that evidence.

## 1. Goal and interpretation

Create a responsive, side-view, one-versus-one Pokémon fighting game with:

- Pikachu, Charmander, Squirtle, Bulbasaur, Eevee and Meowth.
- An animated title screen with a Start action.
- A six-character selector.
- A woodland fighting arena.
- Movement, melee attacks, firing, blocking, jumping and squeezing.
- Super Power earned by successfully hitting the opponent.
- A super attack that ignores blocking but can be avoided by jumping or squeezing.
- Victory, defeat and a neutral draw variant.
- Rematch and Change Pokémon actions.
- One human player against a CPU opponent.

### 1.1 Definitions

| Term | Meaning |
| --- | --- |
| Life / HP | Remaining health. Reaching zero ends the fight. |
| Stamina / STA | Resource spent on attacks and defensive movement. Regenerates under defined conditions. |
| Super Power / SP | A separate 0–100 meter earned through successful ordinary hits. |
| Fire / shoot | Launch a character-themed ranged projectile. Only Charmander's projectile is literal fire. |
| Squeeze | A brief crouching dodge that compresses the character and lowers its hurtbox. |
| Hurtbox | The region of a fighter that can receive damage. |
| Hitbox | The damaging region of an attack. |
| Pushbox | The region preventing fighters from standing inside one another. |
| Tick | One authoritative simulation update. Timing values are measured in ticks. |
| Startup | Time before an action can hit or provide its defensive effect. |
| Recovery | Time after an action's active portion during which another action cannot begin. |
| Unblockable | Blocking does not prevent the hit; actual hitbox overlap is still required. |

**Interpretation of “squeeze”:** a short, timed crouch dodge activated by pressing Down. It is not teleportation, a dash, or general invulnerability. This assumption is deliberate and can be changed by editing the squeeze action definition.

### 1.2 Native target and Harness responsibilities

The deliverable is a working native .gba ROM and its editable project/source.

DeepSeek Harness is the development environment: it writes code, converts assets, runs builds and verifies behavior. No LLM, network connection or DeepSeek API is required while playing.

The supplied HTML previews, GIFs, PNG atlases and JavaScript renderer describe the artwork and screen flow. They do not automatically become native GBA runtime code.

## 2. GBA Studio baseline and feasibility

### 2.1 Tool identity

This plan assumes **dackmin/gba-studio**, whose official repository describes a GBA creator built on the Butano C++ engine. The release checked for this plan is **v0.5.0**, published on 21 October 2026. This is a planning baseline, not a claim about the user's installed version. [S1][S2]

If the installed application comes from another repository, first record its identity and inspect its actual project/build interfaces. Preserve the combat rules in this document while adapting the integration layer.

Do not silently substitute GB Studio, a Game Boy/Game Boy Color project, or a browser-only runtime.

### 2.2 Verified integration observations

The v0.5.0 source was inspected for this plan:

| Observation | Source location | Implication |
| --- | --- | --- |
| Project files use .gbasproj | src/cli/commands/build.ts | Create/open a genuine GBA Studio project. |
| The generated build includes project-local src and include directories | src/main/handles/build-project/index.ts | Keep authored C++ in the game project. |
| The runtime has a shared native bootstrap | public/templates/commons/src/main.cpp | A narrow bootstrap adapter is a feasible extension seam to investigate. |
| The builder emits native graphics metadata and invokes make | src/main/handles/build-project/assets.ts and index.ts | Integrate through the real asset/build pipeline. |
| The resulting ROM is copied into the project's out directory | src/main/handles/build-project/index.ts | Verify the actual output file after every milestone. |

These observations do **not** establish a documented drop-in fighting-game plugin API. The native integration proposed below must be proved with a minimal ROM before combat implementation. [S3][S4][S5]

### 2.3 Recommended integration design

Use GBA Studio for the project, asset registration and ROM build. Implement the deterministic fighting game and screen controllers in project-owned C++.

1. Create a minimal blank GBA Studio project and build the unmodified sample.
2. Prove that project-local C++ is compiled.
3. Check for a supported project entry/runtime hook in the installed version.
4. If no suitable hook exists, maintain a small, explicit local fork/patch of the shared bootstrap that calls the project's application controller after Butano initialization.
5. Let that controller own the title, selector, battle, pause and result states.
6. Keep all combat logic outside generated build files.
7. Record the patch, upstream tag/commit and exact build command in the project.

A proposed bootstrap contract is pkmn::run_app(), declared by project include/pkmn_app.h. This is a **new project interface**, not an existing GBA Studio API.

Conceptual adapter:

~~~cpp
// Integration sketch; adapt to the pinned source and prove it in Milestone 0.
bn::core::init();

#if PROJECT_HAS_PKMN_APP
    pkmn::run_app();       // Owns the native application loop.
#else
    // Retain the original GBA Studio bootstrap.
#endif
~~~

PROJECT_HAS_PKMN_APP must be introduced by a reproducible project-aware build flag or a guarded header-presence check. Do not assume this flag already exists.

**MUST:** there is exactly one owner of the frame loop. Do not run the standard scene movement/projectile loop and CombatCore against the same fighters.

**MUST:** no manually edited generated Makefile or build-directory file may be the only copy of a required change. Clean builds must reproduce the integration.

### 2.4 Feasibility gate

Before implementing the full game, demonstrate:

- The actual editor version, repository and project extension.
- A successful clean .gba build.
- A native test screen running in mGBA.
- A project-owned C++ function executing through the chosen bootstrap path.
- GBA button input moving a test fighter.
- Rebuilding after deleting build output preserves all authored behavior.

If the installed tool cannot support that route, document the failing interface and required adapter change. Continue writing configuration, pure simulation tests and asset conversion code that do not depend on that unresolved interface. Do not report a web preview as a native ROM.

## 3. Scope and initial product decisions

| Area | Initial requirement |
| --- | --- |
| Mode | Human versus CPU |
| Characters | Six; all selectable from the start |
| Arena | One flat woodland arena |
| Match format | One round |
| Round duration | 3,600 active simulation ticks |
| Round start | 180 display ticks of countdown |
| Win | Opponent reaches zero HP, or player has the higher HP percentage at timeout |
| Loss | Player reaches zero HP, or CPU has the higher HP percentage at timeout |
| Draw | Simultaneous KO or equal HP percentage at timeout |
| Difficulty | Easy, Normal and Hard; Normal is default |
| Native input | GBA D-pad, A, B, L, R and Start |
| Persistence | Not required for the initial release |
| Excluded initially | Campaign, evolution, items, online play, link-cable multiplayer and unlock systems |

Nominal timing is 60 ticks per second. Native GBA display refresh is approximately 59.73 Hz, so seconds stated here are convenient approximations; tick counts are authoritative. [S6]

The type labels Electric, Fire, Water, Grass/Poison and Normal are descriptive in this version. There are no elemental damage multipliers, critical hits or random damage rolls.

## 4. Application flow

~~~mermaid
stateDiagram-v2
    [*] --> Title
    Title --> Selector: Start
    Selector --> Title: Back
    Selector --> Countdown: Fight
    Countdown --> Fighting: Countdown ends
    Fighting --> Paused: Start
    Paused --> ResumeCountdown: Resume
    ResumeCountdown --> Fighting: 60 display ticks
    Paused --> Selector: Change Pokemon
    Paused --> Countdown: Rematch
    Fighting --> RoundEnd: KO or timeout
    RoundEnd --> Victory: Human wins
    RoundEnd --> Defeat: CPU wins
    RoundEnd --> Draw: Tie
    Victory --> Countdown: Rematch
    Defeat --> Countdown: Try Again
    Draw --> Countdown: Rematch
    Victory --> Selector: Change Pokemon
    Defeat --> Selector: Change Pokemon
    Draw --> Selector: Change Pokemon
~~~

### 4.1 State ownership

- Title owns its illustration animation, sparkle effects and Start selection.
- Selector owns the chosen character id and selected card.
- Countdown creates a fresh match, then prevents damage until it finishes.
- Fighting owns CombatCore and the CPU input controller.
- Paused freezes the complete combat state, including RNG, cooldowns and resource regeneration.
- RoundEnd freezes combat immediately and shows its final state for 60 display ticks.
- Result states display the selected Pokémon and valid actions.

A rematch preserves both selected character ids but resets HP, stamina, Super Power, projectiles, action state, AI state and timers.

Returning to selection preserves the highlighted player character. Starting a new fight from selection chooses a seeded-random CPU character different from the player. Debug tests may explicitly allow mirror matches.

## 5. Runtime architecture

### 5.1 Modules

| Module | Owns | Boundary |
| --- | --- | --- |
| AppController | Application state and screen transitions | Does not calculate combat damage |
| InputAdapter | Physical button samples and pressed/released edges | Does not mutate HP |
| CpuController | Delayed observations and CPU input commands | Uses the same commands as the human |
| CombatCore | Fighters, actions, resources, collision and outcomes | No graphics, filesystem or audio calls |
| CharacterCatalog | Immutable six-character definitions | Generated from validated configuration |
| ProjectileSystem | Fixed projectile pool and movement | No independent HP changes outside combat resolution |
| CollisionSystem | AABB and swept contact queries | Returns contact data |
| CombatResolver | Guarding, damage, meter gain and hit effects | Only authority for damage application |
| MatchController | Countdown, clock, timeout and final outcome | Reads resolved combat state |
| FighterRenderer | Sprite frame, facing, squash and palette effects | Reads simulation state |
| HudRenderer | HP, stamina, Super Power and timer | Does not own resource values |
| ScreenRenderer | Title, selector, pause and result graphics | Reads selection/result state |
| AudioRouter | SFX/music requests from semantic events | Cannot change gameplay timing |
| AssetCatalog | Native graphics and audio identifiers | Does not perform runtime PNG/GIF decoding |
| DebugRecorder | Input replay, state checksums and profiling | Disabled or bounded in release builds |

### 5.2 Data flow

Physical input and CPU-generated input are converted into the same InputFrame structure. CombatCore consumes two InputFrames and immutable definitions, updates once, and produces state plus bounded events.

Rendering and audio consume that result. They must not feed animation completion events back into damage timing.

**MUST:** attack timing follows action ticks, not GIF timing, sprite frame counters, sound duration or render speed.

### 5.3 Proposed project layout

The following is a proposed game layout, not a list of files already present:

| Path | Purpose |
| --- | --- |
| PokemonBattleArena.gbasproj | GBA Studio project |
| content/ | Editor-owned asset and project records |
| include/pkmn_app.h | Native application entry contract |
| include/pkmn_combat_types.h | Simulation structures |
| include/pkmn_character_defs.h | Generated immutable definitions |
| include/pkmn_combat_core.h | Pure combat API |
| src/pkmn_app.cpp | Native application state machine |
| src/pkmn_combat_core.cpp | Authoritative tick update |
| src/pkmn_collision.cpp | Contact detection |
| src/pkmn_combat_resolver.cpp | Damage and guard handling |
| src/pkmn_cpu.cpp | CPU controller |
| src/pkmn_fighter_renderer.cpp | Native sprite presentation |
| src/pkmn_hud.cpp | Three-resource HUD |
| src/pkmn_screens.cpp | Native menu/result screens |
| config/balance.json | Extracted Section 19 configuration |
| assets/source/ | Unmodified supplied asset pack |
| assets/native/ | Converted graphics/audio sources |
| tools/validate_balance.* | Configuration validator |
| tools/convert_assets.* | Reproducible asset conversion |
| tools/build_native.* | Wrapper around the verified build command |
| tests/combat/ | Host-native simulation tests |
| tests/replays/ | Seeded input fixtures and checksums |
| patches/gba-studio-bootstrap.patch | Narrow integration patch, if needed |
| docs/ENGINE_CAPABILITY_REPORT.md | Engine identity and proof results |
| docs/ASSET_CONVERSION_REPORT.md | Palettes, sizes, tile counts and remaining art gaps |
| docs/TEST_REPORT.md | Commands and actual verification results |
| out/PokemonBattleArena.gba | Built game ROM |

Confirm whether the pinned build recursively compiles source subdirectories. If not, keep source files directly in src until the source list is deliberately extended.

## 6. Controls and input arbitration

### 6.1 Native controls

| Input | In a fight | In menus |
| --- | --- | --- |
| Left / Right | Move horizontally | Move selection |
| Up | Jump, on a press edge | Previous row |
| Down | Squeeze, on a press edge | Next row |
| A | Melee attack | Confirm |
| B | Fire a ranged attack | Back |
| L held | Block while eligible | No required action |
| R pressed | Super attack if fully charged and affordable | No required action |
| Start | Pause/resume | Confirm Start / selected action |
| Select | No required release behavior | No required action |

Emulator keyboard and touch controls map to these physical GBA controls. Keep host key bindings configurable in the emulator rather than hardcoding a second combat ruleset.

### 6.2 Simultaneous inputs

Priority among eligible newly requested actions:

1. Super.
2. Jump.
3. Squeeze.
4. Fire.
5. Melee.
6. Held block.
7. Movement.

Opposite horizontal directions cancel. An ineligible request must not spend resources or prevent a lower-priority eligible request from being considered.

Accept at most one new resource-spending action per fighter per tick. If a higher-priority action starts, discard lower-priority presses from that same sample.

Use one pending press-action buffer lasting three simulation ticks. Store only the highest-priority pending intent. A new higher-priority intent may replace it. Held blocking and movement are not buffered press actions.

A, B, Up, Down and R require a fresh press; holding them does not automatically repeat their action.

A player may leave ordinary blocking to jump or squeeze if not in blockstun. Jumping and squeezing do not cancel an attack's startup or recovery.

## 7. Character characteristics

### 7.1 Proposed starting balance

These values are a first balance pass. Validate them through mirrored-side CPU matches and human playtesting.

| Pokémon | Role | Life HP | Max STA | STA regen / nominal second | Ground speed, px / nominal second | Melee damage | Fire damage | Super damage |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Pikachu | Fast ranged pressure | 95 | 105 | 26 | ~90 | 8 | 12 | 32 |
| Charmander | High damage, expensive pressure | 100 | 90 | 24 | ~72 | 10 | 14 | 36 |
| Squirtle | Durable defender | 115 | 110 | 22 | ~63 | 7 | 11 | 30 |
| Bulbasaur | Slow, durable control | 120 | 100 | 20 | ~57 | 9 | 12 | 34 |
| Eevee | Mobile stamina specialist | 100 | 115 | 28 | ~84 | 7 | 10 | 31 |
| Meowth | Fast close-range attacker | 90 | 100 | 25 | ~93 | 8 | 11 | 33 |

“Power” has two concrete meanings: attack damage in the table above, and the separate Super Power meter. Do not introduce an additional hidden attack multiplier.

All characters have:

- A 100-point Super Power cap.
- Zero starting Super Power.
- The same jump and squeeze clearance requirements.
- The same authoritative standing and squeezed hurtbox dimensions in the initial ruleset.
- No passive health regeneration.
- No random critical damage or type bonus.

### 7.2 Character moves and identities

| Pokémon | Melee | Fire / ranged | Super | Strength / tradeoff |
| --- | --- | --- | --- | --- |
| Pikachu | Quick Jab | Spark Shot | Volt Burst | Fast projectile and movement; lower HP |
| Charmander | Claw Swipe | Ember Shot | Inferno Wave | Highest damage; low stamina capacity and slower recovery |
| Squirtle | Shell Bump | Water Shot | Hydro Wave | HP and stamina support defensive play; slower movement |
| Bulbasaur | Vine Jab | Leaf Shot | Solar Pulse | Durable with solid damage; slow actions and regeneration |
| Eevee | Tackle | Swift Star | Swift Nova | High stamina and quick movement; lower ordinary damage |
| Meowth | Scratch | Coin Shot | Gold Rush | Fastest melee startup; lowest HP |

These are game move labels. All supers initially share the same collision profile and unblockable rule. Their colors, particles, sounds and names differ.

### 7.3 Ordinary action timing

All numbers are ticks. Startup S means the first active tick has action_tick = S.

| Pokémon | Melee startup / active / recovery | Melee total | Fire startup / spawn / recovery | Fire total | Projectile speed, px/tick |
| --- | --- | ---: | --- | ---: | ---: |
| Pikachu | 4 / 3 / 11 | 18 | 8 / 1 / 17 | 26 | 3.50 |
| Charmander | 6 / 3 / 14 | 23 | 10 / 1 / 21 | 32 | 3.00 |
| Squirtle | 6 / 3 / 13 | 22 | 10 / 1 / 19 | 30 | 2.75 |
| Bulbasaur | 7 / 3 / 15 | 25 | 11 / 1 / 20 | 32 | 3.25 |
| Eevee | 4 / 3 / 10 | 17 | 8 / 1 / 17 | 26 | 3.25 |
| Meowth | 3 / 3 / 10 | 16 | 8 / 1 / 18 | 27 | 3.00 |

For a 4 / 3 / 11 action:

- Ticks 0–3: startup.
- Ticks 4–6: active.
- Ticks 7–17: recovery.
- Tick 18: the fighter may accept a new eligible action.

A fire action spawns its projectile exactly once at the first active tick. Its one-tick spawn phase is not the projectile lifetime.

## 8. Life, stamina and Super Power

### 8.1 Life

- Initialize HP to the selected character's maximum.
- Subtract the move's listed damage only when a valid clean hit resolves.
- Clamp HP into 0..max_hp.
- Ordinary successful blocks remove no HP; there is no chip damage initially.
- Damage is a flat integer. Do not multiply it by the victim's sprite size or a display rating.

### 8.2 Stamina

| Action | Cost |
| --- | ---: |
| Move | 0 |
| Jump | 18 |
| Melee | 8 |
| Fire | 20 |
| Squeeze | 12 |
| Super | 25, plus 100 Super Power |
| Hold block | 12 points per nominal second |
| Successful ordinary block | Additional ceil(incoming_damage × 1.25) |

Compute the guard impact cost with integer arithmetic: (incoming_damage × 5 + 3) / 4, using integer division. For example, an 8-damage strike costs 10 guard stamina and a 14-damage shot costs 18.

Store stamina in integer subunits:

- One displayed stamina point = 60 subunits.
- max_stamina_subunits = max_stamina × 60.
- An action costing 20 stamina deducts 1,200 subunits.
- A regeneration rate of 26 points/second adds 26 subunits each eligible tick.
- Holding block deducts 12 subunits each tick.

This avoids floating-point regeneration drift and zero regeneration after integer division.

Regeneration begins after 30 simulation ticks without stamina expenditure. It runs only while action_state is NONE, including ordinary movement or airborne movement after that delay. It is disabled during blocking, attack phases, squeeze, stun, guard break, KO and pause.

Damage to HP does not directly subtract stamina. Blocking is the explicit exception through guard costs.

Reject an unaffordable action without partial spending. A rejected super keeps its full meter. Rejected attempts may show a restrained “not enough stamina” cue.

### 8.3 Super Power

| Event | Attacker gains | Defender gains |
| --- | ---: | ---: |
| Clean melee hit removing HP | 18 | 0 |
| Clean ordinary projectile hit removing HP | 22 | 0 |
| Blocked hit | 0 | 0 |
| Miss / whiff | 0 | 0 |
| Projectile hits another projectile | 0 | 0 |
| Hit rejected by existing invulnerability | 0 | 0 |
| Super attack hit | 0 | 0 |

Clamp the meter to 100. It does not decay during a fight.

The meter is awarded only by the combat resolver after it confirms positive HP damage. Sprite contact, attack animation playback and particles must not award power.

Examples:

- Five clean ranged hits award 110 before clamping, resulting in 100.
- Six clean melee hits award 108 before clamping, resulting in 100.
- Four clean ranged hits plus one clean melee hit produce 106, clamped to 100.

When the super request is accepted, deduct 100 Super Power and 25 stamina immediately. Cancellation or interruption does not refund either resource.

New meter gained later in the same tick cannot retroactively validate an earlier super request. A fresh eligible R press is required.

## 9. Defensive actions and the counter system

### 9.1 Block

Blocking is available only while grounded and facing the incoming attack.

- Entering block requires at least 8 stamina points.
- While held and valid, movement is stopped.
- Ordinary melee and ordinary fire can be blocked.
- Determine the required facing from the attack's travel/strike direction, not the current location of its owner.
- On a successful block, deduct its guard cost and apply six ticks of blockstun.
- No HP damage and no Super Power are awarded.
- A fighter in blockstun cannot instantly jump or squeeze.
- Holding L after blockstun resumes blocking if resources permit.

The continuous holding drain applies while action_state is BLOCK. BLOCKSTUN has its impact cost but no additional holding drain during its six-tick lockout.

If stamina is insufficient for an ordinary impact's full guard cost:

1. Set stamina to zero.
2. Apply the ordinary hit's full HP damage.
3. Enter 45 ticks of guard break.
4. Do not add another ordinary hitstun timer on top of guard break.
5. Regeneration remains disabled until guard break ends.

If block holding drains stamina to zero without an impact, also enter guard break.

A super **never** enters the ordinary block-resolution branch. If it overlaps a vulnerable blocking fighter, apply full super damage and super hitstun. Do not convert the impact into a stamina-only event.

### 9.2 Jump

- Press Up while grounded and otherwise eligible.
- Deduct 18 stamina.
- Set vertical velocity to -1088 in Q8.8 units, equal to -4.25 px/tick.
- Each airborne simulation tick first adds gravity of 64 Q8.8 units, then updates feet_y.
- Gravity is +0.25 px/tick².
- Clamp falling speed to +5 px/tick.
- Clamp landing to floor_y and set vertical velocity to zero.
- No double jump.
- Air horizontal control is 75% of ground speed.

With this integration order, an unobstructed jump reaches approximately 34 pixels above the floor. The collision test, not that approximation, decides whether an attack misses.

Melee and ordinary fire are allowed in the air when action_state is NONE. Their hitboxes/spawn height follow the current feet position. They do not reset vertical velocity.

Blocking, squeeze and super activation are grounded actions.

### 9.3 Squeeze

A Down press starts a fixed-duration low-profile action:

| Phase | Ticks | Hurtbox |
| --- | --- | --- |
| Startup | 0–2 | Standing |
| Low profile | 3–26 | Squeezed |
| Recovery | 27–35 | Standing |
| Total | 36 | — |
| Additional cooldown after ending | 24 | — |

- Cost: 12 stamina.
- Must start grounded.
- No horizontal movement, attacking or blocking during the action.
- A held Down button does not repeat it.
- No generic invulnerability is granted.
- A melee attack reaching low enough can still hit it.
- A hit interrupts squeeze and restores the standing hurtbox.
- Show an obvious compressed pose during the low-profile interval.

This prevents permanent crouching from defeating every ranged attack without commitment.

### 9.4 Required counter matrix

“Evade” assumes correct timing and non-overlapping geometry.

| Incoming attack | Correct facing block | Jump | Active squeeze |
| --- | --- | --- | --- |
| Grounded ordinary melee | Stops HP damage if stamina is sufficient | Can evade if hurtboxes separate | Usually hits the low hurtbox |
| Grounded ordinary fire | Stops HP damage if stamina is sufficient | Can evade | Passes above |
| Super | **Full damage if it connects** | Can evade | Passes above |

Air-fired ordinary shots follow their actual height. Do not assume a squeeze dodges an airborne shot that is genuinely low enough to intersect it.

## 10. Super attack specification

### 10.1 Shared action

| Property | Requirement |
| --- | --- |
| Activation | Fresh R press, grounded, eligible, SP = 100, stamina >= 25 |
| Meter cost | 100 |
| Stamina cost | 25 |
| Startup | 30 ticks |
| Spawn phase | One tick |
| Recovery | 24 ticks |
| Total owner action duration | 55 ticks |
| Wave speed | 5 px/tick |
| Wave hurtbox interaction | Width 12 px, height 12 px |
| Lifetime | Maximum 60 active simulation ticks |
| Tracking | None; horizontal direction is locked |
| Block | Ignored |
| Per-target damage | At most once per super instance |
| Super Power gain | Zero |

During startup, show a clear color pulse, sound cue and visible super-ready/activation feedback. Do not flash the entire screen in a way that hides the incoming wave.

Startup is interruptible. A valid hit before the spawn tick cancels the pending wave; the spent resources remain spent.

After spawning, the wave exists independently of the owner's subsequent action state. It remains until it hits, expires, leaves the arena or the round ends. A normal hit on its owner does not recall it.

The owner cannot move, block, squeeze or launch another action during the super action. There is no super armor or startup invulnerability initially.

### 10.2 Why jumping and squeezing both work

Use half-open rectangle coordinates: [left, right) and [top, bottom). Merely touching an edge is not a hit.

At the floor:

~~~text
floor_y = 120
standing hurtbox vertical interval = [92, 120)
squeezed hurtbox vertical interval = [110, 120)
super wave vertical interval = [96, 108)
~~~

Therefore:

- A standing fighter overlaps the wave.
- A blocking fighter uses the standing hurtbox and overlaps it.
- A fully squeezed fighter starts below the wave's bottom and does not overlap.
- A jumping fighter whose feet_y <= 96 is entirely above the wave's top.

The same geometry applies to all six Pokémon. Size differences in decorative ears, flames, bulbs and tails must not make one character incapable of the required dodge.

**MUST:** jumping and squeezing avoid a super through geometry. Do not implement “if jumping then ignore super” or “if squeezing then ignore all damage.”

### 10.3 Projectile interactions

- A super wave destroys an ordinary projectile it meets and continues.
- Destroying that projectile awards no power.
- Two super waves pass through one another initially.
- No ordinary attack, block animation or coin/leaf effect can stop a super.
- Standard post-hit invulnerability can still reject a hit; unblockable means bypassing guard, not bypassing the entire damage eligibility system.
- Jumping and squeezing have no such invulnerability of their own.

## 11. Coordinate system, movement and collision

### 11.1 World and presentation

- Logical screen: 240 × 160.
- World origin: top-left.
- floor_y: 120.
- Fighter body positions: x plus feet_y.
- Initial spawns: x = 64 and x = 176.
- Fighter center bounds: x = 24..216.
- No camera scrolling in the first arena.

Use signed 32-bit Q8.8 positions and velocities. A signed 16-bit Q8.8 value cannot represent the full horizontal screen range, so do not store world positions in int16.

When using Butano's center-based sprite placement, convert explicitly:

~~~text
sprite_top_left_x = fighter_x - anchor_x
sprite_top_left_y = feet_y - anchor_y

sprite_center_screen_x = sprite_top_left_x + frame_width / 2
sprite_center_screen_y = sprite_top_left_y + frame_height / 2

butano_x = sprite_center_screen_x - 120
butano_y = sprite_center_screen_y - 80
~~~

The provided main sprites use a 64 × 64 frame and anchor (32, 58). The bonus sheets have different anchors; read their metadata rather than assuming the same values. Butano's default placement convention is documented in its FAQ. [S7]

### 11.2 Core rectangles relative to fighter feet

| Rectangle | Left | Top | Right | Bottom |
| --- | ---: | ---: | ---: | ---: |
| Standing hurtbox | -7 | -28 | 7 | 0 |
| Squeezed hurtbox | -8 | -10 | 8 | 0 |
| Standing pushbox | -8 | -24 | 8 | 0 |
| Squeezed pushbox | -8 | -10 | 8 | 0 |
| Right-facing melee hitbox | 5 | -24 | 21 | 0 |

Mirror attack rectangles horizontally around the body anchor for left-facing attacks.

A normal projectile has a centered 6 × 6 hitbox, spawns 12 pixels ahead of its owner, and has center_y = owner's feet_y - 18 at its spawn tick.

A super has a centered 12 × 12 hitbox, spawns 14 pixels ahead, and has center_y = floor_y - 18.

Melee normally reaches the squeezed hurtbox because its vertical interval extends to the feet.

### 11.3 Separation and facing

- Grounded pushboxes must not overlap after resolution.
- Share ordinary push correction between the fighters.
- If one fighter is against an arena boundary, apply the remaining correction to the other.
- Fighters may cross above one another if their pushboxes do not overlap vertically.
- Neutral fighters auto-face the opponent.
- Attack, squeeze and stun states lock their facing until that action ends.
- Projectile direction is captured at spawn and never changes after the owner turns.

### 11.4 Contact detection

- Use AABB overlap for short-range melee.
- Use swept collision for moving projectiles, accounting for relative fighter movement.
- Every attack instance has a unique id and a target-hit mask.
- A multi-tick melee active phase can damage the other fighter only once.
- A projectile is consumed on accepted fighter contact, including a successful block.
- A projectile contacting a currently invulnerable hurtbox is also consumed, with no damage or power.
- A geometric miss does not consume a projectile until it exits or expires.

Collect hit candidates from the same pre-resolution combat snapshot. Use a deterministic contact ordering: earliest contact time, then attack instance id.

Resolve defender guard spending against a working copy in that order, then apply accumulated HP/stun results. Newly granted invulnerability does not erase other contacts already accepted from that same tick. This allows consistent simultaneous trades.

Do not cancel an already-collected opposing hit merely because the other fighter's damage was processed first.

## 12. Fighter state model

Keep life, locomotion and action state separate.

| State group | Values |
| --- | --- |
| Life | ALIVE, KO |
| Locomotion | GROUNDED, AIRBORNE |
| Action | NONE, BLOCK, MELEE, FIRE, SQUEEZE, SUPER, HITSTUN, BLOCKSTUN, GUARD_BREAK |

A fighter may be AIRBORNE + FIRE. A fighter cannot be BLOCK + SUPER or SQUEEZE + MELEE.

Derived presentation states such as RUN, JUMP_RISE and JUMP_FALL do not add another authority over combat.

### 12.1 Action eligibility

| Action | Ground | Air | Additional conditions |
| --- | --- | --- | --- |
| Move | Yes | Yes, reduced control | Controllable state |
| Jump | Yes | No | NONE or ordinary BLOCK; not blockstun |
| Melee | Yes | Yes | NONE; sufficient stamina |
| Fire | Yes | Yes | NONE; sufficient stamina and normal-projectile reservation available |
| Block | Yes | No | NONE/BLOCK; sufficient stamina |
| Squeeze | Yes | No | NONE/BLOCK; squeeze cooldown zero |
| Super | Yes | No | NONE; full meter, stamina and super reservation available |

Ground attacks lock controlled horizontal movement. Air attacks preserve gravity and existing momentum; new air steering is suspended until the action ends.

A clean hit interrupts ordinary actions and enters hitstun. Existing projectiles continue independently. A pending projectile reservation is released if its startup is cancelled.

### 12.2 Reaction timing

| Event | Stun | Hitstop |
| --- | ---: | ---: |
| Ordinary clean hit | 12 ticks | 4 display updates |
| Super clean hit | 24 ticks | 8 display updates |
| Successful ordinary block | 6 ticks | 2 display updates |
| Guard break | 45 ticks | 4 display updates |

After a clean HP hit, grant 12 simulation ticks of post-hit invulnerability starting on the next simulation tick.

Ordinary hit knockback begins at 2 px/tick; super knockback begins at 3 px/tick. Decay its magnitude by 0.25 px/tick toward zero. Apply it on later physics ticks and clamp to arena bounds. There is no vertical launch or wall bounce initially.

Blocking may add a two-pixel push away from the impact, subject to pushbox and arena constraints.

## 13. Authoritative simulation update

### 13.1 Two clocks

Maintain:

- display_tick: advances once per native frame; used for UI, pause animation and hitstop countdown.
- simulation_tick: advances only during an active, unpaused, non-hitstop combat update.

During hitstop:

- Continue rendering and sampling buttons.
- Freeze physics, action phases, projectiles, resources, AI decisions and the round clock.
- Keep at most one buffered press intent.
- Do not repeatedly spawn a projectile from a frozen active frame.

### 13.2 Tick order

For each active simulation tick:

1. Capture the human input and CPU command using the previous completed observation.
2. Determine new eligible actions and reserve any needed projectile slot.
3. Spend action resources once on accepted action entry.
4. Establish the current action phase and defensive dimensions.
5. Apply movement, gravity, landing, arena bounds and pushbox separation.
6. Spawn attacks at their one-time phase transitions.
7. Move existing projectiles and construct candidate contacts.
8. Resolve valid contacts, ordinary guards, guard breaks and damage.
9. Award Super Power for accepted ordinary HP damage.
10. Apply reaction states, cancellation and projectile cleanup.
11. Update eligible stamina regeneration, action/cooldown timers and invulnerability timers.
12. Decrement the round clock and determine KO/timeout outcomes.
13. Publish semantic events and a read-only render state.

A newly started action is evaluated at action_tick = 0 in this tick; increment its age only once at the end. At age = total duration on the next update it becomes eligible to end.

Hitstop requested during resolution begins on the following display update. A detected match end takes ownership immediately, freezing combat and starting the result delay.

### 13.3 Core pseudocode

~~~cpp
void CombatCore::step(const InputFrame& player, const InputFrame& cpu)
{
    if (match.phase != MatchPhase::FIGHTING) {
        return;
    }

    const KinematicSnapshot previous_motion = snapshot_kinematics();

    finish_expired_actions();
    accept_actions(player, cpu);
    establish_current_action_phases();
    advance_physics();
    spawn_once_for_active_phase();
    const CombatSnapshot before_resolution = snapshot_combat_state();
    collect_contacts(previous_motion, before_resolution, events);
    resolve_contacts_from_snapshot(before_resolution, events);
    award_clean_hit_power(events);
    apply_reactions_and_cleanup(events);
    update_resources_and_timers();
    update_round_clock();
    resolve_match_outcome();
    publish(events);
}
~~~

This is proposed project pseudocode. Its function names are not Butano/GBA Studio APIs.

The damage/guard snapshot is captured after this tick's accepted actions and defensive phases, before damage resolution. Previous kinematics are retained only for relative swept collision. A newly accepted block therefore works in that tick; a squeeze in startup still uses its standing dimensions.

### 13.4 Timer boundaries

Use explicit expiration ticks where practical. A normal action starting at T with duration D occupies [T, T + D); another action can start at T + D. A complete squeeze starting at T occupies 36 ticks and cannot start again before T + 60 because of its 24-tick additional cooldown.

A reaction or invulnerability granted by a hit resolved at tick T applies to the next F simulation ticks, T + 1 through T + F. Its exclusive expiry is T + F + 1. Do not immediately decrement a newly granted reaction timer during the same resolution that created it. A cancelled squeeze begins its additional cooldown from its actual cancellation time.

For stamina, record last_spend_tick and permit regeneration only when current_tick - last_spend_tick >= 30 and the state is eligible. Keep all of these tick rules independent of display-only hitstop updates.

### 13.5 Outcome rules

Check KO before timeout:

- Only CPU HP is zero: Victory.
- Only human HP is zero: Defeat.
- Both are zero from the completed tick: Draw.
- If neither is zero and the clock expired, compare HP percentages.

Use integer cross multiplication:

~~~text
human_score = human_hp × cpu_max_hp
cpu_score   = cpu_hp × human_max_hp
~~~

The higher score wins. Equality is a draw.

On match end:

- Stop accepting combat actions.
- Clear all pending damage, projectiles and reservations.
- Freeze the combat clock and resources.
- Show the final pose for 60 display ticks.
- Enter the corresponding result state.

Do not award a winner merely because one fighter was iterated first.

## 14. Bounded runtime data

Suggested structures:

~~~cpp
struct FighterState {
    CharacterId character;
    int32_t x_q8;
    int32_t feet_y_q8;
    int32_t velocity_x_q8;
    int32_t velocity_y_q8;

    uint16_t hp;
    uint16_t stamina_subunits;
    uint8_t super_power;

    LifeState life;
    LocomotionState locomotion;
    ActionState action;
    Direction facing;

    uint16_t action_tick;
    uint16_t squeeze_cooldown;
    uint16_t regen_delay;
    uint16_t invulnerability_ticks;
    uint32_t attack_instance_id;
};

struct InputFrame {
    uint16_t held_buttons;
    uint16_t pressed_buttons;
    uint16_t released_buttons;
};

struct Projectile {
    bool active;
    bool reserved;
    uint8_t owner;
    AttackKind kind;
    uint32_t instance_id;
    int32_t x_q8;
    int32_t y_q8;
    int32_t previous_x_q8;
    int32_t previous_y_q8;
    int32_t velocity_x_q8;
    uint16_t lifetime_ticks;
    uint8_t damage;
    uint8_t target_hit_mask;
};
~~~

Use fixed-capacity storage:

| Resource | Capacity |
| --- | ---: |
| Fighters | 2 |
| Active/pending melee instances | 2 |
| Ordinary projectile slots | 2, one per fighter |
| Super projectile slots | 2, one per fighter |
| Total projectile slots | 4 |
| Contact candidates | 16 |
| Semantic event buffer | 64 |
| CPU observation snapshots | 32 |
| Cosmetic effects | 8 |
| Buffered press intents | 1 per fighter |
| Debug event ring, debug only | 256 |

Reserve the projectile slot when a fire/super action is accepted. If no slot is available, reject the action without spending. Release reservations on pre-spawn cancellation.

No per-tick heap allocations are allowed in CombatCore. An exhausted cosmetic pool may drop an effect; it may not drop gameplay damage or alter a hitbox.

## 15. CPU opponent

### 15.1 Fairness

The CPU uses the same InputFrame, resource requirements, action states, collision and cooldowns as the player.

It may observe public positions, visible actions, visible projectiles, meter states and HP. It must not inspect future human input, bypass startup, refill resources or change damage by difficulty.

### 15.2 Difficulty

| Difficulty | Observation delay | Think interval | Attempt super evasion when available |
| --- | ---: | ---: | ---: |
| Easy | 18 ticks | 6 ticks | 45% |
| Normal | 12 ticks | 6 ticks | 70% |
| Hard | 8 ticks | 6 ticks | 85% |

Percentages describe decision attempts, not guaranteed successful dodges. A committed action, insufficient stamina, delayed observation or poor timing can still cause a hit.

### 15.3 Decision priorities

1. Respect KO, hitstun, recovery and pause.
2. Evaluate an observed incoming super.
3. If evading, estimate arrival time from visible position/velocity.
4. Prefer a jump centered near the expected impact, or a squeeze whose low-profile window covers it.
5. Evaluate ordinary projectile blocking when grounded and affordable.
6. Use melee at valid close range.
7. Fire when the path is useful and a projectile slot is available.
8. Consider a super when charged, with a plausible standing target.
9. Reposition or recover stamina.

For a super, useful starting targets are jump activation around 16 ticks before expected impact and squeeze activation around 12 ticks before impact. These are prediction heuristics, not new collision rules.

The CPU must not consider blocking a valid super to be a successful defensive answer.

Use a seeded deterministic RNG for decision variation. Cosmetic particles use a separate generator/hash so changing visuals does not change combat decisions.

## 16. Screens and UI

### 16.1 Existing assets to reuse

| Need | Existing asset/reference |
| --- | --- |
| Animated title reference | screens/start_screen.gif |
| Title frame sheet | screens/title_animation.png and title_animation.json |
| New six-character title artwork | screens/layers/title_illustration.png |
| Full-resolution title artwork | sources/title_ensemble.png |
| Selector reference | screens/pokemon_selector.png |
| Battle reference | screens/fighting_screen.png |
| Clean fighting environment | screens/layers/fighting_background.png |
| Victory reference | screens/winner_screen.png |
| Defeat reference | screens/loser_screen.png |
| UI states | ui/buttons/ and ui/buttons.json |
| Screen bounds/flow | screens/screens.json |
| Browser interaction reference | GAME_FLOW_PREVIEW.html |

The browser asset pack is an input to native authoring. Preserve source files and generate native derivatives.

### 16.2 Title

- Keep the six-Pokémon illustration and visible Start action.
- Recreate gentle bobbing, sparkles and a blinking prompt natively.
- Use a nominal 288-tick loop, corresponding to the reference's approximate 4.8 seconds.
- A or Start opens the selector.
- Do not embed a runtime GIF decoder.
- Use one foreground background layer for the wide illustration and move that layer slightly; it is larger than one native sprite.
- Use small cosmetic sprites for sparkles.

### 16.3 Selector

- Six cards in a three-column, two-row layout.
- Order: Pikachu, Charmander, Squirtle; Bulbasaur, Eevee, Meowth.
- Left/right changes the index by one; up/down changes it by three, wrapping within six entries.
- Show name, portrait, type and a compact Life / Stamina / Power summary for the highlighted character.
- A or Start confirms; B returns to title.
- Keep all six available from the start.

The existing selector image shows portraits and names. Adding the stat summary is a native UI task; it is not already present in that PNG.

### 16.4 Battle HUD

Extend the existing HP-only visual reference to three resources per fighter:

| Resource | Color | Meaning |
| --- | --- | --- |
| HP | Green; orange when low | Remaining Life |
| STA | Blue | Available Stamina |
| SP | Gold | Super Power charge |

Suggested native panel region: player at (4,4) size 94 × 38; CPU at (142,4) size 94 × 38.

Use compact labels and bars inside each panel. The center timer remains near the top middle.

- All bars read authoritative values.
- SP displays a clear READY state at 100.
- No blink may conceal whether stamina is empty.
- Use integer bar fill lengths.
- Do not show type effectiveness or a second hidden “power” stat.

### 16.5 Results and pause

Victory: selected human Pokémon, its name, celebratory effect, Rematch and Change Pokémon.

Defeat: selected human Pokémon with a subdued treatment, Try Again and Change Pokémon.

Draw: reuse the result layout with neutral DRAW text and Rematch / Change Pokémon. It must not call the human a loser.

Pause: Resume, Rematch and Change Pokémon. Resume freezes and restores the same battle state after a 60-display-tick resume countdown. Rematch creates a fresh match.

## 17. Asset conversion and missing artwork

### 17.1 Current pack status

- Six main roster PNGs: one static battle pose per character.
- Optional generated Charmander/Bulbasaur sheets: eight poses each, in 96 × 96 cells.
- Animated title reference: 48 full-screen frames.
- Main sprite anchor: (32,58).
- Clean arena: 240 × 160.
- Buttons and some foreground artwork have sizes that are not legal single GBA sprite sizes.

Do not claim that six complete fighting animation sets already exist.

### 17.2 Native graphics rules

Butano's documented import route uses indexed, uncompressed BMP graphics and matching import metadata. Assets are embedded in the ROM during the build. PNGs, browser atlas JSON and GIFs are source/reference formats here. Use palette index zero for transparency and validate 16/256-color imports. [S8]

For regular tiled backgrounds, pad the 240 × 160 view into a supported 256 × 256 background surface. Preserve the intended visible top-left region rather than stretching the artwork. [S8]

A native object is at most 64 × 64. Do not import a 96 × 96 bonus cell or the 208 × 92 title illustration as a single object. Butano does not automatically provide a metasprite abstraction. Reauthor/crop fighter art to 64 × 64, separate its effects, or deliberately manage multiple legal objects. The title illustration should use a background layer. [S6][S7]

### 17.3 Palette strategy

**Design decision:**

- Fighters and small projectile/effect sprites use separate 16-entry object palettes.
- Use a common 256-entry background palette for simultaneously displayed 8-bit arena/HUD layers.
- Reserve background color entries for readable UI colors during conversion.
- Do not let independent quantization assign different meanings to the same color indexes across simultaneously visible 8-bit background layers.
- Reduce colors and tile diversity before exceeding memory budgets.
- Use the pinned GBA Studio importer when it produces valid artifacts; validate the emitted BMP and metadata.

The requirement to coordinate shared 8-bit palettes follows Butano's palette behavior. The exact allocations above are this game's proposed implementation strategy. [S7]

### 17.4 Conversion pipeline

1. Keep original PNG/GIF/JSON files unchanged in assets/source.
2. Read the current supplied manifests for frame dimensions and anchors.
3. Create deterministic palette-constrained native derivatives.
4. Snap colors to the native display's color precision.
5. Ensure palette entry zero is transparent.
6. Pad backgrounds; do not resample 240 × 160 into 256 × 256.
7. Repack fighter frames into legal 64 × 64 cells.
8. Separate baked decorative projectile effects from body art when reusing bonus attack poses.
9. Register native assets through the verified editor/build pipeline.
10. Inspect the generated native image, metadata, tile counts and palette assignments.
11. Build and view an asset-check ROM.
12. Fail the conversion step on illegal sizes, missing frames, incorrect anchors or palette overflow.

Native import metadata must come from the installed toolchain's actual supported schema. Do not copy the browser's TexturePacker atlas JSON into the native importer.

### 17.5 Required final fighter animation inventory

Target 17 meaningful frames per character:

| Animation | Frames |
| --- | ---: |
| Idle | 2 |
| Run | 4 |
| Melee | 2 |
| Fire | 2 |
| Block | 1 |
| Squeeze | 1 |
| Jump | 1 |
| Hit reaction | 1 |
| KO | 1 |
| Super startup/release | 2 |
| Total | 17 |

A four-column, five-row native sheet can hold these 17 cells, with three explicitly unused cells.

For the first playable prototype, the supplied static sprites may use clear code-driven movement, flash, squash and recoil. Record this as provisional art. Final acceptance requires readable block and squeeze presentation for all six, with no invisible hurtbox changes.

Do not wait for every final animation before implementing and testing collision.

### 17.6 Native UI adaptation

The existing PNG layouts are visual references. Build labels, focus states and buttons as native UI elements.

Where tiled UI needs grid alignment, move/pad components deliberately to the eight-pixel grid and record native rectangles in native_ui_layout.json. Keep the hierarchy and screen flow. Do not stretch a nonstandard-size button into one hardware object.

## 18. Native performance and memory plan

These are target budgets, not statements about current measured usage.

| Resource | Target |
| --- | --- |
| Simulation/render cadence | One update per native display frame; nominal 60 |
| Combat fighters | 2 |
| Total live hardware objects | Prefer <= 64; hardware limit is 128 |
| Cosmetic effects | <= 8 |
| BG VRAM, tiled battle layout | <= 50 KiB used; leave at least 14 KiB margin |
| OBJ VRAM | <= 24 KiB used; leave at least 8 KiB margin |
| Combat state and collision buffers | <= 16 KiB |
| Engine + application EWRAM target | <= 192 KiB |
| Hot code/stack IWRAM target | <= 24 KiB |
| Initial ROM size target | <= 8 MiB |
| Combat heap allocation per tick | 0 |
| Worst-case frame workload | Must complete within a display frame |

GBA has 256 KiB external work RAM, 32 KiB internal work RAM and 96 KiB total VRAM; tiled layouts divide graphics storage between backgrounds and objects. Use actual linker and graphics reports rather than treating the proposed budgets as measured results. [S9][S10]

An uncompressed 64 × 64, four-bit frame is 2,048 bytes. Seventeen frames for each of two fighters are 69,632 bytes, larger than object VRAM. Store the animation library in ROM and upload/cache only the needed subset.

Do not preload all six complete animation sets into object VRAM.

Prefer scene-entry asset allocation and bounded frame caches. Release graphics handles when leaving a scene. Check repeated rematches and selection changes for leaked palettes, objects or tile allocations.

Use one simple arena, bounded particles and a compact HUD before adding parallax, large affine sprites or more effects.

The title should animate layers rather than upload 48 complete full-screen native frames during play.

## 19. Canonical initial balance configuration

Extract the following JSON into config/balance.json. Tables above are its readable specification. The validator must reject disagreement.

~~~json
{
  "ruleset_version": "0.2.0",
  "nominal_tick_hz": 60,
  "match": {
    "round_ticks": 3600,
    "countdown_display_ticks": 180,
    "resume_countdown_display_ticks": 60,
    "result_delay_display_ticks": 60,
    "cpu_mirror_matches": false
  },
  "world": {
    "width": 240,
    "height": 160,
    "floor_y": 120,
    "min_fighter_x": 24,
    "max_fighter_x": 216,
    "player_spawn_x": 64,
    "cpu_spawn_x": 176
  },
  "resources": {
    "stamina_subunits_per_point": 60,
    "stamina_regen_delay_ticks": 30,
    "block_drain_subunits_per_tick": 12,
    "block_enter_min_stamina": 8,
    "super_power_max": 100,
    "super_power_start": 0,
    "melee_clean_hit_power": 18,
    "fire_clean_hit_power": 22,
    "guard_cost_numerator": 5,
    "guard_cost_denominator": 4
  },
  "motion": {
    "gravity_q8": 64,
    "jump_velocity_q8": -1088,
    "max_fall_velocity_q8": 1280,
    "air_speed_numerator": 3,
    "air_speed_denominator": 4,
    "normal_knockback_q8": 512,
    "super_knockback_q8": 768,
    "knockback_decay_q8": 64
  },
  "input": {
    "buffer_ticks": 3,
    "press_actions_repeat_while_held": false
  },
  "collision": {
    "standing_hurtbox": [-7, -28, 7, 0],
    "squeezed_hurtbox": [-8, -10, 8, 0],
    "standing_pushbox": [-8, -24, 8, 0],
    "squeezed_pushbox": [-8, -10, 8, 0],
    "right_melee_hitbox": [5, -24, 21, 0],
    "fire_hitbox": [-3, -3, 3, 3],
    "super_hitbox": [-6, -6, 6, 6],
    "fire_spawn_forward_px": 12,
    "super_spawn_forward_px": 14,
    "projectile_spawn_above_feet_px": 18
  },
  "actions": {
    "jump": {
      "stamina_cost": 18
    },
    "melee": {
      "stamina_cost": 8
    },
    "fire": {
      "stamina_cost": 20,
      "lifetime_ticks": 60
    },
    "squeeze": {
      "stamina_cost": 12,
      "startup_ticks": 3,
      "active_ticks": 24,
      "recovery_ticks": 9,
      "cooldown_after_end_ticks": 24
    },
    "super": {
      "stamina_cost": 25,
      "power_cost": 100,
      "startup_ticks": 30,
      "active_ticks": 1,
      "recovery_ticks": 24,
      "projectile_speed_q8": 1280,
      "lifetime_ticks": 60,
      "blockable": false,
      "tracking": false,
      "power_gain": 0
    }
  },
  "reactions": {
    "normal_hitstun_ticks": 12,
    "super_hitstun_ticks": 24,
    "blockstun_ticks": 6,
    "guard_break_ticks": 45,
    "post_hit_invulnerability_ticks": 12,
    "normal_hitstop_display_ticks": 4,
    "super_hitstop_display_ticks": 8,
    "block_hitstop_display_ticks": 2
  },
  "capacities": {
    "fighters": 2,
    "melee_instances": 2,
    "normal_projectiles": 2,
    "super_projectiles": 2,
    "contact_candidates": 16,
    "combat_events": 64,
    "cpu_observation_history": 32,
    "cosmetic_effects": 8
  },
  "characters": [
    {
      "id": "pikachu",
      "name": "Pikachu",
      "type": "Electric",
      "max_hp": 95,
      "max_stamina": 105,
      "stamina_regen_per_second": 26,
      "walk_speed_q8": 384,
      "melee": {
        "name": "Quick Jab",
        "damage": 8,
        "startup_ticks": 4,
        "active_ticks": 3,
        "recovery_ticks": 11
      },
      "fire": {
        "name": "Spark Shot",
        "damage": 12,
        "startup_ticks": 8,
        "active_ticks": 1,
        "recovery_ticks": 17,
        "speed_q8": 896
      },
      "super": {
        "name": "Volt Burst",
        "damage": 32
      }
    },
    {
      "id": "charmander",
      "name": "Charmander",
      "type": "Fire",
      "max_hp": 100,
      "max_stamina": 90,
      "stamina_regen_per_second": 24,
      "walk_speed_q8": 307,
      "melee": {
        "name": "Claw Swipe",
        "damage": 10,
        "startup_ticks": 6,
        "active_ticks": 3,
        "recovery_ticks": 14
      },
      "fire": {
        "name": "Ember Shot",
        "damage": 14,
        "startup_ticks": 10,
        "active_ticks": 1,
        "recovery_ticks": 21,
        "speed_q8": 768
      },
      "super": {
        "name": "Inferno Wave",
        "damage": 36
      }
    },
    {
      "id": "squirtle",
      "name": "Squirtle",
      "type": "Water",
      "max_hp": 115,
      "max_stamina": 110,
      "stamina_regen_per_second": 22,
      "walk_speed_q8": 269,
      "melee": {
        "name": "Shell Bump",
        "damage": 7,
        "startup_ticks": 6,
        "active_ticks": 3,
        "recovery_ticks": 13
      },
      "fire": {
        "name": "Water Shot",
        "damage": 11,
        "startup_ticks": 10,
        "active_ticks": 1,
        "recovery_ticks": 19,
        "speed_q8": 704
      },
      "super": {
        "name": "Hydro Wave",
        "damage": 30
      }
    },
    {
      "id": "bulbasaur",
      "name": "Bulbasaur",
      "type": "Grass/Poison",
      "max_hp": 120,
      "max_stamina": 100,
      "stamina_regen_per_second": 20,
      "walk_speed_q8": 243,
      "melee": {
        "name": "Vine Jab",
        "damage": 9,
        "startup_ticks": 7,
        "active_ticks": 3,
        "recovery_ticks": 15
      },
      "fire": {
        "name": "Leaf Shot",
        "damage": 12,
        "startup_ticks": 11,
        "active_ticks": 1,
        "recovery_ticks": 20,
        "speed_q8": 832
      },
      "super": {
        "name": "Solar Pulse",
        "damage": 34
      }
    },
    {
      "id": "eevee",
      "name": "Eevee",
      "type": "Normal",
      "max_hp": 100,
      "max_stamina": 115,
      "stamina_regen_per_second": 28,
      "walk_speed_q8": 358,
      "melee": {
        "name": "Tackle",
        "damage": 7,
        "startup_ticks": 4,
        "active_ticks": 3,
        "recovery_ticks": 10
      },
      "fire": {
        "name": "Swift Star",
        "damage": 10,
        "startup_ticks": 8,
        "active_ticks": 1,
        "recovery_ticks": 17,
        "speed_q8": 832
      },
      "super": {
        "name": "Swift Nova",
        "damage": 31
      }
    },
    {
      "id": "meowth",
      "name": "Meowth",
      "type": "Normal",
      "max_hp": 90,
      "max_stamina": 100,
      "stamina_regen_per_second": 25,
      "walk_speed_q8": 397,
      "melee": {
        "name": "Scratch",
        "damage": 8,
        "startup_ticks": 3,
        "active_ticks": 3,
        "recovery_ticks": 10
      },
      "fire": {
        "name": "Coin Shot",
        "damage": 11,
        "startup_ticks": 8,
        "active_ticks": 1,
        "recovery_ticks": 18,
        "speed_q8": 768
      },
      "super": {
        "name": "Gold Rush",
        "damage": 33
      }
    }
  ]
}
~~~

### 19.1 Configuration validation

Reject the configuration if:

- There are not exactly six unique character ids.
- A resource maximum is nonpositive.
- A cost is negative.
- A timing is negative or an attack has no active/spawn tick.
- A required super is blockable or tracking.
- A squeezed hurtbox overlaps the grounded super band.
- The configured jump never produces feet_y <= 96.
- A projectile spawn height or speed makes the specified attack impossible to evade as required.
- A character's projectile or melee timing contradicts its table.
- A resource value cannot fit its selected storage type.
- A referenced required native animation or graphics item is absent in a release build.

Compile configuration into immutable C++ structures. Do not parse JSON on the GBA during a match.

## 20. Verification and acceptance tests

### 20.1 Engine/build tests

| ID | Test | Expected result |
| --- | --- | --- |
| ENG-01 | Clean GBA Studio project build | Produces a nonempty .gba and a successful build log |
| ENG-02 | Native boot in mGBA | Shows the game title, without an assertion |
| ENG-03 | Project C++ integration | The project controller owns input and updates |
| ENG-04 | Delete build outputs and rebuild | No authored changes are lost |
| ENG-05 | Editor open/save after integration | Project and asset records remain valid |

### 20.2 Resources and actions

| ID | Test | Expected result |
| --- | --- | --- |
| RES-01 | Try a 20-point fire at less than 20 stamina | No action, projectile or partial cost |
| RES-02 | Regenerate for 60 eligible ticks | Exact character regen amount, clamped at max |
| RES-03 | Hold block for 60 simulation ticks | 12-point holding cost, excluding impact costs |
| RES-04 | Attack misses | No Super Power |
| RES-05 | Attack is blocked | No HP loss and no Super Power |
| RES-06 | One clean melee | Listed damage and +18 attacker SP |
| RES-07 | One clean fire | Listed damage and +22 attacker SP |
| RES-08 | Repeated overlap of the same melee active phase | Only one damage and meter award |
| RES-09 | SP would exceed 100 | Clamped to 100 |
| RES-10 | Super is accepted | Exactly 100 SP and 25 stamina spent once |
| RES-11 | Super hits | No Super Power gain |
| RES-12 | Full projectile reservation pool | No action or cost |
| RES-13 | Rematch | Full HP/STA; SP zero; no old projectiles or timers |

### 20.3 Super defense matrix

Run every test for every character and both facing directions.

| ID | Defender state at actual contact | Expected |
| --- | --- | --- |
| SUP-01 | Grounded neutral | One full super hit |
| SUP-02 | Correct facing block, full stamina | One full super hit; no ordinary block event |
| SUP-03 | Squeeze startup | Hit if rectangles overlap |
| SUP-04 | Squeeze active phase | No hit at the required ground band |
| SUP-05 | Squeeze recovery | Hit if rectangles overlap |
| SUP-06 | Jumping with feet_y <= 96 | No hit |
| SUP-07 | Jump started too late | Hit if rectangles overlap |
| SUP-08 | Wave misses and exits | No damage or power |
| SUP-09 | Wave overlaps across multiple ticks | At most one accepted hit |
| SUP-10 | Owner interrupted before spawn | No wave; spent resources stay spent |
| SUP-11 | Owner interrupted after spawn | Existing wave continues |
| SUP-12 | Super meets an ordinary shot | Ordinary shot destroyed; super continues |

Do not accept a test that merely checks an enum such as is_jumping. Assert actual rectangles and HP.

### 20.4 Collision and timing

- Edge-touching rectangles do not hit.
- Mirrored hitboxes produce the same result from both sides.
- A fast projectile cannot tunnel through a moving target.
- Two opposing hits in the same tick can trade.
- Simultaneous lethal hits produce Draw.
- An ordinary block without sufficient stamina creates guard break and full HP damage.
- Squeezing remains vulnerable to a low-reaching melee hit.
- Action ages advance once per simulation tick.
- A frozen spawn tick never creates multiple projectiles.
- Pause freezes HP, STA, SP, projectiles, RNG, action ages and the clock.
- Hitstop freezes simulation but does not lose the one buffered input.
- KO takes precedence over timeout on the same completed tick.
- Timeout compares normalized HP rather than raw remaining HP.

### 20.5 Screen tests

- Start opens selection.
- All six characters can be selected by native buttons.
- The selected id controls fighter stats and graphics.
- Result artwork/name follows the chosen Pokémon.
- Victory and defeat correspond to actual resolved outcomes.
- Rematch preserves both character choices but resets the match.
- Change Pokémon returns to selection.
- HP, STA and SP bars show authoritative values.
- A full SP meter is clearly distinguishable from 99.
- Draw uses neutral text.
- Pause/resume does not accidentally spend a buffered super.

### 20.6 Performance and balance

- Exercise two fighters, both projectile types, hits, HUD updates, effects and audio together.
- Measure the actual worst-case frame cost and memory.
- Run at least 20 consecutive rematches/selection changes without increasing live allocations.
- Replay identical initial state, seed and InputFrames to produce identical state checksums.
- Run all 36 ordered character pairs in host simulation, including debug mirror matches, over 100 fixed seeds per pair.
- Report win rates, timeout/draw rates, damage, blocks, guard breaks, squeeze/jump super evasions and super use.
- Treat a 35–65% non-mirror win-rate band under equal CPU policy as a starting investigation threshold, not proof of balance.
- Manually test whether super telegraphs and movement are readable on a small display.

Do not change damage for the human and CPU separately to hide a balance problem.

## 21. Implementation milestones

Each milestone must leave a reproducible build or independently runnable host tests. Do not implement the entire game before the first native ROM is verified.

### Milestone 0 — Native integration proof

**Work:** identify installed GBA Studio; pin its source/toolchain; create a blank .gbasproj; build and boot it; establish project C++ and the narrow bootstrap route; record actual commands.

**Exit:** clean native ROM shows a test fighter responding to GBA input. Engine report names the real paths, toolchain, version and patch.

**DeepSeek instruction:** “Prove the native project and project-code entry path. Do not invent plugin APIs or CLI flags. Build and boot the smallest ROM before adding combat.”

### Milestone 1 — Configuration and pure simulation

**Work:** extract Section 19; implement validation, fixed-point types, fighter state, inputs, resource math, action phases and host tests.

**Exit:** deterministic host tests pass for costs, phase boundaries, regeneration and input arbitration.

**DeepSeek instruction:** “Implement the data-driven CombatCore without graphics dependencies. Use the exact initial stats and integer resource arithmetic.”

### Milestone 2 — Playable two-character vertical slice

**Work:** Pikachu and Charmander, one arena, movement, jump, melee, fire, guard, squeeze, Super Power and super attacks. Add a stationary/practice CPU mode for testing.

**Exit:** native play demonstrates the required super counter matrix. Guard never stops a super, while correct jump/squeeze geometry does.

**DeepSeek instruction:** “Finish the complete combat loop for two fighters. Validate real hitboxes before adding more content.”

### Milestone 3 — Six-character roster and CPU

**Work:** all six definitions, species effects, selection data, shared geometry, difficulty and delayed CPU input.

**Exit:** all six can fight with their own values; CPU uses the same rules; all defense matrix cases pass for every character.

**DeepSeek instruction:** “Add the remaining roster through configuration, without character-specific collision exceptions.”

### Milestone 4 — Full native screen flow

**Work:** animated title, selector, countdown, battle HUD with three meters, pause, outcome delay, victory, defeat, draw and rematch.

**Exit:** a user can complete and restart a match using native controls only.

**DeepSeek instruction:** “Port the supplied screen references into native UI and wire them to actual match state. The HTML preview is a reference.”

### Milestone 5 — Native asset and animation pass

**Work:** deterministic indexed conversion; native-compatible frame packing; visible block/squeeze poses; title-layer animation; complete or clearly track missing final frames.

**Exit:** asset report has no invalid release assets; squeeze graphics match the lowered hurtbox; no palette corruption or duplicated projectile art.

**DeepSeek instruction:** “Preserve originals, generate native derivatives, and validate their palettes, anchors, dimensions and tile counts.”

### Milestone 6 — Profiling, sound and balance

**Work:** hit/block/jump/fire/super/result sounds; bounded effects; frame/memory reports; replay tests; matchup simulations; manual feel tuning.

**Exit:** worst-case native play fits frame/memory limits; changes are in the canonical configuration with test results.

**DeepSeek instruction:** “Measure before optimizing. Keep collision and resource rules stable while tuning the documented values.”

### Milestone 7 — Release package

**Work:** clean build, emulator playthrough, repeated-rematch check, exact controls, editable project, engine patch, native assets and reports.

**Exit deliverables:**

- PokemonBattleArena.gba.
- Editable .gbasproj and project source.
- config/balance.json and generated definitions.
- Source and native assets.
- Reproducible build wrapper and pinned dependencies.
- Engine/asset/test reports.
- Short release notes naming any remaining provisional animation.

**DeepSeek instruction:** “Package the native game and reproducible project. Report actual verification, and identify any untested hardware behavior.”

## 22. Copy-paste kickoff instruction for DeepSeek Harness

~~~text
Implement the game described in GBA_Pokemon_Fighter_Architecture_and_Plan.md.

Use the supplied pokemon_fighter_assets.zip as the source art and screen reference.
The target is a native Game Boy Advance .gba ROM built through GBA Studio.

First:
1. Read the complete specification, including the binding version 1.1 clarifications in Section 0.
2. Inspect the installed GBA Studio identity and version.
3. Inspect the existing workspace before creating or changing project files.
4. Create/preserve an editable .gbasproj.
5. Extract the canonical Section 19 JSON into config/balance.json.
6. Prove a clean native build and project-owned C++ entry path.
7. Record exact commands and engine integration in docs/ENGINE_CAPABILITY_REPORT.md.

The researched baseline is dackmin/gba-studio v0.5.0, built on Butano.
Project-local src/include support was observed in that source. A supported runtime
hook is not assumed. If necessary, implement the narrow, reproducible bootstrap
adapter described by the spec, keeping the game controller in project-owned code.

Then implement the milestones in order. Do not stop after writing a plan.
Use a deterministic fixed-tick CombatCore separated from rendering and CPU input.

Critical mechanics:
- Six characters have different HP, stamina, speed and damage.
- Life, Stamina and Super Power are separate resources.
- Melee and fire can be blocked when facing correctly and guard stamina permits.
- Clean ordinary hits charge Super Power; blocks and misses do not.
- A 100-point meter enables the super, costing 100 SP and 25 stamina.
- The super bypasses guard but uses a real traveling mid-height hitbox.
- Correctly timed jumping or active squeeze must avoid it for every character.
- Squeeze is a timed low-profile crouch, not general invulnerability.
- One attack instance cannot repeatedly damage the same target.
- Costs, phases, collision and outcome rules come from the specification.
- The CPU submits the same commands and obeys the same restrictions as the human.

Use native graphics conversion. The HTML, GIFs and browser atlas JSON are references,
not GBA runtime code. Do not import oversized artwork as one native object.

After each milestone, build or run its host checks and record actual results.
Do not claim native completion unless the .gba has been built and booted in an emulator.
Keep authored code outside generated output and preserve original source artwork.
~~~

## 23. Completion checklist

- [ ] Native GBA Studio project opens and clean-builds.
- [ ] ROM boots in mGBA.
- [ ] Animated title and Start action work.
- [ ] All six Pokémon are selectable.
- [ ] Per-character HP, stamina, speed and damage match the configuration.
- [ ] Movement, melee, fire, block, jump and squeeze work.
- [ ] Clean hits charge SP; blocked hits and misses do not.
- [ ] Super consumes the correct resources once.
- [ ] Blocking cannot stop a connected super.
- [ ] Proper jump and squeeze geometry avoid a super for all six.
- [ ] Melee can punish an inappropriate squeeze.
- [ ] HUD shows HP, STA and SP for both fighters.
- [ ] CPU cannot bypass the rules.
- [ ] Victory, defeat, draw, rematch and Change Pokémon work.
- [ ] Pause and hitstop preserve deterministic state.
- [ ] Native assets satisfy size, palette and memory constraints.
- [ ] Matchup and replay checks are recorded.
- [ ] Missing final art is explicitly identified.
- [ ] Editable sources, patch and build instructions accompany the ROM.

## 24. Sources and verification notes

Engine research was performed on 27 October 2026 and official documentation was rechecked on 28 October 2026. Recheck the installed version at implementation time. Source observations establish available integration seams, while the architecture and combat rules are the design proposed in this document.

- **[S1] GBA Studio official repository:** https://github.com/dackmin/gba-studio
- **[S2] Official release/download page:** https://dackmin.itch.io/gba-studio and https://github.com/dackmin/gba-studio/releases/tag/v0.5.0
- **[S3] v0.5.0 CLI build source:** https://github.com/dackmin/gba-studio/blob/v0.5.0/src/cli/commands/build.ts
- **[S4] v0.5.0 project build source:** https://github.com/dackmin/gba-studio/blob/v0.5.0/src/main/handles/build-project/index.ts
- **[S5] v0.5.0 runtime bootstrap and asset build source:** https://github.com/dackmin/gba-studio/blob/v0.5.0/public/templates/commons/src/main.cpp and https://github.com/dackmin/gba-studio/blob/v0.5.0/src/main/handles/build-project/assets.ts
- **[S6] Tonc graphics overview:** https://gbadev.net/tonc/video.html
- **[S7] Butano FAQ:** https://gvaliente.github.io/butano/faq.html
- **[S8] Butano asset import guide:** https://gvaliente.github.io/butano/import.html
- **[S9] Tonc hardware and memory:** https://gbadev.net/tonc/hardware.html
- **[S10] Tonc sprite/graphics reference:** https://gbadev.net/tonc/regobj.html
- **Butano sprite module reference:** https://gvaliente.github.io/butano/group__sprite.html

The GBA Studio source files above were retrieved directly from the v0.5.0 raw repository for inspection. The research also observed the then-current main tree at commit 27962d09ce5a43cecbe18867582ee9184d782e06; the release-specific observations in this document come from v0.5.0, not an assumption that main and the release are identical.

The supplied asset pack's existing CREDITS.md and SOURCES.json preserve the roster artwork provenance. The native implementation should retain those credits with its project.

