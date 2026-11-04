#pragma once
namespace pkmn {
inline constexpr int cup_count=3,cup_rivals=3;
struct CupRival { const char* name; int fighter,arena; const char* cue; };
struct CupDefinition { const char* name; const char* goal; int target; CupRival rivals[cup_rivals]; };
inline constexpr CupDefinition cups[cup_count] = {
{"TEMPO CUP","3 FOLLOW-UP HITS",3,{
{"NIA",0,0,"BLOCK THEN ANSWER A"},
{"MILO",4,2,"BLOCK THE FOLLOW-UP"},
{"REX",5,1,"RECOVER OUT OF REACH"},
}},
{"GUARD CUP","5 SUCCESSFUL BLOCKS",5,{
{"FERN",3,0,"BAIT THE VINE THEN CLOSE"},
{"MARIN",2,2,"SUPER BEATS SHELL GUARD"},
{"BRICK",8,4,"STEP BACK FROM HEAVY A"},
}},
{"MIRAGE CUP","2 SUPER HITS",2,{
{"LYRA",7,3,"WAIT OUT THE AIR BRAKE"},
{"SHADE",10,4,"BLOCK OR JUMP THE BURST"},
{"REN",11,0,"BAIT A THEN PUNISH"},
}},
};
}
