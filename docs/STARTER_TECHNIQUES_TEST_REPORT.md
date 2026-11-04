# Starter Mobility validation — v1.5.0

This release was reconstructed from merged v1.4 after the previous chat's
unpublished working files were unavailable. All results below were rerun on
this reconstructed build; earlier v1.5 counts are not reused.

ROM SHA-256: `bbac8d74a0f1408a6ca5531da1b043fbd9228d02f4895f1922d42c54168d9e42`. Size: 3,542,840 bytes.

## Result

- **20 native suites, 25,649 checks, zero missed display frames**.
- **17 host suites** and **15 release-integrity tests** pass, together with
  all animation/graphics, elemental effects, original audio and import checks.
- The new starter suite plays both lessons, uses both Versus sides, measures
  first-frame acceptance and exact costs/movement, observes real super evasion,
  freezes movement during Pause, clears a partial gesture across Resume and
  verifies fast retry plus byte-identical solo saves.
- Host starter coverage adds 379 checks for both directions/sides, held/expired
  gestures, insufficient stamina, recovery/cooldown boundaries, walls, ordinary
  damage, melee counterplay, hitstop queues and latched-direction checksums.
- Paired seeded probes play 10,140 matches per build. The 121 pairings between
  the other eleven fighters retain the same outcomes, timing and replay digests
  for all 7,260 matches. Human balance is not established by CPU probes.
- A separate source snapshot, fresh pinned dependencies and full animation,
  art/audio/config regeneration produce a **byte-identical clean-build ROM**.
  Save schemas, the Lua adapter, thirteen-fighter roster and five arenas remain
  compatible. Full Cup/Arcade/records/mastery/audio/save regressions pass.

Peak across the native reports: **95.53%** of a display frame.
The dedicated starter journey peaks at
**80.79%**.
The general suites use mGBA 0.10.2; Versus/techniques/starter suites use the
pinned actual upstream 0.11 development core with Lua and real raw-key events.
The test harness writes only its declared input mailbox; new technique tests
never modify live combat state. Native framebuffer captures were visually
reviewed for the guides, effects and completion labels.

Evidence is under `docs/verification/v1.5.0/`, bound to this ROM and the actual
runner/driver sources. The publication guard rejects missing, stale or partial
starter evidence and a changed input adapter. The dedicated release branch can
publish v1.5 as GitHub Latest while its PR remains owner-controlled.

Physical keyboard rollover, the Qt/controller experience, human readability and
matchup fairness remain open playtest work. Automated emulation does not prove
those results.
