# v1.2.0-rc.1 Local Versus — validation

Validated 4 November 2026 against the included preview ROM and keyboard adapter.
The stable release remains v1.1.0.

| Gate | Evidence |
| --- | --- |
| Native regression | 17 suites, 24,605 checks; zero missed display frames |
| Host regression | 14 suites, including 240 mailbox, selection and set-state checks |
| One-window input | One upstream mGBA development core; actual raw-key events and the shipping Lua adapter |
| Independent selection | All 13 choices per player; separate F1 cursors, ready locks/cancel, exact launched fighters and all 5 locations |
| Combat input | Simultaneous P1 melee/P2 shot and movement accepted on the first sampled frame; held attacks do not repeat; both jump, guard and squeeze |
| Set flow | Real P1/P2 wins, mirror matches, first-to-two, Next Round, fresh-score Rematch Set and Change Pokémon |
| Draw | Real equal-life timeout; neutral result and no point awarded |
| Pause/reconnect | Either player pauses; combat checksum freezes; stale attacks discarded; loss pauses; reconnect requires confirmation; script reload removes old callbacks |
| Save isolation | The existing legacy SRAM fixture remains byte-identical after the Local Versus journey |
| Solo modes | Existing 16 native suites pass: Arcade, Practice, mastery, records, save recovery, response, navigation, all fighters and animation effects |
| Reproducibility | Fresh pinned dependencies, regenerated art/audio/config and independent clean build produce a byte-identical ROM |
| Distribution | 11 release-integrity tests; packaging requires all 17 native reports and the tested adapter hash |

The Local Versus runner performs 239 native checks and peaks at **77.12%** of a
display frame. The highest peak across the complete regression is **92.43%**.
Records/Mastery/audio stress stays below its 90% gate. All recorded missed-frame
counters are zero. These are emulated CPU/frame measurements; they do not measure
physical keyboard-to-display latency.

The harness uses upstream mGBA commit
`a67176cd3e6bf60a5016d7adc94357142dd12617` with Lua 5.4.8. Host key events pass
through `mScriptContextFireEvent`, the real InputContext, the Lua raw input API
and the adapter's `keysRead` callback. The adapter discovers the versioned ROM
descriptor and writes only its dedicated input mailbox. The harness has no
combat-state write command. Damage, super power, KOs and wins come from actual
gameplay inputs. Images and GIF frames are captures of the native framebuffer.

The core, combat configuration, CPU configuration and save implementation are
byte-identical to the v1.1 baseline. This iteration routes a second human input
stream into the existing combat core and adds the surrounding match flow.
Mirror-match body palettes are independent, keeping recall/hit tint on the
correct fighter. Solo progression is excluded from Local Versus.

Version: `1.2.0-rc.1`. Combat ruleset: `1.0.0-rc.2`. Save schema: 4.
ROM: **3,424,892 bytes**. SHA-256:
`a6037fa9089f04d858444846ae9b3294fab27b28e0dc383e9c7094a0b81363f2`.

Adapter SHA-256:
`7a73371bd4d8858ec90ae487e121260b51714635ec6051ebc1662bb4c75b78f5`.

[Setup and controls](LOCAL_VERSUS.md) · [Native test setup](NATIVE_TESTING.md)
· [Machine-readable evidence](verification/v1.2.0-rc.1/).

## Desktop pilot still required

The automated harness does not exercise the Qt GUI, a physical keyboard,
keyboard rollover, operating-system shortcuts or gamepads. Stable mGBA 0.10.x
lacks the raw input API. This remains a development-build keyboard preview.

Before promoting it to stable, test two people on the same computer for ten
sets. Check focus changes, simultaneous movement/block/attacks, mirrors,
readiness, each player's pause, reconnect and quick rematch. Record OS, mGBA
revision, keyboard and any missed/confused inputs. Observe whether players want
another set and which matchups feel unfair. Physical GBA play, non-Linux rebuilds
and human balance review remain open.
