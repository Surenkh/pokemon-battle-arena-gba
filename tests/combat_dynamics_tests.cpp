#include "pkmn_combat_core.h"
#include <cstdio>
#include <cstdlib>

using namespace pkmn;
static int checks=0;
static void check(bool value,const char* message){
 ++checks;
 if(!value){std::fprintf(stderr,"FAIL combat dynamics: %s\n",message);std::exit(1);}
}
static int events(const CombatCore& core,EventKind kind){
 int count=0;
 for(int i=0;i<core.event_count;++i)if(core.events[i].kind==kind)++count;
 return count;
}
static void frame(CombatCore& core,int who,Input input={},Input opponent={}){
 core.frame(who?opponent:input,who?input:opponent);
}
static void shot(CombatCore& core,int slot,int owner,int x,int velocity,int damage,uint32_t id,bool super=false,int burst=-1){
 auto& projectile=core.p[slot];projectile=Projectile{};
 projectile.active=true;projectile.owner=owner;projectile.x=x*256;projectile.y=102*256;
 projectile.vx=velocity;projectile.damage=damage;projectile.id=id;projectile.super=super;
 projectile.born=core.tick;projectile.burst_at=burst;
}

int main(){
 for(int a=0;a<character_count;++a)for(int b=0;b<character_count;++b){
  CombatCore core;core.reset(a,b);int hp[2]={core.f[0].hp,core.f[1].hp};
  core.frame({B,B},{B,B});bool clash=false;
  for(int i=0;i<100&&!clash;++i){
   core.frame({},{});
   if(events(core,EV_CLASH)){
    check(events(core,EV_CLASH)==1,"One event for a projectile pair");
    check(core.p[0].active&&core.p[1].active,"Normal shots continue after colliding");
    check(core.p[0].damage==(characters[a].fire_damage+1)/2&&core.p[1].damage==(characters[b].fire_damage+1)/2,"Both shots retain half damage rounded up");
    check(core.f[0].hp==hp[0]&&core.f[1].hp==hp[1],"The collision itself causes no fighter damage");
    clash=true;
   }
  }
  check(clash,"Real opposing attacks clash for every ordered matchup");
  for(int i=0;i<100&&core.f[0].hp==hp[0]&&core.f[1].hp==hp[1];++i){
   core.frame({},{});check(!events(core,EV_CLASH),"A touching pair does not repeatedly clash");
  }
  check(core.f[0].hp<hp[0]||core.f[1].hp<hp[1],"At least one weakened shot can hit after a clash");
 }

 {
  CombatCore core;core.reset(0,1);
  shot(core,0,0,120,0,1,1);shot(core,1,1,120,0,1,2);
  core.frame({},{});check(core.p[0].damage==1&&core.p[1].damage==1,"Repeated weakening has a minimum of one damage");
  for(int i=0;i<5;++i){core.frame({},{});check(!events(core,EV_CLASH),"Stationary overlap only clashes once");}
  shot(core,1,1,120,0,14,3);core.frame({},{});
  check(events(core,EV_CLASH)==1&&core.p[0].damage==1&&core.p[1].damage==7,"A new opposing shot can clash again");
 }
 {
  CombatCore core;core.reset(0,1);int hp0=core.f[0].hp,hp1=core.f[1].hp;
  shot(core,0,0,115,512,12,1);shot(core,1,1,125,-512,14,2);
  int clashes=0;
  for(int i=0;i<100&&(core.f[0].hp==hp0||core.f[1].hp==hp1);++i){core.frame({},{});clashes+=events(core,EV_CLASH);}
  check(clashes==1&&core.f[0].hp==hp0-7&&core.f[1].hp==hp1-6,"Both continuing shots deal their reduced damage exactly once");
 }
 {
  CombatCore core;core.reset(0,10);
  shot(core,0,0,120,0,12,1);shot(core,1,1,120,0,10,2,false,0);
  core.frame({},{});check(!core.p[0].active&&!core.p[1].active,"An armed Shadow Orb is still destroyed by a normal shot");
  core.reset(0,1);shot(core,0,0,120,0,12,1);shot(core,3,1,120,0,32,2,true);
  core.frame({},{});check(!core.p[0].active&&core.p[3].active,"A super still overcomes a normal shot");
  core.reset(0,1);shot(core,2,0,120,0,32,1,true);shot(core,3,1,120,0,32,2,true);
  core.frame({},{});check(core.p[2].active&&core.p[3].active,"Opposing supers pass through each other");
 }

 for(int character=0;character<character_count;++character)for(int who=0;who<2;++who){
  CombatCore core;core.reset(who?0:character,who?character:0);int stamina=core.f[who].sta;
  frame(core,who,{UP,UP});check(core.f[who].sta==stamina-10*60,"Jump costs ten stamina");
  core.reset(who?0:character,who?character:0);stamina=core.f[who].sta;
  frame(core,who,{DOWN,DOWN});check(core.f[who].sta==stamina-6*60,"Stationary squeeze costs six stamina");
  core.reset(who?0:character,who?character:0);stamina=core.f[who].sta;
  int away=who?RIGHT:LEFT;int start=core.f[who].x;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].action==BACKSTEP&&events(core,EV_BACKSTEP)==1,"Block plus a fresh away press starts backstep");
  check(core.f[who].sta==stamina-6*60&&core.f[who].invuln_until==0,"Backstep spends six stamina and adds no invulnerability");
  for(int i=0;i<backstep_startup_ticks+backstep_travel_ticks+backstep_recovery_ticks;++i)frame(core,who);
  check((core.f[who].x-start)*(who?1:-1)==backstep_speed_q8*backstep_travel_ticks,"Backstep travels fifteen pixels away");
  int remaining=core.f[who].sta;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].action!=BACKSTEP&&core.f[who].sta>=remaining-12,"Cooldown prevents another backstep");
  while(core.tick<core.f[who].backstep_ready)frame(core,who);
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].action==BACKSTEP,"A fresh press works after the cooldown");

  core.reset(who?0:character,who?character:0);core.f[who].action=BLOCKSTUN;core.f[who].ends=2;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});check(core.f[who].action==BLOCKSTUN,"Backstep does not cancel blockstun");
  frame(core,who,{L,0});frame(core,who,{L,0});
  check(core.f[who].action==BACKSTEP,"A late backstep press is buffered until blockstun ends");

  core.reset(who?0:character,who?character:0);core.f[who].action=BLOCKSTUN;core.f[who].ends=2;core.hitstop=2;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].pending==REQUEST_BACKSTEP&&core.f[who].sta==characters[character].sta*60,"Hitstop queues the backstep without spending");
  CombatCore opposite=core;opposite.f[who].mobility_direction=-core.f[who].mobility_direction;
  check(core.checksum()!=opposite.checksum(),"Queued backstep direction belongs to the replay checksum");
  for(int i=0;i<4;++i)frame(core,who,{L,0});
  check(core.f[who].action==BACKSTEP,"A queued backstep survives hitstop and starts after blockstun");

  core.reset(who?0:character,who?character:0);core.f[who].x=(who?world_max_fighter_x:world_min_fighter_x)*256;stamina=core.f[who].sta;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].action!=BACKSTEP&&core.f[who].sta>=stamina-12,"A blocked wall does not spend the backstep cost");

  core.reset(who?0:character,who?character:0);core.f[who].sta=backstep_stamina_cost*60-1;
  frame(core,who,{uint16_t(L|away),uint16_t(away)});
  check(core.f[who].action!=BACKSTEP&&events(core,EV_TIRED)==1,"Insufficient stamina rejects the retreat");

  core.reset(who?0:character,who?character:0);
  frame(core,who,{uint16_t(L|away|UP),uint16_t(away|UP)});
  check(core.f[who].vy<0&&core.f[who].action!=BACKSTEP,"A simultaneous jump keeps its existing priority");

  core.reset(who?0:character,who?character:0);frame(core,who,{uint16_t(L|away),uint16_t(away)});
  shot(core,1-who,1-who,core.f[who].x/256,who?512:-512,8,77);
  int hp=core.f[who].hp;frame(core,who);
  check(core.f[who].hp==hp-8&&core.f[who].action==HITSTUN,"An attack can interrupt the backstep");
 }
 for(int who=0;who<2;++who){
  int requested=0,executed=0;
  for(int seed=1;seed<=40;++seed){
   CombatCore core;core.reset(0,1);core.f[0].x=100*256;core.f[1].x=120*256;
   core.f[who].action=BLOCKSTUN;core.f[who].ends=2;
   CpuController cpu;cpu.reset(1,seed);
   for(int tick=0;tick<5;++tick){
    Input input=cpu.input(core,who);frame(core,who,input);
    if(core.f[who].action==BACKSTEP){++executed;break;}
   }
   requested+=cpu.diagnostics.backsteps>0;
  }
  check(requested>0&&executed==requested,"The CPU can request and execute a legal backstep on both sides");
 }

 for(int who=0;who<2;++who)for(int kind=0;kind<3;++kind){
  CombatCore core;core.reset(who?1:0,who?0:1);
  auto& attacker=core.f[who];auto& defender=core.f[1-who];
  attacker.x=(who?116:100)*256;defender.x=(who?100:116)*256;
  if(kind==2){attacker.x=(who?176:64)*256;defender.x=(who?64:176)*256;}
  frame(core,who,{A,A},kind==1?Input{L,0}:Input{});
  int original_end=attacker.ends;bool blocked=false;
  for(int i=0;i<40&&attacker.action==MELEE;++i){
   int before_defender=defender.x,before_attacker=attacker.x;
   frame(core,who,{},kind==1?Input{L,0}:Input{});
   if(events(core,EV_BLOCK)){
    blocked=true;
    check((defender.x-before_defender)*(who?-1:1)==combat_dynamics_blocked_melee_pushback_px*256,"A blocked melee pushes the defender away");
    check(attacker.x==before_attacker,"The attacker stays in place when the defender has room");
   }
   if(attacker.melee_penalized)break;
  }
  check(attacker.ends==original_end+(kind?4:0),"Only blocked and missed melee gains recovery");
  if(kind==1)check(blocked,"The guarded attack genuinely connected");
 }
 for(int who=0;who<2;++who){
  CombatCore core;core.reset(0,1);auto& attacker=core.f[who];auto& defender=core.f[1-who];
  defender.x=(who?world_min_fighter_x:world_max_fighter_x)*256;
  attacker.x=defender.x+(who?16:-16)*256;
  frame(core,who,{A,A},{L,0});bool blocked=false;
  for(int i=0;i<20&&!blocked;++i){
   int before=attacker.x;frame(core,who,{},Input{L,0});
   if(events(core,EV_BLOCK)){
    check(defender.x==(who?world_min_fighter_x:world_max_fighter_x)*256,"Defender stays within the wall");
    check((attacker.x-before)*(who?1:-1)>=combat_dynamics_blocked_melee_pushback_px*256,"Wall pressure pushes the attacker away");
    blocked=true;
   }
  }
  check(blocked,"A wall block is reached on each side");
 }
 std::printf("PASS %d combat-dynamics checks; every matchup, both seats, clashes, costs, retreat, guard and recovery.\n",checks);
}
