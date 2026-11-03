#include "pkmn_arcade.h"
#include "pkmn_combat_core.h"
#include "pkmn_roster_navigation.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
using namespace pkmn;
static unsigned checks=0;
#define CHECK(x) do{++checks;assert(x);}while(0)
static int guarded(int who){CombatCore c;c.reset(who,0);c.f[0].x=100*256;c.f[1].x=120*256;int before=c.f[1].sta,ticks=0;bool hit=false;for(int t=0;t<60&&!hit;++t){c.step(t?Input{}:Input{A,A},{L,uint16_t(t?0:L)});++ticks;for(int i=0;i<c.event_count;++i)hit|=c.events[i].kind==EV_BLOCK;}CHECK(hit&&c.f[1].hp==characters[0].hp);return before-c.f[1].sta-ticks*resources_block_drain_subunits_per_tick;}
int main(){
 CHECK(character_count==13&&arena_count==5&&record_count==39&&save_schema==4&&sizeof(SaveSlot)==596);
 CHECK(schema_words(1)==60&&schema_words(2)==87&&schema_words(3)==105&&schema_words(4)==149);
 CHECK(save_offsets[0]>=932&&save_offsets[1]>=save_offsets[0]+int(sizeof(SaveSlot)));
 CHECK(roster_pages==1&&roster_page(8)==0&&roster_page(9)==0&&roster_slot(12)==12);
 for(int i=0;i<13;++i){CHECK(roster_up(i)>=0&&roster_up(i)<13);CHECK(roster_page(roster_up(i))==roster_page(i));}
 CHECK(roster_up(9)==4&&roster_up(12)==7&&!roster_has_down(10)&&!roster_has_down(12));
 CHECK(guarded(12)>guarded(1));CHECK(characters[9].melee_lunge>0&&characters[10].gravity_percent==80);
 CombatCore c;c.reset(11,1);c.f[0].x=100*256;c.f[1].x=120*256;c.f[1].action=FIRE;c.f[1].began=-characters[1].fire_start-1;c.f[1].ends=100;c.step({A,A},{});bool punished=false;
 for(int t=0;t<30;++t){for(int i=0;i<c.event_count;++i)punished|=c.events[i].kind==EV_PUNISH;c.step({},{});}CHECK(punished&&c.f[1].hp==characters[1].hp-characters[11].melee_damage-4);
 SaveSlot a,b,empty;std::ifstream f("tests/fixtures/nine_fighter_v1.sav",std::ios::binary);CHECK(bool(f));f.read(reinterpret_cast<char*>(&a),sizeof(a));f.seekg(512);f.read(reinterpret_cast<char*>(&b),sizeof(b));CHECK(valid_save(a)&&valid_save(b)&&b.words[1]==3);CHECK(select_save(empty,empty,a,b)==3);
 auto p=decode_save(b);CHECK(!p.music&&!p.sound&&p.record(0,0).clears==999999);
 for(int who=0;who<9;++who)CHECK(p.mastery[who].followups==unsigned(who)&&p.mastery[who].evades==unsigned(who%5));
 for(int who=9;who<13;++who){CHECK(p.medals(who)==0);for(int level=0;level<3;++level)CHECK(p.record(who,level).clears==0);}
 auto first=encode_save(p,b.words[2]+1);CHECK(valid_save(first));CHECK(!std::memcmp(decode_save(first).records,p.records,sizeof(p.records)));
 for(unsigned n=0;n<=sizeof(first);++n){SaveSlot partial;std::memcpy(&partial,&first,n);bool complete=!std::memcmp(&partial,&first,sizeof(first));CHECK(select_save(partial,empty,a,b)==(complete?0:3));}
 p.record(12,2)={1,1,600};p.mastery[12]={10,5};auto next=encode_save(p,first.words[2]+1);CHECK(decode_save(next).medals(12)==3);
 for(unsigned n=0;n<=sizeof(next);++n){SaveSlot partial;std::memcpy(&partial,&next,n);bool complete=!std::memcmp(&partial,&next,sizeof(next));CHECK(select_save(first,partial,a,b)==(complete?1:0));}
 for(unsigned bit=0;bit<sizeof(next)*8;++bit){auto bad=next;bad.words[bit/32]^=1u<<(bit%32);CHECK(!valid_save(bad));CHECK(select_save(first,bad,a,b)==0);}
 for(int firstArena=0;firstArena<5;++firstArena){ArcadeRun run;run.begin(0,0,unsigned(firstArena));unsigned mask=0;for(int stage=0;stage<5;++stage){int ar=run.arena();mask|=1u<<ar;CHECK(ar==(firstArena+stage)%5);CHECK(run.retry(3)&&run.arena()==ar);run.advance();}CHECK(mask==31);}
 for(int ar=0;ar<5;++ar)for(int phase=-1;phase<16;++phase)for(int y=0;y<160;++y){const int top[]={50,46,78,46,61},bottom[]={84,69,96,87,77},step[]={12,12,3,5,4};int wave=(phase+y/step[ar])%4,expected=phase<0||y<top[ar]||y>=bottom[ar]?0:wave==0?1:wave==2?-1:0;CHECK(arena_sway(ar,y,phase)==expected);}
 std::printf("PASS %u expansion checks: thirteen fighters, five arenas, real guard/punish traits, frozen schema-3 migration, all interrupted writes and single-bit corruptions.\n",checks);
}
