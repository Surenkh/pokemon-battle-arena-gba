# v0.4.0 validation — Combat Feel

Tested 29 October 2026 on Linux x86_64 with GBA Studio 0.5.0, its bundled devkitARM 16.1.0 and mGBA 0.10.2. The native ROM is 1,430,492 bytes.

SHA-256: `cd01214a2b2eab27f9bbc1795d6cb0f7276854fa3756490b69bd5e4d75c45ede`.

## Changes verified

Source-specific combat events drive 27 original sound assets, hit flashes, bounded shake, guard/stamina feedback and super startup/release cues. The existing input queue now accepts descending jump presses within two ticks of touchdown. Character values, action costs, hitstop, jump trajectory and CPU code retain their prior values. See [COMBAT_FEEL.md](COMBAT_FEEL.md).

| Check | Result |
| --- | --- |
| Combat host suite | 7,195,540 assertions; 720 seeded matches; all 36 ordered matchups |
| Movement host suite | 18,904 checks |
| Feedback/input host suite | 160,143 checks; trades, hitstop, source identity, point-blank KO, pause, buffer boundaries and read-only presentation |
| Character/effect assets | 408 packed character frames; 30 effect sheets / 120 frames |
| Audio assets | 27 WAVs/imports; hashes, non-silence, bounded peaks, smooth endpoints and six distinct banks |
| General native flow | 297 checks; 35 rounds; 36,678 battle display frames (10.19 nominal minutes); 36 player super activations |
| Native character animation | 1260 checks across six fighters |
| Native elemental flow | 436 checks; 7 rounds; six fighter identities |
| Native feedback/audio | 173 checks; 13 stress rounds; all six shot, charge and release banks played |
| Native audio recording | Actual stereo PCM at 32,768 Hz; every captured round non-silent; no sustained clipping; at most two combat voices |
| Camera/HUD | At most 3 px horizontal shake; fixed HUD pixels; common scenery/actor translation during hitstop |
| Pause/resource cases | Combat and charging voices stop; simulation freezes; natural guard break and exhausted input observed |
| Missed native display frames | 0 across the tested suites |
| Peak CPU, general replay | 95.39% of a display frame |
| Peak CPU, all tested replays | 97.41% of a display frame |
| Static EWRAM / IWRAM | 34.9 KB / 25.8 KB; linker reports approximately 6.2 KB of IWRAM left for stack |

Machine-readable reports are under [verification/v0.4.0/](verification/v0.4.0/). Native runners use real button inputs and read-only telemetry, with no game-memory writes. The feedback replay creates distance before a full-SP super; rounds where startup is interrupted do not count as a completed release. All six fighters eventually release their own super through normal play. Native audio is taken from the emulator channels, rather than inferred from asset names or event counters alone.

## Reproduction and limits

[BUILDING.md](BUILDING.md) contains host, asset and native commands. The new native feedback runner requires Pillow. The original generic regression policy remains available alongside the release-oriented audio replay policy.

The highest observed CPU cost leaves about 2.6% of the tested frame budget free. This is limited headroom: increasing voices, background layers or effects requires fresh profiling. Zero missed frames in these deterministic replays does not establish every possible match or physical hardware timing. Human balance testing, physical GBA hardware, non-Linux rebuilding and subjective listening/playtesting remain outstanding. No background music or new balance mechanics are included.

A fresh tracked-source snapshot, without local build output or dependency caches, passed offline npm ci, full character/effect conversion, audio generation and both asset suites, followed by a clean native build. All 183 compared graphics/audio/header/content files matched. A clean build in the original project produced the identical ROM: `cd01214a2b2eab27f9bbc1795d6cb0f7276854fa3756490b69bd5e4d75c45ede`. The incremental candidate had stale build layout and was replaced; final native checks use this clean ROM. See verification/v0.4.0/clean-build.json.
