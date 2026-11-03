#pragma once
#include "pkmn_character_defs.h"
namespace pkmn {
inline constexpr int roster_columns=3,roster_page_size=9;
inline constexpr int roster_pages=(character_count+roster_page_size-1)/roster_page_size;
inline int roster_page(int who){return who/roster_page_size;}
inline int roster_slot(int who){return who%roster_page_size;}
inline int roster_page_count(int who){int remaining=character_count-roster_page(who)*roster_page_size;return remaining<roster_page_size?remaining:roster_page_size;}
inline int roster_up(int who){int local=roster_slot(who);if(local>=roster_columns)return who-roster_columns;int target=local+((roster_page_count(who)-1)/roster_columns)*roster_columns;if(target>=roster_page_count(who))target-=roster_columns;return who-local+target;}
inline bool roster_has_down(int who){return roster_slot(who)+roster_columns<roster_page_count(who);}
}
