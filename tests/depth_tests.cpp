#include "pkmn_practice.h"
#include "pkmn_combat_feel.h"
#include <cstdio>
#include <initializer_list>
using namespace pkmn;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void frame(CombatCore&c,int who,Input input={},Input other={}){c.frame(who?other:input,who?input:other);}
static int events(const CombatCore&c,EventKind kind){int n=0;for(int i=0;i<c.event_count;++i)n+=c.events[i].kind==kind;return n;}
static void shot(CombatCore&c,int who,int damage,bool super=false){
 auto&b=c.p[who+(super?2:0)];b=Projectile{};b.active=true;b.owner=who;b.super=super;b.id=77;b.born=c.tick;b.damage=damage;
 b.x=c.f[1-who].x;b.y=c.f[1-who].y-18*256;b.vx=(who?-1:1)*1280;
}
static void pair(CombatCore&c,int character,int who){c.reset(who?0:character,who?character:1);c.f[0].x=100*256;c.f[1].x=116*256;}

int main(){
 // Grounded Quick Step moves exactly during startup, on either side.
 for(int ch=0;ch<6;++ch)for(int who=0;who<2;++who){
  CombatCore c;pair(c,ch,who);c.f[0].x=64*256;c.f[1].x=176*256;int start=c.f[who].x;
  frame(c,who,{A,A});CHECK(c.f[who].vx==(who?-1:1)*characters[ch].melee_lunge);
  for(int n=1;n<characters[ch].melee_start;++n)frame(c,who);
  CHECK(c.f[who].x==start+(who?-1:1)*characters[ch].melee_lunge*characters[ch].melee_start);
  frame(c,who);CHECK(c.f[who].vx==0);
  c.reset(ch,1);c.f[0].y=110*256;c.f[0].vy=-256;c.f[0].vx=200;frame(c,0,{A,A});CHECK(c.f[0].vx==200);
 }
 // Vine reach includes 38 px, excludes the exact strict-overlap edge at 39.
 for(int ch=0;ch<6;++ch)for(int who=0;who<2;++who)for(int gap:{34,38,39}){
  CombatCore c;pair(c,ch,who);c.f[1].x=(100+gap)*256;auto&v=c.f[who];v.action=MELEE;v.began=-characters[ch].melee_start;v.ends=100;v.attack_id=12;
  int hp=c.f[1-who].hp;frame(c,who);
  CHECK(c.f[1-who].hp==hp-((ch==3&&gap<39)?characters[ch].melee_damage:0));
 }
 // Explicit guard thresholds include pressure and Shell Guard on both sides.
 for(int who=0;who<2;++who)for(int shell=0;shell<2;++shell)for(int margin=-1;margin<=1;++margin){
  CombatCore c;c.reset(who?(shell?2:0):1,who?1:(shell?2:0));auto&v=c.f[1-who];v.action=BLOCKSTUN;v.ends=100;v.face=who?1:-1;
  int cost=shell?18:23;v.sta=cost*60+margin;int hp=v.hp;shot(c,who,14);frame(c,who);
  CHECK(v.hp==hp-(margin<0?14:0));CHECK(v.sta==(margin>0?1:0));CHECK((v.action==GUARD_BREAK)==(margin<=0));
 }
 for(int who=0;who<2;++who){
  CombatCore c;c.reset(who?2:0,who?0:2);auto&v=c.f[1-who];v.action=BLOCKSTUN;v.ends=100;v.face=who?1:-1;int sta=v.sta;shot(c,who,8);frame(c,who);CHECK(v.sta==sta-8*60);
 }
 // Water push is physical, while supers retain their shared knockback rule.
 for(int ch=0;ch<6;++ch)for(int who=0;who<2;++who)for(int super=0;super<2;++super){
  CombatCore c;pair(c,ch,who);shot(c,who,8,super);frame(c,who);
  CHECK(c.f[1-who].kb==(who?-1:1)*(super?motion_super_knockback_q8:characters[ch].fire_knockback));
 }
 // Punish only attack recovery; test startup, active, final recovery and expiry.
 for(int who=0;who<2;++who)for(Action action:{MELEE,FIRE,SUPER,NONE,BLOCKSTUN,HITSTUN,GUARD_BREAK})for(int phase=0;phase<4;++phase){
  CombatCore c;pair(c,5,who);auto&me=c.f[who];auto&target=c.f[1-who];const auto&d=characters[target.character];
  int recovery=action==MELEE?d.melee_start+d.melee_active:action==FIRE?d.fire_start+1:super_startup_ticks+super_active_ticks;
  int total=action==MELEE?d.melee_total:action==FIRE?d.fire_total:super_startup_ticks+super_active_ticks+super_recovery_ticks;
  int age=phase==0?recovery-1:phase==1?recovery:phase==2?total-1:total;
  target.action=action;target.began=-age;target.ends=total-age;target.melee_contact=true;target.spawned=true;
  me.action=MELEE;me.began=-characters[5].melee_start;me.ends=100;me.attack_id=11;
  int hp=target.hp;bool punish=(action==MELEE||action==FIRE||action==SUPER)&&(phase==1||phase==2);frame(c,who);
  int expected=action==BLOCKSTUN&&phase<3?0:characters[5].melee_damage+(punish?3:0);
  if(target.hp!=hp-expected)std::fprintf(stderr,"punish side=%d action=%d phase=%d damage=%d expected=%d\n",who,int(action),phase,hp-target.hp,expected);
  CHECK(target.hp==hp-expected);CHECK(events(c,EV_PUNISH)==int(punish));
 }
 // Every fighter can confirm A into B, with a real defensive gap and full costs.
 for(int ch=0;ch<6;++ch)for(int who=0;who<2;++who)for(int defend=0;defend<2;++defend){
  CombatCore c;pair(c,ch,who);frame(c,who,{A,A});
  for(int n=0;n<30&&c.f[who].confirmed_at<0;++n)frame(c,who);
  CHECK(c.f[who].confirmed_at>=0);int confirmed=c.f[who].confirmed_at,hp=c.f[1-who].hp;
  for(int n=0;n<40&&!c.can_followup(who,B);++n)frame(c,who,{},defend?Input{L,0}:Input{});
  CHECK(c.can_followup(who,B));CHECK(c.tick<c.f[who].ends);int sta=c.f[who].sta;
  frame(c,who,{B,B},defend?Input{L,0}:Input{});CHECK(c.f[who].action==FIRE&&c.f[who].chain_depth==1);CHECK(c.f[who].sta==sta-fire_stamina_cost*60);CHECK(events(c,EV_FOLLOWUP)==1);
  CHECK(c.f[who].began+characters[ch].fire_start>=confirmed+reactions_normal_hitstun_ticks+followup_escape_ticks);
  bool hit=false,blocked=false;
  for(int n=0;n<65;++n){frame(c,who,{},defend?Input{L,0}:Input{});hit|=events(c,EV_HIT)>0;blocked|=events(c,EV_BLOCK)>0;}
  CHECK(defend?blocked:hit);CHECK(defend?c.f[1-who].hp==hp:c.f[1-who].hp<hp);
 }
 // Misses and ordinary blocks cannot manufacture a follow-up.
 for(int block=0;block<2;++block){CombatCore c;pair(c,4,0);if(!block)c.f[1].x=208*256;frame(c,0,{A,A},block?Input{L,0}:Input{});for(int n=0;n<16;++n){frame(c,0,{},block?Input{L,0}:Input{});CHECK(!c.can_followup(0,A)&&!c.can_followup(0,B));}CHECK(c.f[0].confirmed_at<0);}
 // Eevee's second A cannot rearm the chain. Held A also cannot repeat it.
 for(int who=0;who<2;++who){CombatCore c;pair(c,4,who);frame(c,who,{A,A});for(int n=0;n<40&&!c.can_followup(who,A);++n)frame(c,who);CHECK(c.can_followup(who,A));frame(c,who,{A,A});CHECK(c.f[who].chain_depth==1);bool hit=false;for(int n=0;n<35;++n){frame(c,who,{A,0});hit|=events(c,EV_HIT)>0;CHECK(!c.can_followup(who,A)&&!c.can_followup(who,B));}CHECK(hit);CHECK(c.f[who].action==NONE);}
 // A trade creates two hits but stun prevents either immediate cancel.
 {CombatCore c;pair(c,4,0);auto&v=c.f[0];v.action=MELEE;v.began=0;v.ends=17;v.confirmed_at=4;c.tick=12;CHECK(!c.can_followup(0,A));CHECK(c.can_followup(0,A,2));CHECK(!c.can_followup(0,A,3));CHECK(!c.can_followup(0,A,-1));frame(c,0,{A,A});CHECK(v.pending==A&&v.chain_depth==0);frame(c,0);CHECK(v.chain_depth==0);frame(c,0);CHECK(v.chain_depth==1&&events(c,EV_FOLLOWUP)==1);}
 {CombatCore c;pair(c,4,0);for(int i=0;i<2;++i){auto&v=c.f[i];v.action=MELEE;v.began=-characters[v.character].melee_start;v.ends=100;v.attack_id=i+1;}frame(c,0);CHECK(events(c,EV_HIT)==2);CHECK(!c.can_followup(0,B)&&!c.can_followup(1,B));}
 // Insufficient resources, occupied shot and air state remain real restrictions.
 {CombatCore c;pair(c,4,0);auto&v=c.f[0];v.action=MELEE;v.began=0;v.ends=17;v.confirmed_at=4;c.tick=14;CHECK(c.can_followup(0,A)&&c.can_followup(0,B));v.sta=479;CHECK(!c.can_followup(0,A));v.sta=1199;CHECK(!c.can_followup(0,B));v.sta=1200;CHECK(c.can_followup(0,B));c.p[0].active=true;CHECK(!c.can_followup(0,B));c.p[0].active=false;v.y=119*256;CHECK(!c.can_followup(0,A));}
 // Practice has the same damage/costs, a frozen timeout and normal KO outcomes.
 {CombatCore c;pair(c,4,0);c.training=true;c.remaining=1;PracticeSession practice;practice.begin_round();uint32_t before=c.checksum();CHECK(practice.input(c).held==0);CHECK(c.checksum()==before);practice.mode=DUMMY_BLOCK;CHECK(practice.input(c).held==L);practice.mode=DUMMY_ATTACK;CHECK(practice.input(c).pressed==A);c.f[1].hp=1;frame(c,0,{A,A});for(int n=0;n<10&&!c.outcome;++n){frame(c,0);practice.observe(c);}CHECK(c.outcome==1);CHECK(c.remaining==1);CHECK(practice.stats.last_damage==characters[4].melee_damage);CHECK(practice.stats.hits==1);practice.observe(c);CHECK(practice.stats.hits==1);practice.begin_round(true);CHECK(practice.stats.hits==1);practice.begin_round();CHECK(practice.stats.hits==0);}
 {CombatCore c;c.reset(0,1);c.training=true;PracticeSession practice;practice.mode=DUMMY_ATTACK;c.f[1].sp=100;CHECK(practice.input(c).pressed==R);c.hitstop=1;CHECK(practice.input(c).pressed==0);c.reset(0,1);CHECK(!c.training);}
 // Read-only consumers and all three dummy policies preserve deterministic replay.
 for(int mode=0;mode<DUMMY_MODE_COUNT;++mode)for(int ch=0;ch<6;++ch){CombatCore c,d;c.reset(ch,(ch+1)%6,7);d.reset(ch,(ch+1)%6,7);c.training=d.training=true;PracticeSession practice;practice.mode=DummyMode(mode);CpuController player;player.reset(1,99);CombatFeel feel;for(int n=0;n<2400&&!c.outcome;++n){auto left=player.input(c,0),right=practice.input(c);c.frame(left,right);d.frame(left,right);auto before=c.checksum();practice.observe(c);feel.update(c,true);CHECK(c.checksum()==before&&c.checksum()==d.checksum());CHECK(c.event_count<=CombatCore::max_events);}}
 std::printf("PASS %d fighter-depth, follow-up and practice checks.\n",checks);
}
