# Optional native regression runner

On a Linux machine with mGBA development headers and library installed, compile the driver:

```sh
cc tests/mgba_driver.c -lmgba -o tests/mgba_driver
python3 tests/native_flow.py
```

Run a native build first, preserving `.gbastudio/PokemonBattleArena.elf`. The script discovers the telemetry symbol from that ELF, sends physical button inputs to the ROM, and reads telemetry. It does not write gameplay state. It creates its logs and captures under `tests/native_artifacts/`. Set `PKMN_MGBA_DRIVER` to override the driver executable and `LD_LIBRARY_PATH` if using a locally extracted mGBA library. The supplied mGBA adapter was tested with 0.10.2; API differences may require adaptation for other versions.

The driver now reads 32 telemetry words. Slots 25–26 are fighter frame indices, 27–28 are clip IDs and 29 is the live particle count. Run `tests/native_animation_flow.py` with the same driver and environment as `tests/native_flow.py`; it writes its report and native preview captures under `tests/native_artifacts/animation/`.
