#pragma once
#include "pkmn_character_defs.h"
#include "pkmn_arenas.h"
#include <stdint.h>
namespace pkmn {
inline constexpr int arcade_rivals=5;
inline constexpr int record_count=character_count*3;
struct ArcadeRun {
 bool active=false,finished=false;int fighter=0,difficulty=1,stage=0,continues=2,used=0;
 int roster[arcade_rivals]={};uint32_t ticks=0;int first_arena=0;
 void begin(int who,int level,uint32_t seed){
  *this=ArcadeRun();active=true;fighter=who;difficulty=level;first_arena=int(seed%arena_count);
  int candidates[character_count-1],n=0;for(int i=0;i<character_count;++i)if(i!=who)candidates[n++]=i;
  for(int i=n-1;i>0;--i){seed=seed*1664525u+1013904223u;int j=int(seed%uint32_t(i+1)),v=candidates[i];candidates[i]=candidates[j];candidates[j]=v;}
  for(int i=0;i<arcade_rivals;++i)roster[i]=candidates[i];
 }
 int opponent()const{return roster[stage<arcade_rivals?stage:arcade_rivals-1];}
 int arena()const{return arcade_arena(uint32_t(first_arena),stage<arcade_rivals?stage:arcade_rivals-1);}
 void advance(){if(active&&!finished){++stage;if(stage==arcade_rivals)finished=true;}}
 bool retry(int outcome){if(!active||finished)return false;if(outcome==2){if(!continues)return false;--continues;++used;}return true;}
};
struct ArcadeRecord {uint32_t clears=0,perfect=0,best_ticks=0;};
inline constexpr uint32_t mastery_followup_goal=10,mastery_evade_goal=5;
struct FighterMastery {uint32_t followups=0,evades=0;};
struct Progress {
 bool music=true,sound=true;ArcadeRecord records[record_count]={};FighterMastery mastery[character_count]={};
 ArcadeRecord& record(int fighter,int difficulty){return records[fighter*3+difficulty];}
 const ArcadeRecord& record(int fighter,int difficulty)const{return records[fighter*3+difficulty];}
 bool no_continue(int fighter)const{for(int level=0;level<3;++level)if(record(fighter,level).perfect)return true;return false;}
 int medals(int fighter)const{return int(mastery[fighter].followups>=mastery_followup_goal)+int(mastery[fighter].evades>=mastery_evade_goal)+int(no_continue(fighter));}
 void complete(const ArcadeRun&run){if(!run.finished)return;auto&r=record(run.fighter,run.difficulty);if(r.clears<999999)++r.clears;if(!run.used&&r.perfect<999999)++r.perfect;if(!r.best_ticks||run.ticks<r.best_ticks)r.best_ticks=run.ticks;}
};
// Schema 3 appends two bounded mastery counters per fighter. Original records
// keep their offsets; schema 1/2 checksums use their original payload lengths.
// The no-continue medal is derived from records, including migrated records.
inline constexpr int mastery_offset=4+record_count*3;
inline constexpr int save_words=6+record_count*3+character_count*2;
struct SaveSlot {uint32_t words[save_words]={};};
static_assert(sizeof(SaveSlot)<512,"Each save copy fits its independent slot");
inline uint32_t save_checksum(const SaveSlot&s,int count=save_words){uint32_t h=2166136261u;for(int i=0;i<count-1;++i)for(int b=0;b<4;++b){h^=(s.words[i]>>(b*8))&255;h*=16777619u;}return h;}
inline SaveSlot encode_save(const Progress&p,uint32_t sequence){SaveSlot s;s.words[0]=0x504b4d38;s.words[1]=3;s.words[2]=sequence;s.words[3]=uint32_t(p.music)|(uint32_t(p.sound)<<1);for(int i=0;i<record_count;++i){s.words[4+i*3]=p.records[i].clears;s.words[5+i*3]=p.records[i].perfect;s.words[6+i*3]=p.records[i].best_ticks;}for(int i=0;i<character_count;++i){s.words[mastery_offset+i*2]=p.mastery[i].followups;s.words[mastery_offset+i*2+1]=p.mastery[i].evades;}s.words[save_words-1]=save_checksum(s);return s;}
inline bool valid_save(const SaveSlot&s){
 if(s.words[0]!=0x504b4d38||s.words[1]<1||s.words[1]>3||s.words[3]>3)return false;
 int records=s.words[1]==1?18:record_count,count=s.words[1]==3?save_words:6+records*3;
 if(s.words[count-2]!=0||s.words[count-1]!=save_checksum(s,count))return false;
 for(int i=0;i<records;++i){uint32_t clears=s.words[4+i*3],perfect=s.words[5+i*3],best=s.words[6+i*3];if(clears>999999||perfect>clears||bool(clears)!=bool(best))return false;}
 if(s.words[1]==3)for(int i=0;i<character_count;++i)if(s.words[mastery_offset+i*2]>mastery_followup_goal||s.words[mastery_offset+i*2+1]>mastery_evade_goal)return false;
 return true;
}
inline int newest_save(const SaveSlot&a,const SaveSlot&b){bool av=valid_save(a),bv=valid_save(b);if(!av)return bv?1:-1;if(!bv)return 0;uint32_t delta=b.words[2]-a.words[2];return delta&&delta<0x80000000u?1:0;}
inline Progress decode_save(const SaveSlot&s){Progress p;p.music=s.words[3]&1;p.sound=s.words[3]&2;int count=s.words[1]==1?18:record_count;for(int i=0;i<count;++i)p.records[i]={s.words[4+i*3],s.words[5+i*3],s.words[6+i*3]};if(s.words[1]==3)for(int i=0;i<character_count;++i)p.mastery[i]={s.words[mastery_offset+i*2],s.words[mastery_offset+i*2+1]};return p;}
}
