# v1.4.0-rc.1 Rival Cup — validation

Validated 4 November 2026 against the included ROM.

| Gate | Evidence |
| --- | --- |
| Full native regression | 19 suites, 25,338 checks; zero missed display frames |
| Host regression | 16 suites, including 5,449 Cup checks |
| Cup setup | All thirteen fighters, including mirrors; all nine Cup/difficulty setups; authored rival and location; ordinary combat resources |
| Cup completion | Real three-win clears of Tempo, Guard and Mirage through standard button inputs; one award per run and exact cumulative saved time |
| Goals | All three goals exercised with real resolved combat in host tests; Guard's style goal earned and saved in native play |
| Run flow | Pause freezes time; paid pause restart retains time; unavailable rematch skipped; loss/retry/exhaustion/abandon covered; draws retry free in host tests |
| Native UI | Countdown rival/counter and result cues matched against actual framebuffer glyphs; maximum legal counters fit all nine record entries |
| Persistence | Earlier six-fighter progress retained; Cup power cycle, corrupt-newest and interrupted-write fallback, both-invalid reset; ordinary Arcade records preserved |
| Isolation | Practice leaves Cup journal unchanged; actual two-player sets leave a populated Cup journal and all solo SRAM byte-identical |
| Clean build | Fresh pinned dependencies and complete animation/art/audio/config regeneration reproduce the ROM byte for byte |
| Distribution | 13 release-integrity tests, including stale/incomplete Cup evidence rejection; deterministic ZIP includes player guide and prior adapter/guides |

The dedicated Cup runner performs 672 checks through mGBA
0.10.2, using real GBA button masks and read-only telemetry. Offline save
fixtures test recovery and bounded record display. No gameplay memory is
written by that runner. The existing Versus/technique suites run through one
actual upstream mGBA development core and the shipped keyboard adapter.

The Cup journey peaks at 93.55% of a display frame after
staggering result uploads. The highest peak across the full regression is
93.70%. All missed-frame counters are zero. These are
emulated CPU/frame measurements, not physical button-to-display latency.

Combat core/header, balance, CPU configuration/definitions, existing progress
schema/store and Lua adapter are byte-identical to merged v1.3.0-rc.1
(commit 07ca5b0cec00079640ea22a15f7afa869ac21f70). Host checks confirm the Cup
observer leaves combat checksums unchanged. No additional 10,140-round balance
probe is claimed for this iteration; the previous probe remains historical
evidence, and the full host/native regression ran on this ROM.

Cup journal: schema 1, two 160-byte copies at offsets 4096/5120.
Existing progress: schema 4, two 596-byte copies at 1024/2048; older migrations
remain compatible. Cup records do not award Arcade clears or its mastery medal.

ROM: 3,539,528 bytes; SHA-256:
`1349e629e795a929f1f9185e5c85276b2b29faa3323aa398ae06939eaf32ec28`.

[Player guide](RIVAL_CUP_PLAYER_GUIDE.md) · [Design and pilot](RIVAL_CUP.md)
· [Machine-readable evidence](verification/v1.4.0-rc.1/).

Human Cup pacing, scouting usefulness, style-goal difficulty and voluntary
rematches remain unverified. Tempo/Mirage style completion is verified in host
combat rather than a native Cup clear. Physical GBA/controller sessions,
keyboard/Qt interaction and human matchup fairness remain open preview gates.
