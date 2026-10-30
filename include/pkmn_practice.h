#pragma once
#include "pkmn_combat_core.h"
namespace pkmn {
enum DummyMode { DUMMY_IDLE,DUMMY_BLOCK,DUMMY_ATTACK,DUMMY_MODE_COUNT };
struct PracticeStats {
 int last_damage=0,chain=0,best_chain=0,hits=0,followups=0,punishes=0;
};

// Produces ordinary buttons; the dummy receives no special collision or resources.
class PracticeSession {
 uint32_t last_event_frame=~0u;
 static int bounded(int n){return n>9999?9999:n;}
public:
 DummyMode mode=DUMMY_IDLE;int opponent=1;PracticeStats stats;
 void begin_round(bool keep_stats=false){last_event_frame=~0u;if(!keep_stats)stats=PracticeStats{};}
 Input input(const CombatCore& c)const {
  if(c.hitstop||c.outcome)return {};
  if(mode==DUMMY_IDLE)return {};
  if(mode==DUMMY_BLOCK)return {L,0};
  const auto&v=c.f[1];int delta=c.f[0].x-v.x,dist=(delta<0?-delta:delta)/256;
  if(c.tick%practice_dummy_think_ticks==0){
   if(c.can_followup(1,A)&&dist<characters[v.character].melee_reach+6)return {A,A};
   if(c.can_followup(1,B)&&dist<140)return {B,B};
  }
  if(v.action!=NONE&&v.action!=BLOCK)return {};
  if(c.tick%practice_dummy_think_ticks==0){
   if(v.sp==100&&v.sta>=super_stamina_cost*60&&dist>38&&c.grounded(v)&&c.tick%practice_dummy_super_interval_ticks==0)return {R,R};
   if(dist<characters[v.character].melee_reach+6&&v.sta>=melee_stamina_cost*60)return {A,A};
   if(dist>48&&v.sta>=fire_stamina_cost*60&&c.tick%practice_dummy_shot_interval_ticks==0)return {B,B};
  }
  if(dist>characters[v.character].melee_reach+3)return {uint16_t(delta>0?RIGHT:LEFT),0};
  return {};
 }
 void observe(const CombatCore& c){
  if(last_event_frame==c.event_frame)return;last_event_frame=c.event_frame;
  for(int i=0;i<c.event_count;++i){const auto&e=c.events[i];if(e.owner!=0)continue;
   if(e.kind==EV_PUNISH)stats.punishes=bounded(stats.punishes+1);
   if(e.kind==EV_HIT){stats.last_damage=e.damage;stats.hits=bounded(stats.hits+1);
    stats.chain=e.followup?2:1;if(stats.chain>stats.best_chain)stats.best_chain=stats.chain;
    if(e.followup)stats.followups=bounded(stats.followups+1);
   }
  }
 }
};
}
