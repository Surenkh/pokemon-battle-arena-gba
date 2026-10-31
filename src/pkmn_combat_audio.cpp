#include "pkmn_combat_audio.h"
#include "bn_sound_items.h"
#include "bn_sound_item.h"
namespace pkmn {
static const bn::sound_item* const shots[]={&bn::sound_items::shot_fx_0,&bn::sound_items::shot_fx_1,&bn::sound_items::shot_fx_2,&bn::sound_items::shot_fx_3,&bn::sound_items::shot_fx_4,&bn::sound_items::shot_fx_5};
static const bn::sound_item* const hits[]={&bn::sound_items::hit_fx_0,&bn::sound_items::hit_fx_1,&bn::sound_items::hit_fx_2,&bn::sound_items::hit_fx_3,&bn::sound_items::hit_fx_4,&bn::sound_items::hit_fx_5};
static const bn::sound_item* const charges[]={&bn::sound_items::charge_fx_0,&bn::sound_items::charge_fx_1,&bn::sound_items::charge_fx_2,&bn::sound_items::charge_fx_3,&bn::sound_items::charge_fx_4,&bn::sound_items::charge_fx_5};
static const bn::sound_item* const releases[]={&bn::sound_items::release_fx_0,&bn::sound_items::release_fx_1,&bn::sound_items::release_fx_2,&bn::sound_items::release_fx_3,&bn::sound_items::release_fx_4,&bn::sound_items::release_fx_5};
static int priority(const CombatEvent&e){switch(e.kind){case EV_BREAK:return 11;case EV_RELEASE:return 10;case EV_HIT:return e.super?9:7;case EV_BLOCK:return 6;case EV_SHOT:return 5;case EV_READY:return 4;case EV_TIRED:return 3;case EV_MELEE:return 2;case EV_JUMP:return 1;default:return 0;}}
void CombatAudio::reset(){
 for(auto&v:fx){if(v&&v->active())v->stop();v.reset();}
 charge_owner[0]=charge_owner[1]=-1;charge_phase[0]=charge_phase[1]=-1;last_frame=~0u;running=false;clock=cursor=played=critical=0;
 last_tired[0]=last_tired[1]=-100;character_mask=charge_mask=release_mask=0;
}
int CombatAudio::active_voices()const{int n=0;for(const auto&v:fx)if(v&&v->active())++n;return n;}
int CombatAudio::active_charges()const{int n=0;for(int i=0;i<2;i++)if(charge_owner[i]>=0&&fx[i]&&fx[i]->active())++n;return n;}
void CombatAudio::play_event(const CombatCore&c,const CombatEvent&e){
 const int who=e.owner,ch=c.f[who].character;const bn::sound_item* item=nullptr;bn::fixed volume=.30;
 switch(e.kind){
 case EV_SHOT:item=shots[ch];character_mask|=1<<ch;break;
 case EV_RELEASE:item=releases[ch];volume=.40;character_mask|=1<<ch;release_mask|=1<<ch;++critical;break;
 case EV_HIT:item=hits[ch];volume=e.super?.40:.30;character_mask|=1<<ch;break;
 case EV_BLOCK:item=&bn::sound_items::block;volume=.22;break;
 case EV_BREAK:item=&bn::sound_items::guard_break;volume=.38;++critical;break;
 case EV_TIRED:if(clock-last_tired[who]<18)return;last_tired[who]=clock;item=&bn::sound_items::no_stamina;volume=.23;break;
 case EV_READY:item=&bn::sound_items::power_ready;volume=.23;break;
 case EV_MELEE:item=&bn::sound_items::swing;volume=.18;break;
 case EV_JUMP:item=&bn::sound_items::jump;volume=.20;break;
 default:return;
 }
 auto&v=fx[cursor];cursor=(cursor+1)%2;if(v&&v->active())v->stop();
 charge_owner[(cursor+1)%2]=-1;v=item->play(volume,1,bn::fixed(c.f[who].x/256-120)/240);++played;
}
void CombatAudio::update(const CombatCore&c,bool active){
 if(!active){if(running)for(auto&v:fx){if(v&&v->active())v->stop();v.reset();}running=false;last_frame=c.event_frame;charge_phase[0]=charge_phase[1]=-1;charge_owner[0]=charge_owner[1]=-1;return;}
 running=true;++clock;
 for(int n=0;n<2;n++)if(charge_owner[n]>=0){const auto&v=c.f[charge_owner[n]];int age=c.tick-v.began-1;if(v.action!=SUPER||age>=super_startup_ticks){if(fx[n]&&fx[n]->active())fx[n]->stop();fx[n].reset();charge_owner[n]=-1;}}
 if(last_frame!=c.event_frame){last_frame=c.event_frame;int best[2]={-1,-1};
  for(int n=0;n<c.event_count;n++){int rank=priority(c.events[n]);if(!rank)continue;if(best[0]<0||rank>priority(c.events[best[0]])){best[1]=best[0];best[0]=n;}else if(best[1]<0||rank>priority(c.events[best[1]]))best[1]=n;}
  for(int n:best)if(n>=0)play_event(c,c.events[n]);
 }
 // Two total combat voices. Charging fills free slots only, so impacts and
 // releases retain priority and the GBA mixer stays within its frame budget.
 for(int i=0;i<2;i++){const auto&v=c.f[i];int age=c.tick-v.began-1;bool charging=v.action==SUPER&&age>=0&&age<super_startup_ticks;
  if(charging){int phase=age/10;if(charge_phase[i]!=phase){charge_phase[i]=phase;for(int n=0;n<2;n++)if(!fx[n]||!fx[n]->active()){fx[n]=charges[v.character]->play(bn::fixed(.18),bn::fixed(.85)+bn::fixed(phase)/5,bn::fixed(v.x/256-120)/240);charge_owner[n]=i;charge_mask|=1<<v.character;++played;break;}}}
  else charge_phase[i]=-1;
 }
}
}
