#pragma once
#include "pkmn_combat_core.h"
namespace pkmn {
// Pure presentation state, driven by display frames. No randomness or writes
// to CombatCore; pausing does not consume shake/status lifetimes.
class CombatFeel {
 uint32_t last_event_frame=~0u;
 int shake_left=0,shake_size=0,shake_age=0;
public:
 int dx=0,dy=0,display_frame=0;
 int blocked[2]{},broken[2]{},tired[2]{},flash[2]{};
 uint32_t seen_events=0;int shake_frames=0,guard_breaks=0,rejected_actions=0,landing_jumps=0;
 void reset(){*this=CombatFeel{};}
 void update(const CombatCore& c,bool running){
  dx=dy=0;if(!running)return;++display_frame;
  for(int i=0;i<2;i++){if(blocked[i])--blocked[i];if(broken[i])--broken[i];if(tired[i])--tired[i];if(flash[i])--flash[i];}
  if(last_event_frame!=c.event_frame){last_event_frame=c.event_frame;
   for(int n=0;n<c.event_count;n++){
    const auto&e=c.events[n];seen_events|=1u<<e.kind;int size=0,duration=0;
    if(e.kind==EV_HIT){size=e.super?3:1;duration=e.super?12:5;flash[e.target]=e.super?8:4;}
    if(e.kind==EV_BLOCK)blocked[e.target]=12;
    if(e.kind==EV_BREAK){broken[e.target]=30;++guard_breaks;size=2;duration=10;}
    if(e.kind==EV_TIRED){tired[e.target]=18;++rejected_actions;}
    if(e.kind==EV_LANDING_JUMP)++landing_jumps;
    if(size&&(size>shake_size||!shake_left)){shake_size=size;shake_left=duration;shake_age=0;}
   }
  }
  if(shake_left){static constexpr int wave[8]={1,-1,1,0,-1,1,0,0};int strength=shake_left>4?shake_size:1;dx=wave[shake_age%8]*strength;dy=shake_size>=2&&shake_age%3==0?-1:0;++shake_age;--shake_left;if(dx||dy)++shake_frames;if(!shake_left)shake_size=0;}
 }
 // One compact HUD line per fighter, with critical states taking precedence.
 int status(const CombatCore& c,int who)const{
  if(broken[who]||c.f[who].action==GUARD_BREAK)return 4;
  if(tired[who])return 3;if(blocked[who])return 2;
  if(c.f[who].sta<fire_stamina_cost*60)return 1;
  return c.f[who].sp==100?5:0;
 }
};
}
