#include "pkmn_mastery.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cstring>
using namespace pkmn;
static void observe(CombatCore& c,MasterySession& session,Progress& p){
 uint32_t before=c.checksum();session.observe(c,p);assert(c.checksum()==before);
}
static void frame(CombatCore& c,MasterySession& session,Progress& p,Input player={},Input cpu={}){
 c.frame(player,cpu);observe(c,session,p);
 // Re-reading the same display frame never awards another hit or dodge.
 auto a=p.mastery[c.f[0].character];observe(c,session,p);auto b=p.mastery[c.f[0].character];
 assert(a.followups==b.followups&&a.evades==b.evades);
}
static void followup(int fighter,Progress& progress,bool training=false){
 CombatCore c;c.reset(fighter,(fighter+1)%character_count);c.training=training;
 c.f[0].x=100*256;c.f[1].x=120*256;MasterySession session;
 frame(c,session,progress,{A,A});bool linked=false;
 for(int t=0;t<140;++t){
  if(!linked&&c.can_followup(0,B)){frame(c,session,progress,{B,B});linked=true;}
  else frame(c,session,progress);
 }
 assert(linked);
}
static void evade(int fighter,Progress& progress,bool jump,bool training=false){
 CombatCore c;c.reset(fighter,(fighter+1)%character_count);c.training=training;c.f[1].sp=100;MasterySession session;
 frame(c,session,progress,{}, {R,R});
 while(c.tick<39)frame(c,session,progress);
 frame(c,session,progress,{uint16_t(jump?UP:DOWN),uint16_t(jump?UP:DOWN)});
 for(int t=0;t<65;++t)frame(c,session,progress);
 assert(c.f[0].hp==characters[fighter].hp);
}
int main(){
 Progress p;
 for(int fighter=0;fighter<character_count;++fighter){
  followup(fighter,p,true);evade(fighter,p,true,true);
  assert(!p.mastery[fighter].followups&&!p.mastery[fighter].evades);
  for(unsigned i=0;i<mastery_followup_goal+2;++i)followup(fighter,p);
  assert(p.mastery[fighter].followups==mastery_followup_goal&&p.medals(fighter)==1);
  for(unsigned i=0;i<mastery_evade_goal+2;++i)evade(fighter,p,i%2==0);
  assert(p.mastery[fighter].evades==mastery_evade_goal&&p.medals(fighter)==2);
  ArcadeRun run;run.begin(fighter,2,99);p.complete(run);assert(!p.no_continue(fighter));
  for(int i=0;i<arcade_rivals;++i)run.advance();run.ticks=600;
  run.used=1;p.complete(run);assert(p.medals(fighter)==2);
  run.used=0;p.complete(run);assert(p.medals(fighter)==3);
 }
 // Blocked and missed projectiles, enemy follow-ups and ordinary super damage
 // do not masquerade as player mastery. Uses resolved core events only.
 CombatCore c;c.reset(0,1);MasterySession session;Progress empty;
 c.f[0].x=100*256;c.f[1].x=120*256;
 for(int t=0;t<90;++t)frame(c,session,empty,t==0?Input{A,A}:Input{}, {L,L});
 assert(empty.mastery[0].followups==0);
 c.reset(0,1);session.reset();c.f[1].sp=100;
 for(int t=0;t<100;++t)frame(c,session,empty,{},t==0?Input{R,R}:Input{});
 assert(c.f[0].hp<characters[0].hp&&empty.mastery[0].evades==0);
 // Legacy nine-fighter bytes are a frozen schema-2 fixture, independent of
 // the schema-3 encoder under test. Unknown tail bytes must be ignored.
 std::ifstream fixture("tests/fixtures/nine_fighter_v08.sav",std::ios::binary);assert(fixture);
 SaveSlot old;fixture.seekg(512);fixture.read(reinterpret_cast<char*>(&old),sizeof(old));
 assert(valid_save(old)&&old.words[1]==2);Progress migrated=decode_save(old);
 assert(!migrated.music&&!migrated.sound);
 for(int i=0;i<record_count;++i){assert(migrated.records[i].clears==unsigned(i+1));assert(migrated.records[i].perfect==unsigned(i/3));assert(migrated.records[i].best_ticks==unsigned((i+1)*600));}
 for(int i=0;i<character_count;++i){assert(!migrated.mastery[i].followups&&!migrated.mastery[i].evades);assert(migrated.medals(i)==(i==0?0:1));}
 SaveSlot next=encode_save(migrated,100);assert(valid_save(next)&&next.words[1]==3&&sizeof(next)==420);
 Progress recovered=decode_save(next);assert(recovered.record(8,2).clears==27&&recovered.medals(8)==1);
 auto full=encode_save(p,101);assert(valid_save(full));recovered=decode_save(full);
 for(int i=0;i<character_count;++i)assert(recovered.medals(i)==3);
 for(unsigned bytes=0;bytes<sizeof(full);++bytes){SaveSlot interrupted=old;interrupted.words[0]=0;std::memcpy(&interrupted,&full,bytes);assert(newest_save(old,interrupted)==0);}
 full.words[mastery_offset]=mastery_followup_goal+1;full.words[save_words-1]=save_checksum(full);assert(!valid_save(full));
 full=encode_save(p,101);full.words[mastery_offset+1]=mastery_evade_goal+1;full.words[save_words-1]=save_checksum(full);assert(!valid_save(full));
 unsigned rotations=0;
 for(unsigned seed=0;seed<100;++seed){ArcadeRun run;run.begin(0,1,seed);unsigned mask=0;int previous=-1;
  for(int stage=0;stage<5;++stage){int arena=run.arena();assert(arena>=0&&arena<arena_count&&arena!=previous);mask|=1u<<arena;previous=arena;assert(run.retry(3)&&run.arena()==arena);run.advance();}
  assert(mask==7);++rotations;
 }
 for(int stage=0;stage<arena_count;++stage)for(int phase=-1;phase<16;++phase)for(int y=0;y<160;++y){
  int motion=arena_sway(stage,y,phase);assert(motion>=-1&&motion<=1);
  if(phase<0||y<44||y>=100)assert(motion==0);
 }
 std::printf("PASS mastery: all nine fighters, real damaging follow-ups, jump/squeeze evasion, training exclusion, caps, schema-2/3 migration and %u arena rotations; combat checksums unchanged.\n",rotations);
}
