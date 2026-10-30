// Frozen v0.5 CPU policy for controlled v0.6 comparisons. Not linked into the ROM.
#pragma once
#include "pkmn_combat_core.h"
namespace pkmn {
class V05CpuController {
 struct LegacyObservation { Fighter f[2];Projectile p[4]; };
 LegacyObservation history[32]{};int stored=-1;uint16_t held=0;uint32_t rng=1,threat=0;bool evade=false;int dodge=0;
 bool resting=false;
 static int maxv(int a,int b){return a>b?a:b;}
 static int absv(int a){return a<0?-a:a;}
 static int clamp(int n,int a,int b){return n<a?a:n>b?b:n;}
 uint32_t random();
public:
 int difficulty=1;
 void reset(int level,uint32_t seed);
 Input input(const CombatCore& core,int who=1);
};
inline uint32_t V05CpuController::random(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
inline void V05CpuController::reset(int level,uint32_t value){difficulty=clamp(level,0,2);rng=value?value:1;stored=-1;held=0;threat=0;evade=false;dodge=0;resting=false;}
inline Input V05CpuController::input(const CombatCore&core,int who){if(core.hitstop||core.outcome)return{held,0};if(stored!=core.tick){auto&o=history[core.tick%32];o.f[0]=core.f[0];o.f[1]=core.f[1];for(int i=0;i<4;i++)o.p[i]=core.p[i];if(stored<0)for(int i=0;i<32;i++)history[i]=o;stored=core.tick;}
 held&=uint16_t(LEFT|RIGHT|L);if(core.tick%6)return{held,0};const int delays[3]={18,12,8},rates[3]={45,70,85};int delay=delays[difficulty],at=maxv(0,core.tick-delay);const auto&o=history[at%32];const auto&me=core.f[who];const auto&enemy=o.f[1-who];int delta=enemy.x-me.x,dist=absv(delta)/256;uint16_t next=0;
 uint32_t threat_id=0;int eta=999;bool super_threat=false;for(const auto&b:o.p)if(b.active&&b.owner!=who&&((me.x-b.x)*int64_t(b.vx)>0)){int arrival=absv(me.x-b.x)/absv(b.vx)-delay;if(arrival<eta){eta=arrival;super_threat=b.super;threat_id=b.id;}}
 if(enemy.action==SUPER&&enemy.attack_id){int arrival=super_startup_ticks-(at-enemy.began)-delay+maxv(0,dist-collision_super_spawn_forward_px-13)/5;if(arrival<eta){eta=arrival;super_threat=true;threat_id=enemy.attack_id;}}
 if(super_threat&&threat_id!=threat){threat=threat_id;evade=int(random()%100)<rates[difficulty];dodge=random()%2;}
 bool neutral=me.action==NONE||me.action==BLOCK;bool ground=core.grounded(me);
 if(me.sta<12*60)resting=true;else if(me.sta>=32*60)resting=false;
 if(super_threat&&evade&&eta>=0&&eta<=(dodge?15:18)&&neutral&&ground&&me.sta>=18*60){if(dodge&&core.tick>=me.squeeze_ready)next=DOWN;else next=UP;}
 else if(!super_threat&&eta>=0&&eta<18&&neutral&&ground&&me.sta>=20*60)next=L;
 else if(!resting&&core.can_followup(who,A,input_buffer_ticks-1)&&dist<characters[me.character].melee_reach+6)next=A;
 else if(!resting&&core.can_followup(who,B,input_buffer_ticks-1)&&dist<140)next=B;
 else if(neutral){if(resting){next=0;}
 else if(me.sp==100&&me.sta<25*60){next=delta>0?LEFT:RIGHT;}
 else if(me.sp==100&&me.sta>=25*60&&ground&&dist>38&&dist<155&&enemy.y>=110*256)next=R;
 else if(dist<characters[me.character].melee_reach+6&&me.sta>=8*60)next=A;
 else if(dist>42&&dist<175&&me.sta>=25*60&&random()%100<65)next=B;
 else if(dist>23)next=delta>0?RIGHT:LEFT;
 else if(me.sta<18*60)next=delta>0?LEFT:RIGHT;
 if(!next&&ground&&random()%100<5&&me.sta>=40*60)next=UP;}
 uint16_t pressed=next&uint16_t(A|B|R|UP|DOWN);held=next&uint16_t(LEFT|RIGHT|L);return{uint16_t(next),pressed};}
}
