#pragma once
#include "pkmn_arcade.h"
#include "bn_sram.h"
#include "bn_span.h"
namespace pkmn {
class ProgressStore {
 int slot=-1;uint32_t sequence=0;
 static void read_copy(SaveSlot&copy,int offset,int minimum,int maximum){
  auto* bytes=reinterpret_cast<uint8_t*>(&copy);bn::span<uint8_t> header(bytes,8);bn::sram::read_span_offset(header,offset);
  if(copy.words[0]!=0x504b4d38||int(copy.words[1])<minimum||int(copy.words[1])>maximum)return;
  bn::span<uint8_t> payload(bytes+8,schema_words(copy.words[1])*4-8);bn::sram::read_span_offset(payload,offset+8);
 }
public:
 bool failed=false;
 Progress load(){
  SaveSlot copies[2];
  for(int i=0;i<2;++i)read_copy(copies[i],save_offsets[i],save_schema,save_schema);
  int selected=newest_save_version(copies[0],copies[1],save_schema,save_schema);slot=selected;
  if(selected<0){for(auto& copy:copies)copy=SaveSlot();for(int i=0;i<2;++i)read_copy(copies[i],legacy_save_offsets[i],1,save_schema-1);selected=newest_save_version(copies[0],copies[1],1,save_schema-1);}
  if(selected<0)return Progress();sequence=copies[selected].words[2];return decode_save(copies[selected]);
 }
 bool save(const Progress&p){
  int target=slot==0?1:0,offset=save_offsets[target];SaveSlot next=encode_save(p,sequence+1);
  uint32_t invalid=0;bn::sram::write_offset(invalid,offset);
  bn::span<const uint8_t> payload(reinterpret_cast<const uint8_t*>(&next),sizeof(SaveSlot)-4);
  bn::sram::write_span_offset(payload,offset);bn::sram::write_offset(next.words[save_words-1],offset+sizeof(SaveSlot)-4);
  SaveSlot verify;bn::sram::read_offset(verify,offset);failed=!valid_save(verify)||verify.words[2]!=sequence+1;
  if(!failed){slot=target;++sequence;}return !failed;
 }
};
}
