# Implementation status — 31 October 2026

Playable release and input ruleset: **0.7.0 — Sound & Poké Ball Recall**, developed on `feat/audio-and-recall` from released v0.6.0.

The game retains six animated fighters and traits, deterministic combat/physics, elemental attacks and sounds, stamina, hit-earned unblockable supers, one hit-confirm follow-up, fighter-aware CPU with three difficulties, practice/guides, complete title/selector/pause/result/rematch flow, an editable GBA Studio native build and automated GitHub Releases.

The new update adds an original 34.29-second three-voice tracker title/menu loop and fourteen presentation cues. Music and Sound FX have independent On/Off options accessible through L in menus/pause and an Audio pause entry. Settings stay selected during the current run; power-on restores the moderate default mix. The sound toggle controls combat, UI, recall and result sounds together. No new save format is introduced.

A 156-display-frame normal win/loss sequence recalls the defeated Pokémon into a Poké Ball through opening, beam, tint/shrink, close, fall and seal phases. The winner stays visible. Results show the winner or the closed ball on defeat. A/Start can skip from frame 112. Draws and practice retain their quick transition. The sequence never changes the resolved simulation state, outcome or CPU randomness.

Six host suites pass. Seven native suites pass with zero missed display frames, including independent audio mute combinations with recorded digital silence, looping music, actual win/loss recall phases, sprite visibility, skip, CPU levels, animation, effects, combat sound, pause and practice. Selector portrait uploads are spread across three display frames to preserve music/frame headroom. Full clean-build proof and exact counts/hashes are in [AUDIO_AND_RECALL_TEST_REPORT.md](AUDIO_AND_RECALL_TEST_REPORT.md) and verification/v0.7.0/.

Architecture and tradeoffs are in [AUDIO_AND_RECALL.md](AUDIO_AND_RECALL.md), [CPU_TACTICS.md](CPU_TACTICS.md) and [FIGHTER_DEPTH.md](FIGHTER_DEPTH.md). Commands are in [BUILDING.md](BUILDING.md). The release workflow validates six host suites, source audio/art integrity and ROM-bound native evidence before publication.

Remaining work: human playtesting and matchup tuning, physical GBA hardware, non-Linux rebuilds. Arena life and arcade progression remain the next roadmap direction; this sound/recall iteration was an additional improvement outside that roadmap. Additional arenas, multiplayer, music during fighting and persistent settings remain later scope.
