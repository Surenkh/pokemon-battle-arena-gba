#include "pkmn_combat_core.h"
#include <cassert>
#include <cstdio>
using namespace pkmn;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
static void super_at(CombatCore&c,int target){auto&b=c.p[2];b=Projectile{};b.active=true;b.super=true;b.owner=1-target;b.id=777;b.x=c.f[target].x;b.y=102*256;b.vx=1280;b.damage=32;b.born=c.tick;}
int main(){
 for(int a=0;a<6;a++)for(int b=0;b<6;b++){CombatCore c;c.reset(a,b);CHECK(c.f[0].hp==characters[a].hp);CHECK(c.f[1].sta==characters[b].sta*60);}
 {CombatCore c;c.reset(0,1);c.f[0].sta=1199;c.step({B,B},{});CHECK(c.f[0].action==NONE);CHECK(!c.p[0].reserved);CHECK(c.f[0].sta>=1199);}
 {CombatCore c;c.reset(0,1);c.f[0].sta=0;for(int i=0;i<60;i++)c.step({},{});CHECK(c.f[0].sta==26*60);}
 {CombatCore c;c.reset(0,1);for(int i=0;i<60;i++)c.step({L,0},{});CHECK(c.f[0].sta==(105-12)*60);}
 for(int ch=0;ch<6;ch++)for(int side=0;side<2;side++){
  CombatCore c;c.reset(ch,ch);int hp=c.f[side].hp;super_at(c,side);c.step({},{});CHECK(c.f[side].hp==hp-32);
  c.reset(ch,ch);hp=c.f[side].hp;super_at(c,side);c.step(side?Input{}:Input{L,0},side?Input{L,0}:Input{});CHECK(c.f[side].hp==hp-32);
  c.reset(ch,ch);hp=c.f[side].hp;c.f[side].action=SQUEEZE;c.f[side].began=-3;c.f[side].ends=33;super_at(c,side);c.step({},{});CHECK(c.f[side].hp==hp);
  c.reset(ch,ch);hp=c.f[side].hp;c.f[side].y=96*256;c.f[side].vy=-64;super_at(c,side);c.step({},{});CHECK(c.f[side].hp==hp);
  c.reset(ch,ch);hp=c.f[side].hp;c.f[side].action=SQUEEZE;c.f[side].began=0;c.f[side].ends=36;super_at(c,side);c.step({},{});CHECK(c.f[side].hp==hp-32);
 }
 {CombatCore c;c.reset(0,1);c.f[0].x=100*256;c.f[1].x=116*256;c.step({A,A},{});int hp=c.f[1].hp;for(int i=0;i<15;i++)c.step({},{});CHECK(c.f[1].hp==hp-8);CHECK(c.f[0].sp==18);}
 {CombatCore c;c.reset(0,1);c.f[0].x=100*256;c.f[1].x=116*256;c.step({A,A},{L,0});int hp=c.f[1].hp;for(int i=0;i<8;i++)c.step({},{L,0});CHECK(c.f[1].hp==hp);CHECK(c.f[0].sp==0);}
 {CombatCore c;c.reset(0,1);c.f[0].sp=99;c.step({R,R},{});c.f[0].sp=100;c.step({},{});CHECK(c.f[0].action!=SUPER);}
 {CombatCore c;c.reset(0,1);c.f[0].sp=100;c.step({R,R},{});CHECK(c.f[0].sp==0);CHECK(c.f[0].sta==(105-25)*60);for(int i=0;i<30;i++)c.step({},{});CHECK(c.p[2].active);CHECK(c.p[2].x==78*256);CHECK(c.p[2].born==30);}
 {CombatCore c;c.reset(0,1);c.f[0].sp=100;c.f[0].sta=1499;c.step({R,R},{});CHECK(c.f[0].sp==100);CHECK(c.f[0].action!=SUPER);}
 {CombatCore c;c.reset(0,1);c.p[0].active=true;c.p[0].owner=0;c.p[0].born=0;c.p[0].x=100*256;c.p[0].y=102*256;c.step({B,B},{});CHECK(c.f[0].action!=FIRE);CHECK(c.f[0].sta==105*60);}
 {CombatCore c;c.reset(0,1);c.f[0].y=120*256;c.step({UP,UP},{});int apex=c.f[0].y;for(int i=0;i<40;i++){c.step({},{});if(c.f[0].y<apex)apex=c.f[0].y;}CHECK(apex==86*256);CHECK(c.f[0].y==120*256);}
 {CombatCore c;c.reset(0,1);c.f[0].x=100*256;c.f[1].x=116*256;c.f[0].hp=10;c.f[1].hp=8;c.f[0].action=MELEE;c.f[0].ends=100;c.f[0].began=-4;c.f[0].attack_id=1;c.f[1].action=MELEE;c.f[1].ends=100;c.f[1].began=-6;c.f[1].attack_id=2;c.step({},{});CHECK(c.outcome==3);}
 {CombatCore c;c.reset(0,1);c.remaining=1;c.f[0].hp=95;c.f[1].hp=100;c.step({},{});CHECK(c.outcome==3);}
 {CombatCore c;c.reset(0,1);c.f[0].action=BLOCKSTUN;c.f[0].ends=6;c.f[0].face=1;auto&b=c.p[1];b.active=true;b.owner=1;b.id=1;b.x=c.f[0].x;b.y=102*256;b.vx=-768;b.damage=8;b.born=0;int hp=c.f[0].hp;c.step({},{});CHECK(c.f[0].hp==hp);CHECK(c.f[0].sta==(105-10)*60);}
 {CombatCore c;c.reset(0,1);c.f[0].action=BLOCKSTUN;c.f[0].ends=6;c.f[0].face=1;c.f[0].sta=10*60;auto&b=c.p[1];b.active=true;b.owner=1;b.id=1;b.x=c.f[0].x;b.y=102*256;b.vx=-768;b.damage=8;b.born=0;int hp=c.f[0].hp;c.step({},{});CHECK(c.f[0].hp==hp);CHECK(c.f[0].sta==0);CHECK(c.f[0].action==GUARD_BREAK);}
 {CombatCore c;c.reset(0,1);c.f[0].action=BLOCK;c.step({A,A},{});CHECK(c.f[0].action==MELEE);}
 {CombatCore c;c.reset(0,1);c.hitstop=4;int old=c.tick;c.frame({UP,UP},{});for(int i=0;i<3;i++)c.frame({},{});CHECK(c.tick==old);c.frame({},{});CHECK(c.f[0].vy<0);}
 {CombatCore c;c.reset(0,1);c.f[0].sp=100;c.f[0].action=BLOCK;c.step({uint16_t(L|R|UP),uint16_t(R|UP)},{});CHECK(c.f[0].vy<0);CHECK(c.f[0].sp==100);}
 {CombatCore c;c.reset(0,1);c.p[0].active=true;c.p[0].owner=0;c.p[0].born=0;c.p[0].x=100*256;c.p[0].y=102*256;c.step({uint16_t(B|A),uint16_t(B|A)},{});CHECK(c.f[0].action==MELEE);}
 // Deterministic replay and broad invariant stress across every matchup.
 unsigned rounds=0;for(int a=0;a<6;a++)for(int b=0;b<6;b++)for(int seed=1;seed<=20;seed++){
  CombatCore c,d;c.reset(a,b,seed);d.reset(a,b,seed);CpuController ai0,ai1,bi0,bi1;ai0.reset(1,seed*23);ai1.reset(1,seed*37);bi0.reset(1,seed*23);bi1.reset(1,seed*37);
  for(int t=0;t<6000&&!c.outcome;t++){Input x=ai0.input(c,0),y=ai1.input(c,1),u=bi0.input(d,0),v=bi1.input(d,1);c.frame(x,y);d.frame(u,v);CHECK(c.checksum()==d.checksum());for(int i=0;i<2;i++){CHECK(c.f[i].hp>=0&&c.f[i].hp<=characters[c.f[i].character].hp);CHECK(c.f[i].sta>=0&&c.f[i].sta<=characters[c.f[i].character].sta*60);CHECK(c.f[i].sp>=0&&c.f[i].sp<=100);CHECK(c.f[i].x>=24*256&&c.f[i].x<=216*256);}}
  CHECK(c.outcome>=1&&c.outcome<=3);++rounds;
 }
 std::printf("PASS %d assertions; %u seeded matches; all 36 ordered matchups.\n",checks,rounds);return 0;
}
