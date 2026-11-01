# v0.8.0 validation — Modes & New Challengers

Verified 1 November 2026. ROM: 9b6ccf583242de730a76f4d9cf40f50beed2e5bdf2e842704b14df7c9e70d60c (2,174,968 bytes). Native build: pinned GBA Studio 0.5.0, devkitARM 16.1.0; emulator: mGBA 0.10.2 software renderer. These are new checks of this nine-fighter build.

## Outcome

Eight native suites passed with zero reported missed display frames: general match/menu flow, character animation, elemental effects, combat feedback/audio, fighter depth/practice, CPU tactics, music/recall and the expanded menu/Arcade/save suite.

- All six main-menu buttons were entered using D-pad and A/Start/B. A recorded input mask confirms that no L/R shoulder shortcut was used for the new menu paths. The title and mode menu are visibly different from v0.7.
- Nine selectable fighters were exercised in native controls/animation/effects/CPU checks. Every CPU style was observed at Easy, Normal and Hard. All nine shot, charge and release sound banks played in the real mixer. Peak accumulated CPU use in the CPU suite was 98.6%; hardware remains unverified.
- The new-fighter suite entered each guide with Select and played Psyduck, Jigglypuff and Machop in Practice, including movement, jumps, melee, projectiles, block, charge and super release. Actual framebuffer GIFs are included. Runtime animation/graphic bounds and the 611-frame atlas passed asset checks.
- The visible-menu/Arcade suite passed 121 native checks. An idle player naturally lost three fights, consumed exactly two continues and could not take a third. Real external play later completed a five-rival run. Resources reset correctly and the chosen difficulty stayed fixed; the champion screen awarded a single clear.
- Records and audio preferences survived power cycling. A fixed six-fighter schema-1 SRAM fixture retained old counters/times/flags, migrated on the next write to schema 2, and recovered the older schema-1 copy after offline corruption of the newer copy. New fighter records started empty. No gameplay-memory writes were used.
- Visual review checked the mode buttons, nine-card grid, fighter guides and new body sprites. Selector text/focus spacing was adjusted to keep all names and stats readable.

## Host and artifact checks

Host regressions passed 15,786,295 combat assertions over 1,620 seeded matches / all 81 ordered matchups; 28,352 movement/animation checks; 139,443 feedback checks; 23,621 original-trait/practice checks; 2,029,694 CPU assertions / 216 deterministic rounds; and 816,159 round-presentation checks. Separate roster tests verified Jigglypuff's 47-tick jump versus the standard 33, Psyduck's projectile knockback, and Machop's actual guarded-melee stamina pressure.

Arcade/save property checks covered 2,700 rosters, 7,516,800 single-bit corruptions and 696 interrupted-write cases, plus legacy migration, per-fighter/difficulty isolation, retries, audio flags and sequence wraparound. All 611 character frames, 45 VFX sheets / 180 VFX frames, 39 combat sounds and 14 presentation cues passed asset/import checks. Nine packaging/publication unit tests passed locally.

Regenerating the tracked asset/config inputs reproduced their hashes. Removing generated build output and clean-building produced a ROM byte-for-byte identical to the tested build.

## Limits and publication state

Physical GBA hardware, non-Linux builds and human balance testing remain unverified. The existing six keep their original per-fighter tuning; the three new traits use ruleset v0.8.0. The native menu uses GBA controller navigation; keyboard/touch mappings belong to the emulator. Unfinished Arcade runs do not resume after power-off, and recovery from a damaged newest save can lose that latest update.

Changes are local pending owner approval. No GitHub upload, PR, merge, v0.8 tag, release or GitHub Actions run is claimed. Detailed reports are in verification/v0.8.0.
