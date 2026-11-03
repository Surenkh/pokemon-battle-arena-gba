#pragma once
#include <stdint.h>
namespace pkmn {
enum ArenaId { WOODLAND, VOLCANIC, SHORELINE, FROSTPEAK, ANCIENT_RUINS, arena_count };
inline constexpr const char* arena_names[arena_count] = {"WOODLAND", "VOLCANIC", "SHORELINE", "FROSTPEAK", "ANCIENT RUINS"};
inline constexpr const char* arena_descriptions[arena_count] = {
 "A QUIET CLEARING UNDER THE CANOPY.",
 "EMBER SKIES ABOVE A LAVA CRATER.",
 "CALM WATER AND DISTANT ISLANDS.",
 "FROZEN RIDGES UNDER A SNOWY SKY.",
 "TORCHLIGHT AMONG ANCIENT STONES."
};
inline int arcade_arena(uint32_t first, int stage) { return int((first + uint32_t(stage)) % arena_count); }
// Presentation-only, bounded one-pixel horizontal motion above the fighters.
// HUD, floor, collision geometry and gameplay clocks never use these values.
inline int arena_sway(int arena, int y, int phase) {
 if(arena<0||arena>=arena_count)return 0;
 // Static bounds avoid copying arrays for each of the 160 scanlines.
 static constexpr int tops[]={50,46,78,46,61},bottoms[]={84,69,96,87,77};
 if(phase<0||y<tops[arena]||y>=bottoms[arena])return 0;
 int band=arena==SHORELINE?y/3:arena==FROSTPEAK?y/5:arena==ANCIENT_RUINS?y/4:y/12;
 int wave=(phase+band)&3;
 return wave == 0 ? 1 : wave == 2 ? -1 : 0;
}
}
