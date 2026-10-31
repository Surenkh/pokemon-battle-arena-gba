# v0.7.0 audio and recall — test evidence

Tested on 31 October 2026 using pinned GBA Studio 0.5.0, devkitARM 16.1.0 on Linux x86-64, and mGBA 0.10.2. Native scripts use physical buttons, read-only telemetry, actual framebuffer captures and stereo PCM recording. No gameplay memory is written.

## Host and assets

Six host suites pass: combat 5,990,944 checks across 720 seeded matches/all 36 ordered matchups; movement 18,904; feedback 139,443; fighter depth 23,621; CPU 1,221,407 across 144 replayed games; presentation 362,739. Total: **7,757,058 passing host checks**. Counts depend on seeded game duration and are not a game-quality score.

The new presentation suite covers every fighter pairing, both stage edges, wins/losses/draws, practice, timeout recall, phase bounds, monotonic shrinking, visibility, skip eligibility and unchanged combat checksums. Existing mechanics and CPU code retain their prior behavior.

Asset checks pass for 408 character frames, 30 elemental sheets/120 frames and the 27 previously synthesized combat effects. A further **2,225 checks** validate fourteen new PCM cues, original tracker structure/wavetables/three used channels, hashes, source bounds, ramps and three indexed recall sprite imports. Nine release-integrity tests pass.

## Native coverage

| Runner | Passing checks | Coverage |
| --- | ---: | --- |
| General | 351 | Six fighters, actual controls/difficulty/pause, 44 complete replay rounds, 36,233 replay display frames |
| Animation | 1,260 | Native walk/jump/combat poses and pause |
| Elemental effects | 378 | Six themed shot/super identities and bounded effects |
| Combat feedback/audio | 115 | Six original shot/charge/release banks, real stereo audio, guard/exhaustion, fixed-HUD shake and charge pause |
| Practice/depth | 241 | Guides, follow-ups, actual guard costs, Meowth punish, reset/pause, 12,000-frame replay with 19 automatic restarts |
| CPU tactics | 4,649 | All six CPU styles at all three difficulties and 18 complete stress rounds |
| Music/recall | 2,637 | Loop, independent mutes, paused options, real win/loss, all recall phases, renderer visibility and skip |

All seven suites report **zero missed display frames**. Highest measured frame use is **93.85%** (CPU flow); practice/depth peaks at **91.26%**. An initial practice-menu upload peaked at 98.63% with music active. Splitting portrait uploads into two per frame reduced that measured practice-flow peak without changing selection input timing. Static memory is about 20.1 KB IWRAM (62.9%) and 34.9 KB EWRAM (13.6%), leaving about 11.9 KB IWRAM for stack use.

The general runner exceeds ten nominal replay minutes, including round-ending presentation. These measurements cover tested emulator scenarios, not every possible real-hardware situation.

## Measured sound behavior

The music test records over a full nominal 34.29-second loop, observes all four order positions, continues playback after looping, and rejects silent gaps in 200 ms windows. The recorded title tune has **-28.07 dBFS RMS**, peak 3,814/32,768 and zero clipped samples. Fixed moderate gains and source ramps are described in AUDIO_AND_RECALL.md.

With both buses disabled, recorded menu and battle PCM each have **peak 0**: actual digital silence, not only a disabled UI label. Independent Music-On/Sound-Off and Music-Off/Sound-On states are also exercised. Navigation is recorded with music off to establish that foreground cues themselves are audible. Muting, rematch transitions and paused options retain the selected settings during the run.

The new cue mask covers navigation, confirm/back, pause/resume, countdown/fight, knockout, recall opening/absorption/seal and both win/loss stingers. The draw-stinger asset is validated, but a native draw playback was not forced. All recorded menu/recall segments have zero clipped samples. Per-recording peaks, RMS values and sample counts are committed in native-presentation.json; sparse cues include intentional silence between events, so their integrated RMS is not directly comparable with continuous music.

## Recall correctness and footage

Actual button-driven fights produce a player loss and a player victory. Both render all seven recall phases, keep the winner visible, and hide the defeated sprite from frame 92. Tests compare the complete resolved combat telemetry throughout the animation: tick, resources, positions, actions, timer and outcome stay fixed. A separate real round verifies the A/Start skip after closing. Defeat results keep the player in the ball. Practice remains a quick reset; exhaustive host coverage checks draws and timeout losses.

The GIF and MP4 in docs/media/ are exported from the compiled ROM's framebuffer. The MP4 includes actual native title and recall/result audio at the tested mix. The graphics, composition and effects are source-generated native assets rather than an illustrative mockup.

## Rebuild and distribution

A clean staged source snapshot installs pinned Node dependencies from cache offline, regenerates all graphics/character/UI/recall inputs, combat/presentation PCM, the tracker module and configuration, validates assets, then clean-builds with the pinned native compiler. **229 generated native input files match**, and the clean ROM is byte-identical to the tested download.

ROM: **1,628,716 bytes**. SHA-256:

```text
1f647ff90b98bba7a021ad4e02f5ae07fb847cb8cb71db9c0b52c1d1f19216fe
```

Physical GBA hardware, non-Linux builds and human matchup balance remain unverified. Audio choices are per-session; no persistent save format is added. See BUILDING.md for commands, AUDIO_AND_RECALL.md for contracts/tradeoffs, and verification/v0.7.0/ for machine-readable evidence.
