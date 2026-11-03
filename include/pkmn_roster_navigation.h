#pragma once
#include "pkmn_character_defs.h"
#include "pkmn_selection_layout.h"
namespace pkmn {
inline constexpr int roster_columns=selection_columns,roster_page_size=character_count,roster_pages=1;
inline int roster_page(int){return 0;}
inline int roster_slot(int who){return who;}
inline int roster_page_count(int){return character_count;}
inline int roster_x(int who){return selection_grid_x+(who%roster_columns)*selection_step_x+selection_card_width/2;}
inline int roster_y(int who){return selection_grid_y+(who/roster_columns)*selection_step_y;}
inline int roster_up(int who){if(who>=roster_columns)return who-roster_columns;int target=who+((character_count-1)/roster_columns)*roster_columns;if(target>=character_count)target-=roster_columns;return target;}
inline bool roster_has_down(int who){return who+roster_columns<character_count;}
}
