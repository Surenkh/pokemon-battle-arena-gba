#pragma once
#include "pkmn_arcade.h"
#include "pkmn_combat_core.h"
namespace pkmn {
// Read-only observer of resolved combat. Never grants power or changes AI,
// hitboxes, random state, damage, clocks or the authoritative combat checksum.
class MasterySession {
 uint32_t last_frame=~0u,tracked_super=0;
 bool approached=false,evading=false,awarded=false;
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
  const auto& shot=c.p[3];const auto& player=c.f[0];
  if(shot.active&&shot.super&&shot.owner==1){
   if(tracked_super!=shot.id){tracked_super=shot.id;approached=evading=awarded=false;}
   int dx=shot.x-player.x;
   if((shot.vx<0&&dx>0)||(shot.vx>0&&dx<0))approached=true;
   int half_width=collision_super_hitbox[2]+collision_standing_hurtbox[2];
   int proximity=(half_width+8)*256;
   if(approached&&dx>=-proximity&&dx<=proximity&&(!c.grounded(player)||c.low(player)))evading=true;
   bool passed=shot.vx<0?dx < -half_width*256:dx > half_width*256;
   // Wait for the entire projectile to pass. A hit (even into invulnerability)
   // consumes it first, so it cannot count as a successful dodge.
   if(approached&&evading&&passed&&!awarded){
    awarded=true;
    if(m.evades<mastery_evade_goal){++m.evades;changed=true;}
   }
  }
  return changed;
 }
};
}
