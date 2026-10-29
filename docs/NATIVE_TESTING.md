# Optional native regression runner

On a Linux machine with mGBA development headers and library installed, compile the driver:

```sh
cc tests/mgba_driver.c -lmgba -o tests/mgba_driver
python3 tests/native_flow.py
```

Run a native build first, preserving `.gbastudio/PokemonBattleArena.elf`. The script discovers the telemetry symbol from that ELF, sends physical button inputs to the ROM, and reads telemetry. It does not write gameplay state. It creates its logs and captures under `tests/native_artifacts/`. Set `PKMN_MGBA_DRIVER` to override the driver executable and `LD_LIBRARY_PATH` if using a locally extracted mGBA library. The supplied mGBA adapter was tested with 0.10.2; API differences may require adaptation for other versions.

The driver now reads 32 telemetry words. Slots 25–26 are fighter frame indices, 27–28 are clip IDs and 29 is the live particle count. Run `tests/native_animation_flow.py` with the same driver and environment as `tests/native_flow.py`; it writes its report and native preview captures under `tests/native_artifacts/animation/`.

## Elemental effects

Run `python3 tests/native_effects_flow.py` with the same driver/environment. It uses real inputs to earn and fire supers for each fighter and validates normal travel phases, themed muzzle/hit effects, bounded live effects and frame timing. Word 30 is a seen-phase mask (normal bits 0–3, super bits 4–7). Word 31 records per-character hit, muzzle and guard effects at bit offsets 0, 8 and 16. Close supers may hit before displaying a full loop. All four super artwork frames are validated by tests/effects_assets_test.cjs.
