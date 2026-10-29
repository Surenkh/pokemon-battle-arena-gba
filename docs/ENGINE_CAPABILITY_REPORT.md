# Native engine integration

The game is built through the actual GBA Studio 0.5.0 bundled CLI. Its `buildProject` implementation registers indexed project graphics, generates templates and a Makefile, runs Butano/devkitARM, fixes the GBA header, and copies the ROM into `out/`.

The release's resources were extracted locally. Project `src/` and `include/` are compiled by that pipeline. A guarded bootstrap in `engine/studio_resources/public/templates/commons/src/main.cpp` initializes Butano and calls `pkmn::run_app()` when the project header is present. The original bootstrap remains in the other branch. `patches/gba-studio-bootstrap.patch` records the exact change. This is a project-specific native extension, not a documented visual fighting-game plugin.

`tools/studio_cli.mjs` supplies the resource path when running the bundled CLI under Node. `tools/build.py` uses the shipped project-scoped engine, configures the external compiler path, regenerates balance definitions, and invokes that CLI. No global editor installation is patched.

The first minimal ROM showed a native sprite responding to GBA controls. The final project was subsequently clean-built from the shipped engine sources. A clean build deletes `.gbastudio/`, including generated headers and objects; authored C++, indexed graphics and asset records remain outside it.

Verified platform: Linux x86_64, Python 3.12, Node 22 and GBA Studio's bundled devkitARM 16.1.0. Exact release and binary hashes are in `build_versions.json`. No graphical editor session was available; editor open/save and non-Linux rebuilds remain untested. The project is code-owned for combat/screens and asset-owned for GBA Studio graphics records.
