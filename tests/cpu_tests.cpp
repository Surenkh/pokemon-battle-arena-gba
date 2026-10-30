#include "pkmn_combat_core.h"
#include <cstdio>
#include <cstring>
using namespace pkmn;
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL cpu:%d %s\n",__LINE__,#x);return 1;}}while(0)
static bool same(Input a,Input b){return a.held==b.held&&a.pressed==b.pressed;}
int main(){
 // Compact, fixed-size perception history stays within a practical GBA budget.
 CHECK(sizeof(CpuController)<6000);
 for(int l=0;l<3;++l){CpuController a;a.reset(l,0);CHECK(a.difficulty==l);CHECK(cpu_difficulties[l].reaction_ticks>0);}
 {CpuController a;a.reset(-1,1);CHECK(a.difficulty==0);a.reset(7,1);CHECK(a.difficulty==2);}
 // Changing the current visible action cannot alter a decision before its
 // observation delay has elapsed. Identical own resources, positions and seed.
 for(int l=0;l<3;++l)for(int seed=1;seed<=50;++seed){
  CombatCore c,d;c.reset(1,2);d=c;CpuController a,b;a.reset(l,seed);b.reset(l,seed);
  CHECK(same(a.input(c),b.input(d)));
  for(int t=1;t<=cpu_difficulties[l].reaction_ticks;++t){
   c.tick=d.tick=t;c.f[0].action=SUPER;c.f[0].began=1;c.f[0].attack_id=77;
   CHECK(same(a.input(c),b.input(d)));CHECK(a.diagnostics.observed_tick<t);
  }
 }
 // Hidden opponent state is deliberately absent from the stored observation.
 // Perturb it on every tick; both policies must still return identical inputs.
 for(int l=0;l<3;++l)for(int who=0;who<2;++who)for(int ch=0;ch<6;++ch){
  CombatCore c,d;c.reset(ch,(ch+1)%6);d=c;CpuController a,b;a.reset(l,71+ch);b.reset(l,71+ch);
  for(int t=0;t<600;++t){
   c.tick=d.tick=t;
   auto&e=c.f[1-who];e.action=Action((t/40)%6);e.attack_id=unsigned(t/40+1);e.began=(t/40)*40;
   d.f[1-who]=e;auto&hidden=d.f[1-who];
   hidden.pending=R|UP;hidden.pending_until=t+40;hidden.invuln_until=t+90;
   hidden.ends=t+999;hidden.sta=0;hidden.sp=100;hidden.hp=1;hidden.squeeze_ready=t+1000;
   hidden.confirmed_at=t;hidden.chain_depth=1;hidden.last_spend=t;hidden.melee_contact=true;hidden.spawned=true;
   // Reservation, damage and birth tick are hidden; the visible trajectory is identical.
   c.p[1-who].active=d.p[1-who].active=(t%90)<30;c.p[1-who].owner=d.p[1-who].owner=1-who;
   c.p[1-who].id=d.p[1-who].id=unsigned(t/90+40);c.p[1-who].x=d.p[1-who].x=120*256;
   c.p[1-who].y=d.p[1-who].y=102*256;c.p[1-who].vx=d.p[1-who].vx=who?-768:768;
   d.p[1-who].reserved=true;d.p[1-who].damage=999;d.p[1-who].born=t+400;
   auto before=c.checksum(),other=d.checksum();Input x=a.input(c,who),y=b.input(d,who);
   CHECK(same(x,y));CHECK(c.checksum()==before&&d.checksum()==other);
   CHECK(same(x,a.input(c,who))); // Queries do not re-roll decisions.
   CHECK((x.pressed&~(A|B|R|UP|DOWN))==0);CHECK((x.held&~(A|B|R|UP|DOWN|L|LEFT|RIGHT))==0);
   CHECK(a.diagnostics.melee_memory+a.diagnostics.fire_memory+a.diagnostics.super_memory<=cpu_memory_actions);
  }
 }
 // Repetition is learned from delayed attack starts, bounded to six actions,
 // forgotten after inactivity, and reset between rounds.
 for(int l=0;l<3;++l){
  CombatCore c;c.reset(0,2);CpuController a;a.reset(l,13);
  for(int t=0;t<260;++t){c.tick=t;c.f[0].action=FIRE;c.f[0].began=t/30*30;c.f[0].attack_id=unsigned(t/30+1);a.input(c);}
  CHECK(a.diagnostics.fire_memory==6);
  c.f[0].action=NONE;
  for(int t=260;t<560;++t){c.tick=t;a.input(c);}
  CHECK(a.diagnostics.fire_memory==0);a.reset(l,13);CHECK(a.diagnostics.decisions==0&&a.diagnostics.fire_memory==0);
 }
 // Simultaneous normal/super shots can alternate as the nearest threat. Their
 // independent choices survive that ordering change; no per-frame retry bonus.
 for(int level=0;level<3;++level){CombatCore c;c.reset(0,2);CpuController a;a.reset(level,71);
  c.f[1].action=HITSTUN; // Observe requests without applying them to the fixture.
  for(int t=0;t<120;++t){c.tick=t;
   for(int i=0;i<2;++i){auto&b=c.p[2*i];b.active=true;b.super=i;b.owner=0;b.id=100+i;b.vx=1280;b.y=102*256;
    b.x=c.f[1].x-(((t/12+i)%2)?24:100)*256;}
   a.input(c);
  }CHECK(a.diagnostics.response_choices==2);
 }
 // A low-resource CPU cannot be baited into endless retreat. It reserves stamina,
 // takes at most 24 ticks to disengage, then stays put until its resume threshold.
 for(int l=0;l<3;++l)for(int ch=0;ch<6;++ch){
  CombatCore c;c.reset(ch,ch);c.f[1].sta=10*60;CpuController a;a.reset(l,51);
  for(int t=0;t<100;++t){c.tick=t;Input x=a.input(c);CHECK(!x.pressed);if(t>24)CHECK(!(x.held&(LEFT|RIGHT|L)));}
  CHECK(a.diagnostics.resting);c.tick=102;c.f[1].sta=cpu_styles[ch].resume_at*60;a.input(c);CHECK(!a.diagnostics.resting);
 }
 // Real-button super defense at reachable arrival times on both sides. Not a
 // guaranteed dodge: there are intentional missed reactions and insufficient resources.
 for(int side=0;side<2;++side){int responses=0;
  for(int seed=1;seed<=100;++seed){CombatCore c;c.reset(1,2);c.f[side].x=(side?176:64)*256;c.f[1-side].x=(side?64:176)*256;
   c.f[1-side].action=SUPER;c.f[1-side].began=0;c.f[1-side].attack_id=33;CpuController a;a.reset(2,seed);bool acted=false;
   for(int t=0;t<=40;++t){c.tick=t;Input x=a.input(c,side);CHECK(!(x.held&L));acted|=bool(x.pressed&(UP|DOWN));}
   responses+=acted;
  }CHECK(responses>60&&responses<100);
 }
 // Stalled zoning and the last ten seconds cause an approach, rather than a
 // permanent projectile/block loop. These use own HP/SP and the visible timer.
 for(int ch=0;ch<6;++ch){CombatCore c;c.reset(0,ch);CpuController a;a.reset(2,18);
  for(int t=0;t<=320;++t){c.tick=t;a.input(c);}
  CHECK(a.diagnostics.closing&&a.diagnostics.target==24);
  a.reset(2,18);c.tick=0;c.remaining=599;a.input(c);CHECK(a.diagnostics.closing);
 }
 // Matched deterministic full games, every difficulty, both sides, all fighters.
 int requests[3]={},mistakes[3]={},decisions[3]={};unsigned rounds=0;
 for(int l=0;l<3;++l)for(int ch=0;ch<6;++ch)for(int seed=1;seed<=8;++seed){
  CombatCore c,d;c.reset(ch,(ch+seed)%6,seed);d=c;CpuController a,b,u,v;
  a.reset(l,seed*23);b.reset(l,seed*37);u.reset(l,seed*23);v.reset(l,seed*37);
  for(int n=0;n<7000&&!c.outcome;++n){Input x=a.input(c,0),y=b.input(c,1);CHECK(same(x,u.input(d,0)));CHECK(same(y,v.input(d,1)));c.frame(x,y);d.frame(x,y);CHECK(c.checksum()==d.checksum());
   CHECK(c.event_count<=CombatCore::max_events);for(int i=0;i<2;++i){CHECK(c.f[i].sta>=0);CHECK(c.f[i].sp>=0&&c.f[i].sp<=100);}
  }CHECK(c.outcome);++rounds;requests[l]+=a.diagnostics.defenses+b.diagnostics.defenses;mistakes[l]+=a.diagnostics.hesitations+b.diagnostics.hesitations;decisions[l]+=a.diagnostics.decisions+b.diagnostics.decisions;
 }
 CHECK(decisions[0]<decisions[1]&&decisions[1]<decisions[2]);CHECK(requests[0]<requests[2]);
 CHECK(double(mistakes[0])/decisions[0]>double(mistakes[1])/decisions[1]);CHECK(double(mistakes[1])/decisions[1]>double(mistakes[2])/decisions[2]);
 std::printf("PASS %u CPU assertions; %u deterministic rounds; public perception, reaction delays, bounded memory, recovery, difficulty and replay. CPU bytes=%zu\n",checks,rounds,sizeof(CpuController));
}
