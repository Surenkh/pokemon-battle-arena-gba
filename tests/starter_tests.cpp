#include "pkmn_practice.h"
#include "pkmn_animation.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
using namespace pkmn;
static int checks=0;
static void check(bool value,const char*message){++checks;if(!value){std::fprintf(stderr,"FAIL starter: %s\n",message);std::exit(1);}}
static void frame(CombatCore&c,int who,Input input={}){c.frame(who?Input{}:input,who?input:Input{});}
static CombatCore setup(int character,int who){CombatCore c;c.reset(who?1:character,who?character:1);c.training=true;c.f[who].last_spend=0;c.f[who].x=120*256;c.f[1-who].x=(who?32:208)*256;return c;}
static int events(const CombatCore&c,EventKind kind){int n=0;for(int i=0;i<c.event_count;++i)n+=c.events[i].kind==kind;return n;}
static void gesture(CombatCore&c,int who,int direction){frame(c,who,{uint16_t(direction),uint16_t(direction)});frame(c,who);frame(c,who,{uint16_t(direction),uint16_t(direction)});}
int main(){
 for(int who=0;who<2;++who)for(int direction:{LEFT,RIGHT}){
  const int sign=direction==RIGHT?1:-1;
  CombatCore c=setup(0,who);const int stamina=c.f[who].sta;
  frame(c,who,{uint16_t(direction),uint16_t(direction)});check(c.f[who].action==NONE,"First direction tap retains normal movement");
  for(int n=0;n<20;++n)frame(c,who,{uint16_t(direction),0});
  check(c.f[who].action==NONE&&c.f[who].sta==stamina,"Holding never spends stamina or starts a dash");
  c=setup(0,who);gesture(c,who,direction);check(c.f[who].action==VOLT_STEP&&events(c,EV_VOLT_STEP)==1,"Second fresh tap starts exactly one dash on its eligible frame");
  check(c.f[who].sta==stamina-12*60&&c.f[who].sp==0,"Volt Step costs twelve stamina and no power");
  check(c.f[who].invuln_until==0,"Volt Step grants no invulnerability");
  const int start=c.f[who].x,end=c.f[who].ends;int moved=0;
  while(c.tick<end){const int old=c.f[who].x;frame(c,who,{uint16_t(direction==LEFT?RIGHT:LEFT),uint16_t(A|B|UP|DOWN|R)});if(c.f[who].x!=old)++moved;check(c.f[who].action==VOLT_STEP,"Attacks, jumps and reversal do not cancel dash commitment");}
  check(c.f[who].x-start==sign*24*256&&moved==6,"Dash has six four-pixel movement ticks and eight recovery ticks");
  frame(c,who);check(c.f[who].action==NONE,"Dash ends at the configured recovery boundary");
  gesture(c,who,direction);check(c.f[who].action==NONE,"Post-dash cooldown rejects another gesture");
  c=setup(0,who);frame(c,who,{uint16_t(direction),uint16_t(direction)});for(int n=0;n<10;++n)frame(c,who);frame(c,who,{uint16_t(direction),uint16_t(direction)});check(c.f[who].action==NONE,"Expired double taps remain ordinary movement");
  c=setup(0,who);c.f[who].sta=12*60-1;gesture(c,who,direction);check(c.f[who].action==NONE&&c.f[who].sta==12*60-1&&events(c,EV_TIRED)==1,"Low stamina rejects the complete dash without partial spend");
  for(Action action:{MELEE,FIRE,SUPER,HITSTUN,BLOCKSTUN,GUARD_BREAK,BLOCK,SQUEEZE}){
   c=setup(0,who);c.f[who].action=action;c.f[who].ends=100;
   if(action==BLOCK){frame(c,who,{uint16_t(L|direction),uint16_t(direction)});frame(c,who,{L,0});frame(c,who,{uint16_t(L|direction),uint16_t(direction)});}
   else gesture(c,who,direction);
   check(c.f[who].action!=VOLT_STEP,"A double tap cannot escape an attack, dodge, held guard or stun");
  }
  c=setup(0,who);c.f[who].y=100*256;gesture(c,who,direction);check(c.f[who].action!=VOLT_STEP,"Airborne double taps cannot dash");
  c=setup(0,who);c.f[who].x=(sign>0?208:32)*256;c.f[1-who].x=(sign>0?32:208)*256;gesture(c,who,direction);for(int n=0;n<20;++n)frame(c,who);check(c.f[who].x==(sign>0?208:32)*256,"Walls clamp dash movement without crossing or teleportation");
  c=setup(0,who);gesture(c,who,direction);auto&shot=c.p[1-who];shot.active=true;shot.owner=1-who;shot.x=c.f[who].x;shot.y=c.f[who].y-18*256;shot.px=shot.x;shot.py=shot.y;shot.damage=8;shot.born=c.tick;shot.vx=sign*512;shot.id=999;frame(c,who);check(c.f[who].action==HITSTUN&&c.f[who].hp==characters[0].hp-8,"Ordinary projectiles hit and interrupt Volt Step");
  c=setup(2,who);const int roll_sta=c.f[who].sta;frame(c,who,{uint16_t(DOWN|direction),DOWN});check(c.f[who].action==SQUEEZE&&c.f[who].shell_roll&&events(c,EV_SHELL_ROLL)==1,"Down plus direction starts one owned Shell Roll");
  check(c.f[who].sta==roll_sta-14*60&&c.f[who].invuln_until==0,"Roll costs six plus eight stamina and grants no invulnerability");
  const int roll_start=c.f[who].x,roll_end=c.f[who].ends;int low_ticks=0,roll_ticks=0;
  while(c.tick<roll_end){const int old=c.f[who].x;frame(c,who,{uint16_t(A|B|UP|DOWN|R|direction),uint16_t(A|B|UP|DOWN|R)});low_ticks+=c.low(c.f[who]);roll_ticks+=c.f[who].x!=old;check(c.f[who].ends==roll_end,"Roll retains original dodge recovery despite repeated requests");}
  check(c.f[who].x-roll_start==sign*20*256&&roll_ticks==10,"Roll moves only ten low ticks for twenty pixels");
  check(low_ticks==24,"Roll preserves all twenty-four active squeeze ticks");frame(c,who);check(!c.f[who].shell_roll&&c.f[who].squeeze_ready==c.tick-1+24,"Original squeeze cooldown starts at the original end");
  c=setup(2,who);frame(c,who,{DOWN,DOWN});check(!c.f[who].shell_roll&&c.f[who].sta==roll_sta-6*60,"Down alone uses the six-stamina stationary dodge");
  c=setup(2,who);c.f[who].sta=14*60-1;frame(c,who,{uint16_t(DOWN|direction),DOWN});check(c.f[who].action==NONE&&c.f[who].sta==14*60-1,"An unaffordable directional roll does not silently spend for a different dodge");
  c=setup(2,who);frame(c,who,{uint16_t(DOWN|direction),DOWN});for(int n=0;n<3;++n)frame(c,who);
  auto&enemy=c.f[1-who];enemy.x=c.f[who].x+sign*16*256;enemy.action=MELEE;enemy.began=c.tick-characters[enemy.character].melee_start;enemy.ends=c.tick+20;enemy.face=-sign;enemy.attack_id=55;
  frame(c,who);check(c.f[who].hp<characters[2].hp&&c.f[who].action==HITSTUN&&!c.f[who].shell_roll,"Melee hits the low roll and immediately interrupts its movement");
  c.reset(0,2);check(c.f[0].tap_direction==0&&c.f[0].step_ready==0&&!c.f[1].shell_roll,"Round reset clears all mobility state");
  c=setup(2,who);c.hitstop=2;const int frozen=c.tick,unspent=c.f[who].sta;frame(c,who,{uint16_t(DOWN|direction),DOWN});
  check(c.tick==frozen&&c.f[who].sta==unspent&&c.f[who].pending==REQUEST_SHELL_ROLL,"Hitstop queues Shell Roll without moving or spending");
  CombatCore opposite=c;opposite.f[who].mobility_direction=-c.f[who].mobility_direction;
  check(c.checksum()!=opposite.checksum(),"Queued roll direction belongs to the deterministic state checksum");
  frame(c,who);frame(c,who);check(c.f[who].shell_roll&&c.f[who].sta==unspent-14*60,"Queued roll consumes its latched direction and cost once after hitstop");
  c=setup(0,who);frame(c,who,{uint16_t(direction),uint16_t(direction)});frame(c,who);c.hitstop=2;frame(c,who,{uint16_t(direction),uint16_t(direction)});
  check(c.f[who].pending==REQUEST_VOLT_STEP&&c.f[who].action==NONE,"Hitstop queues the second direction tap without starting a dash");
  opposite=c;opposite.f[who].mobility_direction=-c.f[who].mobility_direction;check(c.checksum()!=opposite.checksum(),"Queued dash direction belongs to the deterministic checksum");
  frame(c,who);frame(c,who);check(c.f[who].action==VOLT_STEP,"Queued Volt Step starts once after hitstop ends");
 }
 for(int fighter:{0,2}){
  CombatCore c;c.reset(fighter,1);c.training=true;PracticeSession p;p.mode=DUMMY_TECHNIQUE;p.begin_round();p.prepare(c);
  check(PracticeSession::available_modes(fighter)==DUMMY_MODE_COUNT,"Both starter lessons are selectable");
  auto step=[&](Input in=Input{}){c.frame(in,p.input(c));p.observe(c);};
  if(fighter==0){step({RIGHT,RIGHT});step();step({RIGHT,RIGHT});for(int n=0;n<20;++n)step();check(p.completed()&&p.feedback==STEP_COMPLETE,"An actual completed dash teaches Pikachu's lesson");}
  else {for(int n=0;n<85;++n)step();step({uint16_t(DOWN|RIGHT),DOWN});for(int n=0;n<45&&!p.completed();++n)step();check(p.completed()&&p.feedback==ROLL_COMPLETE&&c.f[0].hp==characters[2].hp,"An actual rolling super evasion completes Squirtle's lesson");}
 }
 for(const auto*text:practice_feedback_text)check(std::strlen(text)<=23,"Every coaching prompt fits its native line");
 std::printf("PASS %d starter checks: both sides/directions, held/expired/rejected gestures, exact costs/movement/recovery, walls, hits, resets and real lessons.\n",checks);
}
