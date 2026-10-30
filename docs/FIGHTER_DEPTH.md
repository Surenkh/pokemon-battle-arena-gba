# v0.5.0 — Fighter Depth and Practice

This release adds character-specific decisions to the existing deterministic GBA Studio / Butano runtime. The six fighters retain their HP, base damage, normal stamina costs, movement speeds, jump trajectory and super evasion rules. Character styles, a shorter resource recovery delay and bounded follow-ups advance the combat ruleset to 0.5.0.

## Character configuration

config/balance.json remains the numerical source of truth. tools/generate_config.py validates the new property ranges and guide text, then emits pkmn_character_defs.h. No separate hand-edited character table exists.

| Fighter | Trait | Implemented values and limitations |
| --- | --- | --- |
| Pikachu | Quick Step | 128 Q8 units of forward velocity during each grounded melee startup tick: four half-pixel steps. Air attacks retain normal captured momentum. |
| Charmander | Ember Pressure | Ordinary fire applies 125% of the base guard impact cost. Projectile damage and recovery retain their previous values. |
| Squirtle | Shell Control | Guard impacts cost 80% of their ordinary value. Clean ordinary water hits use 768 Q8 knockback instead of 512; other hit reactions and super knockback retain their prior values. |
| Bulbasaur | Vine Reach | Melee forward reach grows from 21 to 32 px, mirrored on the left. A leaf/vine cue extends ahead of the body during startup/active frames. Its existing startup and recovery remain commitments. |
| Eevee | Adaptable | A confirmed melee can follow with A or B. Second melee requires close spacing after knockback. No third cancel is available. |
| Meowth | Punish | +3 melee damage against an opponent in melee, fire or super recovery. No bonus against neutral, startup, active frames, guard or stun. Ordinary fire and supers receive no bonus. |

Guard costs first round damage × 5/4 up to whole stamina points, then apply attacker fire pressure and defender efficiency together, rounded up once. For a 14-damage Charmander projectile this costs 23 guard points against ordinary guard or 18 against Squirtle. An 8-damage ordinary non-Charmander impact costs Squirtle 8 guard points. Exact affordability still blocks without HP loss and then breaks guard at zero stamina; an unaffordable hit deals full damage. Supers bypass guard completely.

The extended vine cue reuses Bulbasaur's authored effect palette/sheet. There are no new projectile slots, random critical hits, damage-over-time statuses or type multipliers.

## Bounded follow-up rules

1. A melee must inflict a clean hit. A geometric miss, ordinary block or invulnerability rejection does not confirm it. A simultaneous trade can record a hit, but hitstun prevents the source from following immediately.
2. During the final six ticks before that melee ends, a legal B press may cancel recovery into ordinary fire. Eevee may also use A. Grounding, action resources and the ordinary projectile slot are checked normally.
3. The follow-up retains its full startup and normal cost: 20 stamina for fire or 8 for melee. Its earliest possible contact must be no earlier than original hit tick + 12 hitstun ticks + 2 escape ticks. Since reactions begin on the next tick, the defender has at least one active simulation tick outside hitstun before that earliest contact. Normal post-hit protection also remains in effect. This is a blockable follow-up, not an inescapable combo.
4. Each sequence permits one cancel. The follow-up carries chain_depth=1 and cannot grant another cancel. The projectile retains that source flag even if its owner starts another action before it arrives.
5. The existing three-tick input buffer still governs queued presses. Held input does not repeat. CPU decisions may queue a follow-up within the same two-tick lookahead window using their own known recovery state; they receive no extra acceptance window or current opponent input.

FOLLOW B and FOLLOW A/B show actual current eligibility. The latter appears only when Eevee's melee cancel is legal. PUNISH marks a clean melee recovery hit; Meowth additionally receives its configured damage bonus. Source event IDs and damage values are retained for delayed projectile contacts and practice feedback.

## Stamina rhythm and CPU integration

The regeneration delay falls from 30 to 24 active ticks. Regeneration still requires neutral action, respects the maximum and uses the existing per-character rates. Action costs and the resources needed for jump, squeeze, guard and super are unchanged.

The CPU begins resource recovery below 12 stamina and resumes ordinary offense at 32. It holds ground while recovering, while retaining its ordinary defensive reactions. A first retreating prototype caused excessive passive timeouts and was discarded. A full-SP CPU that cannot afford its super retains its existing recovery movement. Melee decisions use the fighter's actual reach, and legal hit-confirm follow-ups use the ordinary shared queue. Easy/Normal/Hard observation delays and one-roll super-evasion rules are unchanged.

The balance probe preserves baseline and candidate evidence across 2,160 seeded CPU-versus-CPU rounds each, with all ordered matchups and three difficulties. It measures resource availability, duration, timeout, attacks and super starts/releases. These are regression and tuning signals. Shared CPU policies and starting positions can favor a style; human matchup balance remains unverified.

## Practice flow

B on the title opens practice setup. Choose the player from the six cards. Select cycles card selection, dummy behavior and rival selection without changing the chosen player; D-pad navigation is also available. Behaviors are IDLE, BLOCK and ATTACK. R opens the fighter guide from either selector. Practice excludes mirror opponents because character palettes are shared by the renderer.

Practice runs the same CombatCore with training=true. Only timeout decrement/result-by-time is disabled; damage, stamina, input buffering, collisions, hitstun and KO detection are shared. Both fighters start with full SP for super drills. The dummy produces ordinary button inputs; it does not mutate positions, HP, costs or collision results.

The HUD shows last damage, linked damaging hit count and best sequence. LAST is resolved attack damage, including a punish bonus, before remaining HP is clamped at KO. A blocked or missed follow-up does not count as a second damaging hit. Select resets arena/resources/statistics through countdown. A KO automatically starts another practice round after the normal result delay, retaining statistics. Start pauses simulation, sound and statistics; Change returns to practice setup. Exiting to an ordinary match restores earned SP and a running timer.

PracticeSession owns its UI statistics and event deduplication outside gameplay checksums. No save data or persistence is added. Counters saturate at 9,999. Attack behavior is a predictable drill policy rather than a new difficulty level.

## Native rendering and performance

The arena still uses one background and two masked HBlank scroll effects. Practice adds a static board inside the same background; the board's rows remain outside camera movement. Vine cues reuse the two existing optional charging sprites. The decorative effect pool remains 12, the contact array 16 and the event array 32. Combat audio retains its two-voice cap.

HUD names and practice statistics now have separate cached text pools. Names are loaded after the initial arena and scroll-effect setup; practice text waits until the fight is established. Dynamic status/timer updates no longer recreate those unchanged strings. Countdown keys use the displayed rounded-up value. Fighter-guide text is loaded one frame after its background. These changes remove measured arena/guide transition spikes while reducing the total reserved text-sprite capacity from 96 to 88.

## Verification and remaining work

See [FIGHTER_DEPTH_TEST_REPORT.md](FIGHTER_DEPTH_TEST_REPORT.md) for actual host/native results, balance comparisons, clean-build evidence and ROM checksum. The depth host suite tests both fighter sides, guard thresholds, strict reach edges, recovery boundaries, blocked/missed/traded confirms, legal defensive gaps, queued input and practice ownership. Native tests use actual buttons and read-only telemetry, including real damage, guard costs, pause/reset, all dummy modes, completed follow-ups, a timed super dodge and automatic practice restarts.

Physical hardware and non-Linux building remain unverified. Additional human playtesting should focus on Squirtle's defensive value, Pikachu pressure, interruptible supers, follow-up spacing and stamina recovery. More complex AI variation, arenas, campaign, music, multiplayer and progression remain later roadmap work.
