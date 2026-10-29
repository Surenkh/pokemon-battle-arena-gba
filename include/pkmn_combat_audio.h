#pragma once
#include "pkmn_combat_core.h"
#include "bn_optional.h"
#include "bn_sound_handle.h"
namespace pkmn {
class CombatAudio {
 bn::optional<bn::sound_handle> fx[2];
 int charge_owner[2]={-1,-1};
 uint32_t last_frame=~0u;int charge_phase[2]={-1,-1},cursor=0,clock=0,last_tired[2]={-100,-100};bool running=false;
 void play_event(const CombatCore&c,const CombatEvent&e);
public:
 uint32_t character_mask=0,charge_mask=0,release_mask=0;int played=0,critical=0;
 void reset();
 void update(const CombatCore&c,bool active);
 int active_voices()const;
 int active_charges()const;
};
}
