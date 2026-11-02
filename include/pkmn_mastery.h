#pragma once
#include "pkmn_arcade.h"
#include "pkmn_combat_core.h"
#include "pkmn_super_evasion.h"
namespace pkmn {
// Read-only observer of resolved combat. Never grants power or changes AI,
// hitboxes, random state, damage, clocks or the authoritative combat checksum.
class MasterySession {
 uint32_t last_frame=~0u;
 SuperEvasion evasion;
public:
 void reset(){*this=MasterySession();}
 bool observe(const CombatCore& c,Progress& progress){
  if(c.training||last_frame==c.event_frame)return false;
  last_frame=c.event_frame;bool changed=false;
  auto& m=progress.mastery[c.f[0].character];
  for(int i=0;i<c.event_count;++i){
   const auto& e=c.events[i];
   if(e.kind==EV_HIT&&e.owner==0&&e.followup&&e.damage>0&&m.followups<mastery_followup_goal){++m.followups;changed=true;}
  }
  if(evasion.observe(c)&&m.evades<mastery_evade_goal){++m.evades;changed=true;}
  return changed;
 }
};
}
