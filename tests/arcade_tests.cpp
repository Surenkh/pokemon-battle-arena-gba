#include "pkmn_arcade.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
using namespace pkmn;
int main(){
 unsigned runs=0;
 for(int fighter=0;fighter<character_count;++fighter)for(int level=0;level<3;++level){
  unsigned encountered=0;
  for(unsigned seed=0;seed<100;++seed){
   ArcadeRun run;run.begin(fighter,level,seed);unsigned mask=1u<<fighter;
   assert(run.active&&!run.finished&&run.continues==2&&run.stage==0);
   for(int i=0;i<arcade_rivals;++i){assert(!(mask&(1u<<run.opponent())));mask|=1u<<run.opponent();encountered|=1u<<run.opponent();run.advance();}
   assert(__builtin_popcount(mask)==6&&run.finished&&run.stage==5);run.advance();assert(run.stage==5);
   Progress p;run.ticks=12345;p.complete(run);assert(p.record(fighter,level).clears==1&&p.record(fighter,level).perfect==1);
   run.used=1;run.ticks=15000;p.complete(run);assert(p.record(fighter,level).clears==2&&p.record(fighter,level).perfect==1&&p.record(fighter,level).best_ticks==12345);
   run.ticks=12000;p.complete(run);assert(p.record(fighter,level).best_ticks==12000);
   for(int f=0;f<character_count;++f)for(int d=0;d<3;++d)if(f!=fighter||d!=level)assert(p.record(f,d).clears==0);
   auto save=encode_save(p,seed);assert(valid_save(save));auto decoded=decode_save(save);assert(decoded.record(fighter,level).clears==3&&decoded.record(fighter,level).best_ticks==12000);
   for(unsigned bit=0;bit<sizeof(save)*8;++bit){auto damaged=save;damaged.words[bit/32]^=1u<<(bit%32);assert(!valid_save(damaged));}
   ++runs;
  }
  assert(encountered==((1u<<character_count)-1u-(1u<<fighter)));
 }
 ArcadeRun run;run.begin(0,1,5);run.ticks=60;assert(run.retry(3)&&run.continues==2&&run.used==0&&run.stage==0&&run.ticks==60);
 assert(run.retry(2)&&run.continues==1&&run.used==1);assert(run.retry(2)&&run.continues==0&&run.used==2);assert(!run.retry(2));assert(run.retry(3));
 Progress p;SaveSlot empty;auto old=encode_save(p,4),next=encode_save(p,5);
 assert(newest_save(empty,empty)==-1&&newest_save(old,empty)==0&&newest_save(empty,old)==1&&newest_save(old,next)==1);
 p.music=false;next=encode_save(p,5);
 for(unsigned bytes=0;bytes<sizeof(next);++bytes){
  SaveSlot partial;std::memcpy(&partial,&next,bytes);int n=newest_save(old,partial);assert(n==0||(n==1&&!std::memcmp(&partial,&next,sizeof(next))));
  partial=encode_save(Progress(),3);partial.words[0]=0;std::memcpy(&partial,&next,bytes);n=newest_save(old,partial);assert(n==0||(n==1&&!std::memcmp(&partial,&next,sizeof(next))));
 }
 auto wrap=encode_save(p,0),max=encode_save(p,0xffffffffu);assert(newest_save(max,wrap)==1);
 for(int flags=0;flags<4;++flags){p.music=flags&1;p.sound=flags&2;auto muted=decode_save(encode_save(p,1));assert(muted.music==p.music&&muted.sound==p.sound);}
 // Frozen schema-1 fixture from the prior six-fighter format, with unknown tail.
 std::ifstream file("tests/fixtures/six_fighter_v08.sav",std::ios::binary);assert(file);SaveSlot a,b;file.read(reinterpret_cast<char*>(&a),sizeof(a));file.seekg(512);file.read(reinterpret_cast<char*>(&b),sizeof(b));assert(valid_save(a)&&valid_save(b)&&newest_save(a,b)==1);
 Progress legacy=decode_save(b);assert(!legacy.music&&legacy.sound);assert(legacy.record(0,0).clears==2&&legacy.record(0,0).best_ticks==3600);assert(legacy.record(5,2).clears==7);for(int i=18;i<record_count;++i)assert(!legacy.records[i].clears);
 auto migrated=encode_save(legacy,43);assert(valid_save(migrated)&&newest_save(a,migrated)==1);auto recovered=decode_save(migrated);assert(recovered.record(5,2).perfect==3&&recovered.record(0,0).best_ticks==3600);
 ArcadeRun unfinished;unfinished.begin(0,1,1);Progress untouched;untouched.complete(unfinished);assert(!untouched.record(0,1).clears);
 std::printf("PASS Arcade/save: %u roster runs; %zu bit-corruption cases; %zu interrupted writes; schema-1 migration, nine-fighter records, retries and sequence wraparound.\n",runs,size_t(runs)*sizeof(SaveSlot)*8,sizeof(SaveSlot)*2);
}
