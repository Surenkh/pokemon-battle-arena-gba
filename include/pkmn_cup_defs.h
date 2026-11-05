#pragma once
#include "pkmn_combat_core.h"
namespace pkmn {
inline constexpr int cup_count=3,cup_challengers=3,cup_fights=3;
struct CupRival { const char* name; int fighter,arena; const char* cue; };
struct CupDefinition {
 const char* name; const char* goal; int target; const char* crest;
 CupRival challengers[cup_challengers]; CupRival captain; CaptainTactic tactic;
};
inline constexpr CupDefinition cups[cup_count] = {
{"TEMPO CUP","3 FOLLOW-UP HITS",3,"BOLT CREST",{
 {"NIA",0,0,"BLOCK THEN ANSWER A"},
 {"MILO",4,2,"BLOCK THE FOLLOW-UP"},
 {"CINDER",1,1,"JUMP THE FIRE THEN CLOSE"},
},{"REX",5,1,"BLOCK THE RUSH THEN HIT"},CaptainTactic::RUSH},
{"GUARD CUP","5 SUCCESSFUL BLOCKS",5,"SHIELD CREST",{
 {"FERN",3,0,"BAIT THE VINE THEN CLOSE"},
 {"MARIN",2,2,"SUPER BEATS SHELL GUARD"},
 {"ROOK",12,3,"STEP OUT OF BITE RANGE"},
},{"BRICK",8,4,"SUPER THROUGH HIS GUARD"},CaptainTactic::GUARD},
{"MIRAGE CUP","2 SUPER HITS",2,"MOON CREST",{
 {"LYRA",7,3,"WAIT OUT THE AIR BRAKE"},
 {"SHADE",10,4,"BLOCK OR JUMP THE BURST"},
 {"WISP",9,1,"JUMP IN PAST FIRE SPIN"},
},{"REN",11,0,"JUMP THE SHOTS TO CLOSE"},CaptainTactic::ZONE},
};
}
