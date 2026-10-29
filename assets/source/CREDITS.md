# Sources and artwork

The six main roster sprites are existing Generation III Emerald Pokémon battle sprites, retrieved through the [PokéAPI sprites repository](https://github.com/PokeAPI/sprites). They were cropped, aligned to a common 64 × 64 transparent canvas and mirrored where appropriate. They are sourced game assets, not newly generated character drawings.

The woodland arena and the bonus Charmander and Bulbasaur eight-pose sheets were created with built-in image generation. Original generated PNGs and prompts are included.

## Main sprite source files

- pikachu: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/25.png
- charmander: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/4.png
- squirtle: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/7.png
- bulbasaur: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/1.png
- eevee: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/133.png
- meowth: https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/versions/generation-iii/emerald/52.png

Retrieved: 2026-10-27.

## Game screens update

The six-Pokémon title ensemble is newly generated artwork. Its original PNG is `sources/title_ensemble.png`; its prompt is in `screens/ARTWORK_PROMPT.json`. The screen layouts, bitmap UI font, UI buttons, interface rendering and animation are implemented in code. The screen background layers reuse the generated woodland arena.

## Movement update

Runtime animation credits and original source records: [animations/CREDITS.md](animations/CREDITS.md).
