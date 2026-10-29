# Pokémon Battle Arena

**Pick your Pokémon. Read your rival. Earn your super.**

![Promotional cover](docs/media/cover.jpg)

A native Game Boy Advance fan-made fighting game by **Surenkh**, featuring Pikachu, Charmander, Squirtle, Bulbasaur, Eevee, and Meowth.

## Download and play

[**Download the GBA game**](downloads/PokemonBattleArena.gba?raw=true) · [SHA-256 checksum](downloads/SHA256SUMS.txt)

Download `PokemonBattleArena.gba` and open it in a GBA emulator such as mGBA. On GitHub’s file page, use **Download raw file** if the download does not begin automatically. Use the in-game help screen for exact controls.

Version **0.2.0 — Movement Update**. The playable ROM is available directly in this repository. A separate GitHub Release has not been created yet.

## Combat

Move, jump, squeeze, block, and fire while managing stamina. Land hits to charge a super. Supers bypass blocking, but can be evaded by jumping or squeezing. Animated states, momentum, impact reactions, and CPU opponents bring timing and spacing into each fight.

## Version 0.2.0

This repository currently distributes the playable ROM and its documentation. Editable game source is not included in this package. A source/build release must be added separately; the ROM cannot substitute for the editable project. Physical hardware and non-Linux builds have not been confirmed. The cover is promotional artwork, not a gameplay screenshot.

## Verify your download

Download `downloads/SHA256SUMS.txt` from this repository and compare it with the ROM’s SHA-256 checksum.

```text
d13c7fd6642aaa6a0fe582b9ee8b479f0ce3aae10e503ec70f7d78ec82843845  PokemonBattleArena.gba
```

## Development

The next iteration focuses on [elemental attack identities](docs/development/ITERATION_01_ELEMENTAL_ATTACKS.md), with [five improvement directions](docs/development/ROADMAP.md) and a [Git workflow](CONTRIBUTING.md). Its design is in preparation; the downloadable ROM remains v0.2.0. Editable source must be restored before building the gameplay update.

## Feedback

For bugs, include the game version, emulator/version or device, selected fighters, steps to reproduce, and expected versus actual result.

## Credits

Unofficial fan project. Not affiliated with or endorsed by Nintendo, Game Freak, or The Pokémon Company. Pokémon names and characters belong to their respective owners. No commercial game ROM or BIOS is included. No new license is granted to underlying characters, engine components, or third-party art by this repository. Preserve the original engine notices and corresponding source when preparing wider distribution.
