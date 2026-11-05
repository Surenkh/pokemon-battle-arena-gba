#include "pkmn_animation.h"
#include "pkmn_round_presentation.h"
#include <cstdio>
#include <cstdlib>

using namespace pkmn;

static int checks=0;
static void check(bool condition,const char* message){
 ++checks;
 if(!condition){std::fprintf(stderr,"FAIL combat feedback: %s\n",message);std::exit(1);}
}
static int events(const CombatCore& core,EventKind kind){
 int count=0;
 for(int i=0;i<core.event_count;++i)if(core.events[i].kind==kind)++count;
 return count;
}

int main(){
 for(int character=0;character<character_count;++character){
  CombatCore core;core.reset(character,character);
  const int hp0=core.f[0].hp,hp1=core.f[1].hp;
  core.frame({B,B},{B,B});
  bool clashed=false;
  for(int tick=0;tick<100&&!clashed;++tick){
   core.frame({},{});
   if(events(core,EV_CLASH)){
    check(events(core,EV_CLASH)==1,"One clash is emitted for one pair of normal shots");
    const CombatEvent* clash=nullptr;
    for(int i=0;i<core.event_count;++i)if(core.events[i].kind==EV_CLASH)clash=&core.events[i];
    check(clash&&clash->owner!=clash->target,"Clash identifies both projectile owners");
    check(clash->x>core.f[0].x&&clash->x<core.f[1].x&&clash->y==102*256,"Clash appears where the projectiles meet");
    check(!core.p[0].active&&!core.p[1].active,"Both normal projectiles still cancel");
    check(core.f[0].hp==hp0&&core.f[1].hp==hp1&&core.f[0].sp==0&&core.f[1].sp==0,"A clash deals no damage or super power");
    clashed=true;
   }
  }
  check(clashed,"Each fighter can produce a real projectile clash");

  core.reset(character,character);
  core.frame({DOWN,DOWN},{});
  int first=-1,last=-1,active=0;
  for(int tick=0;tick<40&&core.f[0].action==SQUEEZE;++tick){
   Pose pose=fighter_pose(core,0);
   if(pose.clip==CLIP_SQUEEZE){if(first<0)first=pose.frame;last=pose.frame;++active;}
   core.frame({},{});
  }
  check(active==squeeze_active_ticks,"Dodge keeps the complete active window");
  check(first>=0&&last>first,"The compact dodge advances through its authored poses");
 }

 for(int winner=0;winner<2;++winner){
  const int loser=1-winner;
  CombatCore core;core.reset(0,1);
  core.f[loser].hp=1;
  auto& shot=core.p[winner+2];
  shot.active=true;shot.super=true;shot.owner=winner;shot.id=44;shot.born=core.tick;
  shot.x=core.f[loser].x;shot.y=core.f[loser].y-18*256;shot.damage=32;
  core.frame({},{});
  check(core.outcome==winner+1&&events(core,EV_SUPER_FINISH)==1,"A real lethal super emits its finish event");
  RoundPresentation round;round.begin(core,false);
  check(round.finish==RoundPresentation::SUPER_FINISH,"The winning super selects its celebration");
  round.begin(core,true);
  check(round.finish==RoundPresentation::NORMAL_FINISH,"Practice retains its quick reset");

  core.reset(0,1);core.f[loser].hp=1;
  auto& attacker=core.f[winner];auto& target=core.f[loser];
  attacker.x=(winner?116:100)*256;target.x=(winner?100:116)*256;attacker.face=winner?-1:1;
  attacker.action=MELEE;attacker.began=-characters[attacker.character].melee_start;attacker.ends=100;attacker.attack_id=55;
  target.action=FIRE;target.began=-characters[target.character].fire_start-3;target.ends=100;target.spawned=true;
  core.frame({},{});
  check(core.outcome==winner+1&&events(core,EV_PUNISH_FINISH)==1,"A lethal recovery punish emits its finish event");
  round.begin(core,false);
  check(round.finish==RoundPresentation::PUNISH_FINISH,"The winning punish selects its celebration");
  core.outcome=3;round.begin(core,false);
  check(round.finish==RoundPresentation::NORMAL_FINISH,"Draws do not claim a winner's finish");
 }

 {
  CombatCore core;core.reset(0,1);
  auto& normal=core.p[0];normal.active=true;normal.owner=0;normal.id=1;normal.born=0;normal.x=120*256;normal.y=102*256;
  auto& super=core.p[3];super.active=true;super.super=true;super.owner=1;super.id=2;super.born=0;super.x=120*256;super.y=102*256;
  core.frame({},{});
  check(!normal.active&&super.active&&events(core,EV_CLASH)==0,"A super still passes through a normal projectile without a mutual-clash cue");
 }
 {
  CombatCore core;core.reset(0,1);core.outcome=1;core.f[1].hp=5;core.remaining=0;
  RoundPresentation round;round.begin(core,false);
  check(round.finish==RoundPresentation::NORMAL_FINISH,"A timeout does not claim a KO finish");
 }
 std::printf("PASS %d combat-feedback checks; all fighters, projectile clashes, dodge poses, both finish types and edge cases.\n",checks);
}
