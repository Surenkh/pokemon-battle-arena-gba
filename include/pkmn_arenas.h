#pragma once
#include <stdint.h>
namespace pkmn {
enum ArenaId { WOODLAND, VOLCANIC, SHORELINE, arena_count };
inline constexpr const char* arena_names[arena_count] = {"WOODLAND", "VOLCANIC", "SHORELINE"};
inline constexpr const char* arena_descriptions[arena_count] = {
 "A QUIET CLEARING UNDER THE CANOPY.",
 "EMBER SKIES ABOVE A LAVA CRATER.",
 "CALM WATER AND DISTANT ISLANDS."
};
inline int arcade_arena(uint32_t first, int stage) { return int((first + uint32_t(stage)) % arena_count); }
// Presentation-only, bounded one-pixel horizontal motion above the fighters.
// HUD, floor, collision geometry and gameplay clocks never use these values.
inline int arena_sway(int arena, int y, int phase) {
 int top = arena == WOODLAND ? 50 : arena == VOLCANIC ? 46 : 78;
 int bottom = arena == WOODLAND ? 84 : arena == VOLCANIC ? 69 : 96;
 if(phase < 0 || y < top || y >= bottom) return 0;
 int wave = (phase + y / (arena == SHORELINE ? 3 : 12)) % 4;
 return wave == 0 ? 1 : wave == 2 ? -1 : 0;
}
}
