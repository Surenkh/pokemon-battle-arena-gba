# v0.4.0 — Combat Feel implementation contract

The update makes attacks and resource states easier to perceive through sound, impacts and input responsiveness. CombatCore remains authoritative. The game runs in the existing native GBA Studio 0.5.0 / Butano application; no browser, network or LLM runs in the ROM.

## Ownership and data flow

CombatCore accepts player/CPU inputs, resolves physics and contacts, changes resources, then exposes up to 32 CombatEvent records for that display invocation. Records identify kind, owner, target, position, super flag and source attack id. Separate events retain both sides of a simultaneous trade. Projectile spawn positions are copied before collision/KO cleanup, so a point-blank hit cannot erase its muzzle cue.

CombatFeel reads events and maintains bounded display timers, flash states and camera displacement. CombatAudio reads the same events and fighter startup ages to select sound banks. The renderer reads combat and presentation state. These consumers do not write gameplay fields, consume CPU randomness, or enter the combat checksum. Event sequence numbers prevent double dispatch during hitstop and the result delay. Contact resolution retains its separate 16-entry candidate array.

Event overflow traps rather than silently dropping records. The current fixed pools contain two fighters, two melee instances and four projectiles. Recalculate the event bound before expanding these pools or adding repeated contacts. The 12-slot decorative sprite pool may discard cosmetic requests without suppressing a hit.

## Timing and feedback

| Trigger | Presentation | Timing |
| --- | --- | --- |
| Ordinary clean hit | Attacker-themed impact sound, white tint, 1 px shake | Existing 4-display-frame hitstop; shake decays over 5 display frames |
| Super hit | Heavier themed impact, white tint, up to 3 px shake | Existing 8-display-frame hitstop; shake lasts up to 12 display frames |
| Successful block | Guard effect, shield pop, block sound, BLOCK label | Existing 2-display-frame hitstop; label lasts 12 display frames |
| Guard break | Distinct sound, GUARD BREAK, up to 2 px shake | Existing 45 simulation ticks of guard break; presentation also has a 30-display-frame cue |
| Unaffordable eligible input | Short error cue and NO STA | 18-display-frame label; sound throttled to 18 display frames per fighter |
| Low reserves | LOW STA below 20 stamina | Resource values unchanged |
| Full SP | Ready cue and R READY unless a higher-priority status applies | Super still requires 100 SP and 25 stamina |
| Super startup | Character-themed four-frame glyph and rising charge pulses | Existing 30 simulation ticks; pulse phases 0, 10 and 20 when a voice is available |
| Super release/travel | Dedicated release sound and fixed SUPER! JUMP OR SQUEEZE warning | Existing unblockable, nontracking projectile and geometry-based evasion |

Status priority is guard break, rejected stamina input, recent block, low stamina, then full SP. Presentation clocks run during hitstop while simulation and the round timer freeze. Pause freezes presentation lifetimes and stops all combat voices. Resuming uses the existing countdown; charging resumes its current phase without leaking an old voice.

## Camera and rendering

Deterministic decaying motion is capped at 3 horizontal pixels and 1 vertical pixel. Two HBlank scroll effects affect arena rows 44–141. Hardware scroll deltas are the negative of screen-space displacement; fighters, shadows, projectiles, shields, charging glyphs and particles share the same movement. Names, bars, timer and prompts remain fixed. Clamped offscreen edge columns prevent empty wrapping tiles from appearing during shake.

The scroll-effect handles are created on countdown frames 2 and 3; charging sprites are created on demand. This spreads resource allocation away from the first arena upload. Palette tint changes only when a fighter's flash state changes.

## Audio authoring and mixing

tools/generate_combat_audio.py creates original deterministic mono 16-bit PCM at 16,384 Hz using Python's standard library. Each fighter has shot, hit, charge and release assets; shared cues cover guard break, exhaustion and full SP. WAV hashes, sample counts and provenance are in combat_audio_assets.json. Content JSON imports register all 27 sounds through GBA Studio's native pipeline.

Pikachu uses gated electrical tones; Charmander filtered noise; Squirtle a water-like trill; Bulbasaur airy noise; Eevee harmonic stars; Meowth three short claw-like bursts. These are newly synthesized arcade effects rather than sampled Pokémon recordings.

The dispatcher owns at most **two total combat voices**. It selects the two highest-ranked events per invocation: guard break, super release, super hit, ordinary hit, block, shot, full SP, exhaustion, swing, jump. Selected events replace older voices in a bounded rotation. Charge pulses fill free slots at lower volume and may be omitted during a busy impact. Release sounds retain priority. Interrupted/released supers stop their owned charging sound. Reset/menu transitions stop the dispatcher's handles while preserving menu audio.

The two-voice policy limits GBA mixer work. Dense trades can shorten or omit lower-priority audio; visual and state feedback still appears. There is no new background music.

## Input buffering

An action press already receives attempts at T, T+1 and T+2. This update additionally accepts Up while descending if touchdown will occur within the remaining two ticks. Eligibility predicts only existing gravity and fall-speed clamping. The jump executes after grounding, once, if affordable and otherwise legal. Early airborne presses and held Up do not produce repeated jumps. Recovery buffering, resource rejection, action priority and pause clearing retain their existing rules.

This intentional input change advances the ruleset to 0.4.0. Damage, stamina costs/regeneration, hitstop, attack timings, geometry and CPU decision code retain their prior values.

## Validation and limits

Host tests cover source-correct dispatch, trades, guard depletion, exact hitstop, buffer boundaries, pause, deduplication and checksum isolation. Asset tests validate actual WAV data and imports. Native mGBA tests use physical inputs and read-only telemetry, record actual stereo PCM, exercise all six banks, pause during charging, compare fixed HUD pixels, and verify a common translation of scenery and frozen actors during shake.

See [the test report](COMBAT_FEEL_TEST_REPORT.md) for measured results and clean-build evidence. Emulator profiling does not certify physical hardware or establish human matchup balance. Spare frame capacity is limited; profile before adding voices, background layers or large effects.
