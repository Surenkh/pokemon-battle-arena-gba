#include "pkmn_combat_feel.h"
#include <cstdio>
using namespace pkmn;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int count(const CombatCore&c,EventKind kind){int n=0;for(int i=0;i<c.event_count;i++)n+=c.events[i].kind==kind;return n;}
static void projectile(CombatCore&c,bool super=false,int damage=8){auto&b=c.p[super?2:0];b=Projectile{};b.active=true;b.owner=0;b.super=super;b.id=123;b.born=c.tick;b.x=c.f[1].x;b.y=c.f[1].y-18*256;b.vx=1280;b.damage=damage;}
int main(){
 for(int ch=0;ch<6;ch++){
  CombatCore c;c.reset(ch,(ch+1)%6);c.f[0].x=32*256;c.f[1].x=208*256;
  c.frame({B,B},{});CHECK(c.event_count==0);
  for(int n=0;n<characters[ch].fire_start;n++)c.frame({},{});
  CHECK(count(c,EV_SHOT)==1);CHECK(c.events[0].owner==0);CHECK(c.events[0].x==c.p[0].x);CHECK(c.events[0].y==c.p[0].y);
  c.frame({},{});CHECK(count(c,EV_SHOT)==0);
  c.reset(ch,(ch+1)%6);c.f[0].sp=100;c.frame({R,R},{});CHECK(count(c,EV_CHARGE)==1);
  for(int n=0;n<super_startup_ticks;n++)c.frame({},{});
  CHECK(count(c,EV_RELEASE)==1);CHECK(c.events[0].super);CHECK(c.f[0].sp==0);
 }
 {CombatCore c;c.reset(0,1);c.f[0].sta=0;c.frame({B,B},{});CHECK(count(c,EV_TIRED)==1);CHECK(c.f[0].pending==0);c.frame({B,0},{});CHECK(c.event_count==0);}
 {CombatCore c;c.reset(0,1);c.f[1].action=BLOCKSTUN;c.f[1].ends=100;c.f[1].face=-1;c.f[1].sta=600;projectile(c);int hp=c.f[1].hp;c.frame({},{});CHECK(c.f[1].hp==hp);CHECK(count(c,EV_BLOCK)==1);CHECK(count(c,EV_BREAK)==1);CHECK(c.f[1].action==GUARD_BREAK);CHECK(c.hitstop==reactions_block_hitstop_display_ticks);}
 {CombatCore c;c.reset(0,1);projectile(c,true,32);int hp=c.f[1].hp;c.frame({},{L,0});CHECK(c.f[1].hp==hp-32);CHECK(count(c,EV_HIT)==1);CHECK(count(c,EV_BLOCK)==0);CHECK(c.events[0].super);CHECK(c.hitstop==reactions_super_hitstop_display_ticks);
  CombatFeel feel;feel.update(c,true);CHECK(feel.dx>=-3&&feel.dx<=3);int old=c.tick,time=c.remaining;uint32_t frame=c.event_frame;c.frame({},{});CHECK(c.tick==old&&c.remaining==time);CHECK(c.event_frame!=frame);CHECK(c.event_count==0);
  int frozen=feel.display_frame;feel.update(c,false);CHECK(feel.display_frame==frozen&&feel.dx==0&&feel.dy==0);
  for(int n=0;n<30;n++)feel.update(c,true);CHECK(feel.dx==0&&feel.dy==0);CHECK(feel.guard_breaks==0);
 }
 {CombatCore c;c.reset(0,1);c.f[1].hp=1;c.f[0].sp=100;c.f[0].action=SUPER;c.f[0].began=-30;c.f[0].ends=25;c.f[0].attack_id=7;c.f[0].spawned=false;c.f[1].x=c.f[0].x+16*256;
  c.frame({},{});CHECK(c.outcome==1);CHECK(!c.p[2].active);CHECK(count(c,EV_RELEASE)==1&&count(c,EV_HIT)==1);
  for(int i=0;i<c.event_count;i++)if(c.events[i].kind==EV_RELEASE)CHECK(c.events[i].x>0&&c.events[i].y==102*256);
 }
 {CombatCore c;c.reset(0,1);c.f[0].y=120*256-900;c.f[0].vy=400;int stamina=c.f[0].sta;c.frame({UP,UP},{});CHECK(c.f[0].pending==UP);CHECK(c.f[0].vy>0);c.frame({UP,0},{});CHECK(c.grounded(c.f[0]));c.frame({UP,0},{});CHECK(c.f[0].vy<0);CHECK(c.f[0].sta==stamina-jump_stamina_cost*60);CHECK(count(c,EV_LANDING_JUMP)==1);}
 {CombatCore c;c.reset(0,1);c.f[0].y=120*256-2000;c.f[0].vy=400;c.frame({UP,UP},{});CHECK(c.f[0].pending==0);for(int n=0;n<12;n++)c.frame({UP,0},{});CHECK(c.grounded(c.f[0]));CHECK(c.f[0].vy==0);}
 {CombatCore c;c.reset(0,1);c.f[0].y=110*256;c.f[0].vy=-400;c.frame({UP,UP},{});CHECK(c.f[0].pending==0);}
 {CombatCore c;c.reset(0,1);c.f[0].action=FIRE;c.f[0].spawned=true;c.f[0].ends=2;c.frame({A,A},{});CHECK(c.f[0].pending==A);c.frame({},{});c.frame({},{});CHECK(c.f[0].action==MELEE);CHECK(count(c,EV_MELEE)==1);}
 {CombatCore c;c.reset(0,1);c.f[0].x=100*256;c.f[1].x=116*256;c.f[0].action=c.f[1].action=MELEE;c.f[0].ends=c.f[1].ends=100;c.f[0].began=-4;c.f[1].began=-6;c.f[0].attack_id=1;c.f[1].attack_id=2;c.frame({},{});CHECK(count(c,EV_HIT)==2);CHECK(c.events[0].owner!=c.events[1].owner);}
 {CombatCore c;c.reset(0,1);CombatFeel feel;c.f[0].sta=100;CHECK(feel.status(c,0)==1);feel.blocked[0]=10;CHECK(feel.status(c,0)==2);feel.tired[0]=10;CHECK(feel.status(c,0)==3);feel.broken[0]=10;CHECK(feel.status(c,0)==4);}
 // Event recording and presentation must not change combat checksums or RNG.
 for(int a=0;a<6;a++)for(int b=0;b<6;b++){
  CombatCore c,d;c.reset(a,b,55);d.reset(a,b,55);CpuController x,y;x.reset(1,77);y.reset(1,99);CombatFeel feel;
  for(int n=0;n<3600&&!c.outcome;n++){auto i=x.input(c,0),j=y.input(c,1);c.frame(i,j);d.frame(i,j);uint32_t before=c.checksum();feel.update(c,true);CHECK(c.checksum()==before);CHECK(c.checksum()==d.checksum());CHECK(c.event_count<=CombatCore::max_events);CHECK(feel.dx>=-3&&feel.dx<=3&&feel.dy>=-1&&feel.dy<=1);}
 }
 std::printf("PASS %d combat-feedback, pause, resource and input-buffer checks.\n",checks);
}
