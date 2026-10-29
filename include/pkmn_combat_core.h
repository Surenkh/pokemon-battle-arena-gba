#pragma once
#include <stdint.h>
#include "pkmn_character_defs.h"
namespace pkmn {
enum Buttons { A=1,B=2,SELECT=4,START=8,RIGHT=16,LEFT=32,UP=64,DOWN=128,R=256,L=512 };
enum Action { NONE,BLOCK,MELEE,FIRE,SQUEEZE,SUPER,HITSTUN,BLOCKSTUN,GUARD_BREAK };
enum Feedback { HIT=1,BLOCKED=2,SHOT=4,SUPER_FIRED=8,JUMPED=16,SWUNG=32,CHARGED=64,BROKEN=128 };
struct Input { uint16_t held=0,pressed=0; };
struct Fighter {
 int character=0,x=0,y=120*256,vx=0,vy=0,kb=0,hp=0,sta=0,sp=0,face=1;
 Action action=NONE;int began=0,ends=0,invuln_until=0,squeeze_ready=0,last_spend=-30;
 uint32_t attack_id=0;bool melee_contact=false,spawned=false;
 int pending=0,pending_until=0;
 int stride_q8=0,landed_at=-100,took_off_at=-100;
};
struct Projectile { bool active=false,reserved=false,super=false;int owner=0,x=0,y=0,px=0,py=0,vx=0,born=0,damage=0;uint32_t id=0; };
struct Observation { Fighter f[2];Projectile p[4]; };
class CombatCore {
public:
 Fighter f[2];Projectile p[4];int tick=0,remaining=3600,hitstop=0,outcome=0,feedback=0;uint32_t next_id=1,seed=1;
 void reset(int player,int cpu,uint32_t match_seed=1);
 void frame(Input player,Input cpu);
 void step(Input player,Input cpu);
 void buffer(int who,Input input);
 int age(const Fighter& v) const {return tick-v.began;}
 bool low(const Fighter& v) const {return v.action==SQUEEZE && age(v)>=squeeze_startup_ticks && age(v)<squeeze_startup_ticks+squeeze_active_ticks;}
 bool grounded(const Fighter& v) const {return v.y>=world_floor_y*256;}
 uint32_t checksum() const;
private:
 void action(int who,Input input);void physics();void spawn();void collide(const Fighter previous[2]);void resources();
 void enter(int who,Action a,int duration,int cost);void cancel(int who);
};
class CpuController {
 Observation history[32]{};int stored=-1;uint16_t held=0;uint32_t rng=1,threat=0;bool evade=false;int dodge=0;
 uint32_t random();
public:
 int difficulty=1;
 void reset(int level,uint32_t seed);
 Input input(const CombatCore& core,int who=1);
};
}
