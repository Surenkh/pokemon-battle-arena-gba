#include "pkmn_practice.h"
#include "pkmn_combat_feel.h"
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
using namespace pkmn;
static int checks=0;
static void check(bool condition,const char* message){++checks;if(!condition){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
static int events(const CombatCore& c,EventKind kind,int owner=-1){int count=0;for(int n=0;n<c.event_count;n++)if(c.events[n].kind==kind&&(owner<0||c.events[n].owner==owner))++count;return count;}
static void frame(CombatCore&c,int who,Input input={},Input enemy={}){c.frame(who?enemy:input,who?input:enemy);}
static CombatCore setup(int character,int who=0){CombatCore c;c.reset(who?0:character,who?character:0);c.training=true;c.f[who].x=(who?208:32)*256;c.f[1-who].x=(who?32:208)*256;return c;}
static void descend(CombatCore&c,int who){frame(c,who,{UP,UP});for(int n=0;n<100&&!c.can_air_brake(who);n++)frame(c,who);check(c.can_air_brake(who),"A real jump reaches a usable descent");}
static void cast(CombatCore&c,int who){frame(c,who,{B,B});for(int n=0;n<40&&!c.p[who].active;n++)frame(c,who);check(c.p[who].active,"A real B input releases the normal orb");}
int main(){
 for(int who=0;who<2;who++){
  CombatCore c=setup(7,who);descend(c,who);CombatCore ordinary=c;
  const int stamina=c.f[who].sta,peak=c.f[who].y,power=c.f[who].sp;
  frame(c,who,{UP,UP});check(events(c,EV_AIR_BRAKE,who)==1,"Fresh Up starts Air Brake on the first eligible step");
  check(c.f[who].sta==stamina-8*60&&c.f[who].sp==power,"Air Brake costs exactly 8 stamina and no SP");
  check(c.f[who].vy>=0&&c.f[who].vy<=128,"Brake slows descent without adding upward velocity");
  check(c.f[who].action==NONE&&c.f[who].invuln_until==0,"Air Brake adds neither recovery cancel nor invulnerability");
  int braked_landing=0,ordinary_landing=0,activations=1;
  for(int n=1;n<110;n++){
   frame(c,who,{UP,0});frame(ordinary,who);
   activations+=events(c,EV_AIR_BRAKE,who);
   check(c.f[who].y>=peak,"Air Brake never grants extra jump height");
   if(!braked_landing&&c.grounded(c.f[who]))braked_landing=n;
   if(!ordinary_landing&&ordinary.grounded(ordinary.f[who]))ordinary_landing=n;
   if(braked_landing&&ordinary_landing)break;
  }
  check(activations==1&&braked_landing>ordinary_landing,"Held Up cannot repeat; the technique changes actual landing timing");
  check(!c.f[who].air_brake_used&&c.f[who].air_brake_until==0,"Landing restores exactly one use for the next jump");
  descend(c,who);frame(c,who,{UP,UP});check(events(c,EV_AIR_BRAKE,who)==1,"A new jump permits a new brake");
  frame(c,who);const int last_spend=c.f[who].last_spend;frame(c,who,{UP,UP});
  check(!events(c,EV_AIR_BRAKE)&&c.f[who].last_spend==last_spend,"A second fresh press in one jump cannot spend or extend the brake");
  c=setup(7,who);descend(c,who);const int start=c.tick;frame(c,who,{UP,UP});
  for(int n=1;n<air_brake_duration_ticks;n++){frame(c,who);check(c.f[who].vy<=128,"Fall-speed cap lasts for all twelve simulation steps");}
  check(c.tick==start+12,"Air Brake duration is twelve simulation steps");frame(c,who);check(c.f[who].vy>128,"Gravity resumes immediately after the cap");
  c=setup(7,who);descend(c,who);c.f[who].sta=8*60-1;frame(c,who,{UP,UP});
  check(!c.f[who].air_brake_used&&events(c,EV_TIRED,who)==1,"Unaffordable Air Brake gives rejection feedback without partial activation");
  c=setup(7,who);descend(c,who);c.hitstop=2;const int frozen=c.tick;frame(c,who,{UP,UP});check(c.f[who].pending==REQUEST_AIR_BRAKE&&!c.f[who].air_brake_used,"Hitstop queues rather than executing the technique");
  frame(c,who);check(c.tick==frozen,"Technique input cannot advance hitstop simulation");frame(c,who);check(c.f[who].air_brake_used,"Queued brake executes once when hitstop ends");
  for(Action action:{MELEE,FIRE,SUPER,HITSTUN,BLOCKSTUN,GUARD_BREAK}){
   c=setup(7,who);descend(c,who);c.f[who].action=action;c.f[who].ends=c.tick+100;
   frame(c,who,{UP,UP});check(!c.f[who].air_brake_used,"Air Brake cannot cancel an attack or stun");
  }
  c=setup(7,who);c.f[who].y=120*256-900;c.f[who].vy=400;frame(c,who,{UP,UP});
  check(c.f[who].pending==UP&&!c.f[who].air_brake_used,"Existing near-landing jump buffering wins over Air Brake");
  for(int n=0;n<3&&c.f[who].vy>=0;n++)frame(c,who);
  check(c.f[who].vy<0,"Buffered landing jump still launches normally");
  c=setup(7,who);descend(c,who);frame(c,who,{UP,UP});
  auto& shot=c.p[1-who];shot.active=true;shot.owner=1-who;shot.x=c.f[who].x;shot.y=c.f[who].y-18*256;shot.px=shot.x;shot.py=shot.y;shot.damage=8;shot.born=c.tick;shot.vx=who?512:-512;shot.id=123;
  frame(c,who);check(c.f[who].action==HITSTUN&&c.f[who].air_brake_until==0,"An ordinary hit interrupts the brake");
  check(c.f[who].air_brake_used,"Being hit does not refund an aerial technique use");
  c=setup(10,who);cast(c,who);CombatCore held=c;for(int n=0;n<20;n++)frame(held,who,{B,0});check(held.p[who].burst_at<0,"Holding the original B never arms an orb");
  const int end=c.f[who].ends,began=c.f[who].began,sta=c.f[who].sta,position=c.p[who].x;const auto id=c.p[who].id;
  frame(c,who,{B,B});check(events(c,EV_ORB_ARM,who)==1&&c.p[who].burst_at==c.tick-1,"Fresh B arms the live orb on its first eligible step");
  check(c.f[who].sta==sta-8*60&&c.p[who].vx==0&&c.p[who].x==position,"Burst pays 8 extra stamina and fixes the orb's position");
  check(c.f[who].ends==end&&c.f[who].began==began&&c.f[who].action==FIRE,"Remote input does not cancel or shorten firing recovery");
  check(c.p[who].id==id&&c.p[who].damage==10,"The same single-hit orb becomes a lower-damage burst");
  const int armed=c.p[who].burst_at,enemy=1-who,sign=who?-1:1;
  c.f[enemy].x=c.p[who].x+sign*17*256;const int hp=c.f[enemy].hp;int phases=1<<c.burst_phase(c.p[who]);
  for(int n=1;n<shadow_burst_startup_ticks;n++){frame(c,who,{B,B});check(c.f[enemy].hp==hp,"The complete warning is harmless to fighters");check(c.p[who].burst_at==armed,"Repeated presses cannot restart or extend the warning");phases|=1<<c.burst_phase(c.p[who]);}
  check((phases&15)==15,"All four distinct arming frames are reachable");
  frame(c,who);check(c.f[enemy].hp==hp-10&&events(c,EV_BURST_HIT,who)==1,"Damage begins on the exact first active tick");
  check(!c.p[who].active&&c.f[who].sp==22,"Burst consumes its one hit and grants only ordinary fire SP");
  for(int n=0;n<40;n++)frame(c,who);check(c.f[enemy].hp==hp-10,"The blast cannot hit repeatedly");
  c=setup(10,who);cast(c,who);frame(c,who,{B,B});phases=1<<c.burst_phase(c.p[who]);
  for(int n=0;n<shadow_burst_startup_ticks+shadow_burst_active_ticks;n++){frame(c,who);if(c.p[who].active)phases|=1<<c.burst_phase(c.p[who]);}
  check(phases==255&&!c.p[who].active,"A whiff displays all eight phases and expires after its bounded active window");
  for(int defense=0;defense<4;defense++){
   c=setup(10,who);cast(c,who);frame(c,who,{B,B});c.f[enemy].x=c.p[who].x+sign*17*256;const int life=c.f[enemy].hp;
   for(int n=0;n<25;n++){
    Input input=defense==0?Input{L,0}:defense==1?Input{uint16_t(UP),uint16_t(n==0?UP:0)}:defense==2?Input{uint16_t(sign>0?RIGHT:LEFT),0}:Input{B,uint16_t(n==0?B:0)};
    frame(c,who,{},input);
   }
   char label[128];std::snprintf(label,sizeof(label),"Counter %d on side %d prevents burst damage (%d/%d HP)",defense,who,c.f[enemy].hp,life);check(c.f[enemy].hp==life,label);
  }
  c=setup(10,who);cast(c,who);frame(c,who,{B,B});c.f[enemy].x=c.p[who].x+sign*17*256;
  const int crouched_life=c.f[enemy].hp;
  for(int n=0;n<25;n++)frame(c,who,{},Input{DOWN,uint16_t(n==0?DOWN:0)});
  check(c.f[enemy].hp==crouched_life-10,"Wide burst catches squeeze; its advertised counters are block, jump, movement and shots");
  for(int side:{-1,1}){
   c=setup(10,who);cast(c,who);c.p[who].x=120*256;c.f[enemy].x=(120+side*17)*256;
   frame(c,who,{B,B});const int life=c.f[enemy].hp;
   for(int n=0;n<25;n++)frame(c,who,{},Input{L,0});
   check(c.f[enemy].hp==life,"A grounded guard blocks a stationary burst on either side of its center");
  }
  for(Action action:{MELEE,SUPER,BLOCK,HITSTUN,BLOCKSTUN,GUARD_BREAK}){
   c=setup(10,who);cast(c,who);c.f[who].action=action;c.f[who].ends=c.tick+100;
   frame(c,who,{uint16_t(B|(action==BLOCK?L:0)),B});check(c.p[who].burst_at<0,"Orb arming cannot cancel an unrelated commitment or stun");
  }
  c=setup(10,who);cast(c,who);c.f[who].sta=8*60-1;frame(c,who,{B,B});
  check(c.p[who].burst_at<0&&events(c,EV_TIRED,who)==1,"Unaffordable burst leaves the original traveling orb intact");
  c=setup(10,who);cast(c,who);c.hitstop=2;frame(c,who,{B,B});check(c.f[who].pending==REQUEST_ORB_BURST&&c.p[who].burst_at<0,"Burst queues during hitstop without changing the projectile");
  frame(c,who);frame(c,who);check(c.p[who].burst_at>=0,"Queued burst executes once after hitstop");
  c.reset(7,10);check(!c.f[0].air_brake_used&&c.f[0].air_brake_until==0&&!c.p[0].active&&c.p[0].burst_at==-1,"Round reset clears all technique state");
 }
 for(int fighter=0;fighter<character_count;fighter++){
  check(PracticeSession::available_modes(fighter)==(fighter==0||fighter==2||fighter==7||fighter==10?DUMMY_MODE_COUNT:DUMMY_MODE_COUNT-1),"Signature training appears for its four fighters and Breakaway is shared");
  if(fighter==0||fighter==2)continue;
  PracticeSession practice;practice.mode=DUMMY_TECHNIQUE;CombatCore c=setup(fighter);practice.begin_round();practice.prepare(c);
  if(fighter!=7&&fighter!=10){check(practice.mode==DUMMY_IDLE,"Changing to another fighter safely restores Free Practice");continue;}
  auto tick=[&](Input input=Input{}){c.frame(input,practice.input(c));practice.observe(c);};
  if(fighter==7){tick({UP,UP});for(int n=0;n<100&&!c.can_air_brake(0);n++)tick();tick({UP,UP});for(int n=0;n<100&&!practice.completed();n++)tick();check(practice.completed()&&practice.feedback==BRAKE_COMPLETE,"A real brake and landing complete Jigglypuff's drill");}
  else{tick({B,B});for(int n=0;n<100&&(!c.p[0].active||c.f[1].x-c.p[0].x>17*256);n++)tick();tick({B,B});for(int n=0;n<30&&!practice.completed();n++)tick();check(practice.completed()&&practice.feedback==ORB_COMPLETE,"An actual burst hit completes Gastly's drill");}
 }
 std::printf("PASS %d technique checks: two sides, exact costs/timing, counterplay, interruptions, hitstop, resets and real drill completion.\n",checks);
}
