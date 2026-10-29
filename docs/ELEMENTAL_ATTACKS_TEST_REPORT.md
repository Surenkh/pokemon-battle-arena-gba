# v0.3.0 validation

Tested 29 October 2026. Rebuilding the original project first produced the original v0.2.0 ROM byte for byte: `d13c7fd6642aaa6a0fe582b9ee8b479f0ce3aae10e503ec70f7d78ec82843845`.

## Implementation

Thirty indexed effect sheets contain 120 frames: normal projectiles, supers, muzzle flashes, impacts, and guard effects for all six fighters. The five sheets for a fighter share one palette. Travel animation uses the combat tick; effects pause during hitstop, mirror with facing, and use a 12-slot decorative pool. They consume no simulation RNG.

The native stress run caught an existing HUD edge case: tiny positive stamina values could round an affine sprite scale down to zero. Positive bars now have a one-pixel minimum width. Resource values and gameplay are unchanged.

Combat simulation, balance data, character animation definitions and CPU decision code are unchanged from the restored project.

## Results

| Check | Result |
| --- | --- |
| Combat host suite | 7,195,540 assertions; 720 seeded matches; all 36 ordered matchups |
| Movement/animation host suite | 18,904 checks passed |
| Existing character assets | All 408 packed frames validated |
| Effect assets | 30 sheets / 120 visible frames; four distinct travel frames per sheet; six distinct normal silhouettes; shared palettes verified |
| General native flow | 297 checks; 35 rounds; 36,678 battle display frames; 36 player super activations |
| Native movement animation | 1260 checks across all six fighters |
| Native elemental flow | 436 checks; 7 rounds; earned super activations and themed effects for every fighter |
| Missed native display frames | 0 |
| Peak CPU usage in general replay | 87.62% of the display-frame budget |

Native runs used mGBA 0.10.2, physical inputs and read-only telemetry. Close-range supers can hit before their full travel loop is shown. All super frame artwork is checked by the asset suite; native reports preserve the actual observed frame masks. Reports are in [verification/](verification/).

## Reproduce and limits

See [BUILDING.md](BUILDING.md). Hardware and non-Linux rebuilding have not been tested. This pass adds attack visuals and the HUD fix. New elemental sound sets, separate trails, status effects, balance changes and AI changes remain future work.

ROM bytes: 1,355,164. SHA-256: `1f5091ee0f6959731c609f7d4decda7ac5867d56365229df732c67bdff4db817`.

A clean tracked-file snapshot was extracted without local dependencies or optional web artwork. npm ci, full asset preparation/conversion, both asset suites, and a clean native build succeeded. All regenerated graphics/headers matched, and the ROM SHA-256 was identical: `1f5091ee0f6959731c609f7d4decda7ac5867d56365229df732c67bdff4db817`.
