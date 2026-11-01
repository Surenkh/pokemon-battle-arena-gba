#pragma once
#include "pkmn_arcade.h"
#include "bn_sram.h"
#include "bn_span.h"
namespace pkmn {
class ProgressStore {
 int slot=-1;uint32_t sequence=0;
public:
 bool failed=false;
 Progress load(){SaveSlot a,b;bn::sram::read_offset(a,0);bn::sram::read_offset(b,512);slot=newest_save(a,b);if(slot<0)return Progress();const auto&s=slot?b:a;sequence=s.words[2];return decode_save(s);}
 bool save(const Progress&p){
  int target=slot==0?1:0,offset=target*512;SaveSlot next=encode_save(p,sequence+1);
  uint32_t invalid=0;bn::sram::write_offset(invalid,offset);
  bn::span<const uint8_t> payload(reinterpret_cast<const uint8_t*>(&next),sizeof(SaveSlot)-4);
  bn::sram::write_span_offset(payload,offset);bn::sram::write_offset(next.words[save_words-1],offset+sizeof(SaveSlot)-4);
  SaveSlot verify;bn::sram::read_offset(verify,offset);failed=!valid_save(verify)||verify.words[2]!=sequence+1;
  if(!failed){slot=target;++sequence;}return !failed;
 }
};
}
