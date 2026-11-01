# v0.9.0 validation — Arenas & Fighter Mastery

Verified 1 November 2026 with pinned GBA Studio 0.5.0, bundled devkitARM 16.1.0
on Linux x86-64, and mGBA 0.10.2 software rendering/audio. The ROM is **2283172 bytes**.

SHA-256: `0ac4480de00ad3cdbed6df59e54195e46dd0d160ef1cd90e61ab0b449c7d29ff`.

## Native GBA checks

All nine native suites passed using physical button inputs, read-only telemetry,
framebuffer captures and actual emulator audio. No gameplay-memory writes were
used. Every versioned report is bound to the tested ROM hash and runner source.

| Suite | Checks | Peak CPU fraction | Missed display frames |
| --- | ---: | ---: | ---: |
| general | 319 | 0.7444 | 0 |
| animation | 1890 | 0.7710 | 0 |
| effects | 625 | 0.7227 | 0 |
| feel | 142 | 0.7371 | 0 |
| depth | 241 | 0.9204 | 0 |
| cpu | 7187 | 0.8030 | 0 |
| presentation | 2637 | 0.7471 | 0 |
| arcade | 162 | 0.9966 | 0 |
| mastery | 132 | 0.9175 | 0 |

The new suite plays Quick Battle and Practice on all three stages, checks the
seven visible mode buttons and Mastery/Records navigation without shoulder
shortcuts, earns a ten-follow-up medal from zero in regular fights, evades an
actual CPU super, and proves Practice cannot award mastery. It verifies retained
progress after power cycles, schema-2 migration and fallback from a corrupted
schema-3 copy. The Arcade suite separately covers schema-1 migration, a real
five-win clear, two-continue limits, arena rotation after a win and preservation
on retries.

Visual inspection covered the main menu, nine-fighter grid, new arenas and both
empty/earned mastery states at native 240×160 resolution. Mastery static labels
are baked into a background. Arena background upload and actor allocation use
separate countdown display frames to avoid roster-transition spikes. The
existing background/fighter translation check verifies synchronized impact shake.

## Host, assets and reproducibility

All nine host suites passed: combat, movement, feel, depth, CPU, presentation,
Arcade/save, roster and mastery. Combat tests cover all 81 ordered matchups.
Mastery tests use actual resolved follow-up and jump/squeeze sequences for all
nine fighters, enforce training exclusion and saturation, verify checksum
isolation, and test 100 seeded five-stage rotations. Save tests include 9,072,000
single-bit corruption cases, every interrupted payload length, sequence
wraparound and frozen six-/nine-fighter fixtures with unknown legacy tails.

All animation, elemental-art, combat-audio and presentation-audio checks passed.
An independent 1,716-file source snapshot installed pinned npm dependencies
offline, regenerated all graphics/UI/animation/audio/configuration inputs without
byte differences, and clean-built a byte-identical ROM. Compiler binaries and
caches were not copied into the snapshot. Nine release-packaging/publication
integrity tests passed; local deterministic player ZIP/ROM/checksum packaging
accepted the complete ROM-bound evidence.

## Automated balance pass

The unchanged ruleset completed **4,860** CPU-versus-CPU rounds across all 81
ordered matchups, all three difficulties and 20 seeds, with **zero timeouts**.
Median active battle length was 862 ticks; mean was 1,016.73 ticks.

Two independent 4,860-round CPU comparison probes used fixed and holdout seeds.
These percentages measure the current CPU against the frozen older policy; they
are not human win rates or a human balance score.

| Probe | Easy | Normal | Hard |
| --- | ---: | ---: | ---: |
| Fixed seeds | 16.11% | 46.79% | 64.07% |
| Holdout seeds | 15.19% | 46.91% | 65.43% |

The probe tools now report the actual nine-fighter matchup/round counts and valid
JSON rather than the earlier hard-coded six-fighter metadata. Fighter stats,
movement, damage, resource rules and CPU parameters were not tuned on these data.

## Remaining validation

Human matchup playtesting, physical GBA hardware profiling, non-Linux clean
builds and graphical editor open/save behavior remain unverified. The small
emulator headroom on the busiest Arcade transition warrants hardware profiling
before a hardware-performance claim. Unfinished Arcade runs do not resume after
power-off. Current-match mastery steps can be lost by abrupt power-off before a
save boundary. No human balance or physical-hardware claim is made.
