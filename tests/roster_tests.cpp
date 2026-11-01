#include "pkmn_combat_core.h"
#include "pkmn_animation.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace pkmn;
int flight(int ch,int&apex){CombatCore c;c.reset(ch,0);c.step({UP,UP},{});int duration=1;apex=c.f[0].y;while(!c.grounded(c.f[0])&&duration<150){c.step({},{});if(c.f[0].y<apex)apex=c.f[0].y;++duration;}assert(c.grounded(c.f[0]));return duration;}
int main(){
 assert(character_count==9);assert(!std::strcmp(characters[6].name,"PSYDUCK"));assert(!std::strcmp(characters[7].name,"JIGGLYPUFF"));assert(!std::strcmp(characters[8].name,"MACHOP"));
 int baseApex,floatApex;int ordinary=flight(0,baseApex),floating=flight(7,floatApex);assert(floating>ordinary&&floatApex<baseApex);
 for(int ch=6;ch<9;++ch){
  for(int other=0;other<character_count;++other){CombatCore c;c.reset(ch,other);assert(c.f[0].hp==characters[ch].hp&&c.f[0].sta==characters[ch].sta*60);}
  CombatCore c;c.reset(ch,0);c.f[0].x=64*256;c.f[1].x=176*256;c.step({B,B},{});bool hit=false;
  for(int t=0;t<100&&!hit;++t){c.step({},{});for(int i=0;i<c.event_count;++i)if(c.events[i].kind==EV_HIT){hit=true;assert(c.events[i].damage==characters[ch].fire_damage);assert(c.f[1].kb==characters[ch].fire_knockback);}}
  assert(hit);
  c.reset(ch,0);c.f[0].sp=100;c.step({R,R},{});bool released=false;for(int t=0;t<50;++t){c.step({},{});for(int i=0;i<c.event_count;++i)if(c.events[i].kind==EV_RELEASE)released=true;}assert(released&&c.f[0].sp==0);
  for(int clip=0;clip<CLIP_COUNT;++clip){const auto&a=animations[ch][clip];assert(a.count>0&&a.count<=16);for(int t=0;t<240;++t){auto p=loop_pose(ch,Clip(clip),t);assert(p.frame>=a.first&&p.frame<a.first+a.count);}}
 }
 // Heavy Hands causes additional stamina loss through an actual guarded melee.
 CombatCore c;c.reset(8,0);c.f[0].x=100*256;c.f[1].x=120*256;int initial=c.f[1].sta,frames=0;bool blocked=false;c.step({A,A},{L,L});++frames;
 for(int t=0;t<40&&!blocked;++t){c.step({},{L,0});++frames;for(int i=0;i<c.event_count;++i)if(c.events[i].kind==EV_BLOCK)blocked=true;}
 assert(blocked&&c.f[1].hp==characters[0].hp);int impact=initial-c.f[1].sta-frames*resources_block_drain_subunits_per_tick;assert(impact==19*60);
 std::printf("PASS nine-fighter traits/animation checks; float airtime %d versus %d ticks, Psy Push knockback and Heavy Hands guard pressure.\n",floating,ordinary);
}
