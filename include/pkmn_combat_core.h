#pragma once
#include <stdint.h>
#include "pkmn_character_defs.h"
#include "pkmn_cpu_defs.h"
namespace pkmn {
enum Buttons { A=1,B=2,SELECT=4,START=8,RIGHT=16,LEFT=32,UP=64,DOWN=128,R=256,L=512 };
enum Action { NONE,BLOCK,MELEE,FIRE,SQUEEZE,SUPER,HITSTUN,BLOCKSTUN,GUARD_BREAK,VOLT_STEP,BREAKAWAY };
enum Feedback { HIT=1,BLOCKED=2,SHOT=4,SUPER_FIRED=8,JUMPED=16,SWUNG=32,CHARGED=64,BROKEN=128 };
struct Input { uint16_t held=0,pressed=0; };
// Contextual requests, never additional physical controller buttons.
enum TechniqueRequest { REQUEST_AIR_BRAKE=1024, REQUEST_ORB_BURST=2048, REQUEST_VOLT_STEP=4096, REQUEST_SHELL_ROLL=8192, REQUEST_BREAKAWAY=16384 };
struct Fighter {
 int character=0,x=0,y=120*256,vx=0,vy=0,kb=0,hp=0,sta=0,sp=0,face=1;
 Action action=NONE;int began=0,ends=0,invuln_until=0,squeeze_ready=0,last_spend=-30;
 uint32_t attack_id=0;bool melee_contact=false,spawned=false;
 int pending=0,pending_until=0;
 int stride_q8=0,landed_at=-100,took_off_at=-100;
 int confirmed_at=-100,chain_depth=0;
 int air_brake_until=0;bool air_brake_used=false;
 int tap_direction=0,tap_tick=-100,step_ready=0,mobility_direction=0;bool shell_roll=false;
};
struct Projectile { bool active=false,reserved=false,super=false,followup=false;int owner=0,x=0,y=0,px=0,py=0,vx=0,born=0,damage=0;uint32_t id=0;int burst_at=-1; };
// Read-only presentation events; never consulted by combat resolution or AI.
enum EventKind { EV_MELEE,EV_SHOT,EV_CHARGE,EV_RELEASE,EV_HIT,EV_BLOCK,EV_BREAK,EV_TIRED,EV_READY,EV_JUMP,EV_LANDING_JUMP,EV_FOLLOWUP,EV_PUNISH,EV_AIR_BRAKE,EV_ORB_ARM,EV_ORB_POP,EV_BURST_HIT,EV_VOLT_STEP,EV_SHELL_ROLL,EV_BREAKAWAY,EV_CLASH,EV_SUPER_FINISH,EV_PUNISH_FINISH,EVENT_COUNT };
struct CombatEvent { EventKind kind;int owner,target,x,y;bool super;uint32_t attack;int damage;bool followup; };
class CombatCore {
public:
 Fighter f[2];Projectile p[4];int tick=0,remaining=3600,hitstop=0,outcome=0,feedback=0;uint32_t next_id=1,seed=1;
 bool training=false;
 static constexpr int max_events=32;
 CombatEvent events[max_events]{};int event_count=0;uint32_t event_frame=0;
 void reset(int player,int cpu,uint32_t match_seed=1);
 void frame(Input player,Input cpu);
 void step(Input player,Input cpu);
 void buffer(int who,Input input);
 int age(const Fighter& v) const {return tick-v.began;}
 bool low(const Fighter& v) const {return v.action==SQUEEZE && age(v)>=squeeze_startup_ticks && age(v)<squeeze_startup_ticks+squeeze_active_ticks;}
 bool grounded(const Fighter& v) const {return v.y>=world_floor_y*256;}
 bool can_followup(int who,int button,int ahead=0)const;
 bool recovering(int who)const;
 bool can_air_brake(int who)const;
 bool can_burst(int who)const;
 bool can_volt_step(int who)const;
 bool can_breakaway(int who)const;
 bool rolling(int who)const { const auto& v=f[who];return v.shell_roll&&low(v)&&age(v)<squeeze_startup_ticks+shell_roll_motion_ticks; }
 bool air_braking(int who)const { return f[who].air_brake_used && tick<f[who].air_brake_until && !grounded(f[who]); }
 int burst_phase(const Projectile& shot)const;
 uint32_t checksum() const;
private:
 void begin_events(){event_count=0;++event_frame;}
 void emit(EventKind kind,int owner,int target,bool super=false,int damage=0,bool followup=false,uint32_t attack=0);
 bool jump_buffer_eligible(const Fighter& v)const;
 int technique_request(int who,int button)const;
 void action(int who,Input input);void physics();void spawn();void collide(const Fighter previous[2]);void resources();
 void enter(int who,Action a,int duration,int cost,bool followup=false);void cancel(int who);
};
enum class CaptainTactic { NONE, RUSH, GUARD, ZONE };
class CpuController {
 // This type cannot hold buffered inputs, invulnerability, resource values,
 // projectile reservations or any other hidden opponent state.
 struct VisibleFighter { int character=0,x=0,y=0,vx=0,vy=0,face=1;Action action=NONE;int began=0;uint32_t attack=0; };
 struct VisibleShot { bool active=false,super=false;int owner=0,x=0,y=0,vx=0;uint32_t id=0;int burst_at=-1; };
 struct Observation { VisibleFighter enemy;VisibleShot shots[4]; };
 Observation history[cpu_history_ticks]{};
 int stored=-1,next_plan=0,plan=0,jitter=0,hesitate_until=0,next_shot=0,rest_began=0;
 int last_hp=-1,last_power=-1,last_progress=0;
 uint16_t held=0;Input cached{};uint32_t rng=1,seen_attack=0,combo_attack=0;
 // One live normal shot, super and melee per opponent. Keep their choices
 // independently so changing which is nearest never re-rolls an old threat.
 uint32_t threat[3]{};bool respond[3]{};int dodge[3]{};
 bool resting=false,combo_allowed=false;int memory_at=0,last_seen=-1000;
 CaptainTactic tactic=CaptainTactic::NONE;
 Action memory[cpu_memory_actions]{};
 uint32_t random();
 bool roll(int percent){return int(random()%100)<(percent>95?95:percent);}
public:
 enum Intent { APPROACH,SPACE,PRESSURE,RECOVER,DEFEND,PUNISH,FOLLOW,SUPER_ATTACK,HESITATE };
 struct Diagnostics { int intent=APPROACH,target=0,observed_tick=0,melee_memory=0,fire_memory=0,super_memory=0,decisions=0,defenses=0,punishes=0,followups=0,hesitations=0,plan=0,response_choices=0;bool resting=false,closing=false; };
 Diagnostics diagnostics{};
 int difficulty=1;
 void reset(int level,uint32_t seed,CaptainTactic captain=CaptainTactic::NONE);
 Input input(const CombatCore& core,int who=1);
};
}
