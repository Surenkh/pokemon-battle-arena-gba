# v0.7.0 — Sound & Poké Ball Recall

This update adds atmosphere around fighting: original menu music and cues, separate audio controls, and a short recall scene for the defeated fighter. It is an additional improvement outside the five-direction gameplay roadmap. Combat damage, movement, stamina, CPU behavior and outcomes retain v0.6 mechanics.

## Audio experience

The title plays **Leaflight Circuit**, an original 34.29-second loop at 112 BPM. It continues through the selector, practice setup, fighter guides and menu help. Music fades out before battle; fighting keeps the established elemental combat mix. Results use a short win, loss or draw stinger. No existing Pokémon song or external recording was used.

Fourteen new cues cover navigation, confirmation, back, pause/resume, countdown/fight, knockout, ball opening, absorption, seal and the three results. Navigation is throttled so held/rapid keys do not build up voices. Cue selection is owned by application transitions, not by CombatCore events. Combat sounds retain their separate event dispatcher.

L on the title, selector, practice setup, result or pause opens Audio. The pause menu also has an AUDIO entry. Up/Down selects Music or Sound FX; A/Left/Right toggles it; B/Start/L returns. Sound FX controls menus, combat, recall and results together. Music and effects can each be enabled while the other is disabled. Both default to On and remain selected through rematches, practice and menu transitions during the current play session. They reset at power-on; this release adds no save format.

The default mix uses bounded source amplitudes and moderate fixed playback gains. Music peaks at 84/256 gain with a 28-frame fade-in; an active menu cue ducks it toward 64/256. Music fades out by six gain units per display frame. Music Off stops it immediately. Navigation uses 104/256, other presentation cues 112/256, and the absorption sweep 96/256. Existing combat cues remain at their established 0.18–0.40 gains. Device/emulator output volume remains independent.

`PresentationAudio` owns one effect handle. `CombatAudio` owns at most two. The original tracker module uses three active music voices, so menu music plus a cue uses four software voices; battle normally uses at most three effect voices and no background song. Muting stops the affected handles and discards stale combat events instead of replaying them on re-enable.

## Compact authoring and native integration

`tools/generate_presentation_audio.py` composes the module and synthesizes the 14 mono 16-bit PCM cues at 16,384 Hz. The three looped wavetables, four tracker patterns, melody and chord sequence are authored in that script. The module is only 5,372 bytes, rather than storing a half-minute PCM recording. Source amplitudes have attack/release ramps and a 0.65 peak bound. GBA Studio imports the MOD and WAV files through ordinary music/sound content records; Butano/Maxmod performs playback. No runtime audio streaming service or network is involved.

Music generation and conversion are deterministic. `docs/presentation_audio_assets.json` records provenance, durations, sample counts and hashes. The new script is part of `npm run audio`; `npm run test:audio` validates both combat and presentation assets.

## Recall sequence

A normal win or loss starts a 156-display-frame sequence (about 2.6 seconds). It selects the loser from the resolved outcome, including a timeout decided by remaining HP percentage. A draw selects no loser. Practice and draws retain the quick 60-frame transition; practice remains useful for repeated drills.

| Frames | Visual |
| --- | --- |
| 0–23 | Existing knockout pose and final impact settle; winner begins its victory animation |
| 24–43 | Closed ball arrives in a short arc above the defeated fighter |
| 44–55 | Ball opens; a pink/white recall beam reaches the fighter |
| 56–91 | Defeated sprite tints and shrinks toward the ball; its shadow contracts |
| 92–103 | Fighter is hidden; ball closes |
| 104–127 | Closed ball falls to the ground |
| 128–155 | Small shake, seal chirp and glints; transition to results |

A or Start can advance to results from frame 112, after absorption and closing. Earlier attack presses cannot dismiss the sequence. The winner stays visible throughout. A defeat result shows the closed ball rather than immediately bringing the recalled player back; a victory result shows the winning player as before.

`RoundPresentation` owns this display clock and exposes bounded positions, frame indices, visibility and Q8 transforms. It reads the final core once at begin and never changes HP, resources, fighter positions, outcome, simulation tick or random state. Renderer transforms and palette fades affect sprites only. Pauses/options continue to preserve combat state. No capture probability, collection system or new attack mechanic is introduced.

`tools/round_fx.cjs` authors native pixel artwork for four ball poses, a beam and four seal-glint frames. The ball uses a 32×32 cell, beam 32×64 and glints 64×64; all are indexed 4bpp. The renderer reuses the actual defeated Pokémon sprite and its existing faint/down animation, so every fighter gets the same anchored recall behavior. Three dedicated optional sprites cover the ball, beam and glints. Final combat decorations are cleared at frame 24, and all palette transforms/handles are reset when leaving the scene.

## UI and performance decisions

A new audio screen appends screen ID 11, preserving prior telemetry IDs and controls. The pause menu appends Audio after Controls, preserving the existing Change Pokémon position. Six selector portraits are uploaded two per frame to keep the music mixer and graphic transfers within budget. No artificial wait is added to selection input.

Presentation and combat remain separate. The new `pkmn_presentation_debug[32]` reports audio settings/buses, recall phases and actual sprite visibility for native tests. Tests use physical buttons and read-only telemetry, never gameplay memory writes. The release workflow runs the presentation host/asset checks and requires a ROM-bound passed native presentation report.

## Tradeoffs and limits

The tune uses three inexpensive tracker voices and original simple waveforms, preserving ROM space and frame budget. The one-voice presentation bus can replace a low-priority menu sound with a new cue. Settings last for the current run rather than creating an untested cartridge save contract. The recall introduces a short normal-round delay, with a skip after the fighter is safely inside; practice keeps its rapid reset. Audio level preferences depend on the user's device, so the mix is measured and bounded rather than claiming a universal perceived loudness.

See AUDIO_AND_RECALL_TEST_REPORT.md for measured results. Physical GBA hardware, non-Linux rebuilding and human matchup balance remain unverified. Arena life and arcade progression remain future roadmap work.
