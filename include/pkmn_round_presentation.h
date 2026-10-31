#pragma once
#include "pkmn_combat_core.h"
namespace pkmn {
// Display-only sequence. Never changes combat, the round outcome or random state.
class RoundPresentation {
 static int clamp(int n,int a,int b){return n<a?a:n>b?b:n;}
public:
 enum Phase { IMPACT,ARRIVE,OPEN,ABSORB,CLOSE,FALL,SEAL,DONE };
 static constexpr int recall_frames=156,quick_frames=60;
 int age=0,loser=-1,target_x=120,from_x=120;bool recall=false;
 void reset(){age=0;loser=-1;target_x=from_x=120;recall=false;}
 void begin(const CombatCore&c,bool practice){
  reset();loser=c.outcome==1?1:c.outcome==2?0:-1;recall=!practice&&loser>=0;
  if(loser>=0){target_x=clamp(c.f[loser].x/256,32,208);int winner=1-loser;from_x=clamp(target_x+(c.f[winner].x<c.f[loser].x?-40:40),16,224);}
 }
 int duration()const{return recall?recall_frames:quick_frames;}
 void advance(){if(age<duration())++age;}
 void skip(){if(recall&&age>=112)age=recall_frames;}
 bool done()const{return age>=duration();}
 Phase phase()const{
  if(done())return DONE;
  if(!recall||age<24)return IMPACT;
  if(age<44)return ARRIVE;if(age<56)return OPEN;if(age<92)return ABSORB;
  if(age<104)return CLOSE;if(age<128)return FALL;return SEAL;
 }
 bool ball_visible()const{return recall&&age>=24&&!done();}
 int ball_x_q8()const{int t=clamp(age-24,0,20);return (from_x*(20-t)+target_x*t)*256/20;}
 int ball_y_q8()const{
  if(age<44){int t=clamp(age-24,0,20);return (32+28*t/20-t*(20-t)/14)*256;}
  if(age<104)return 60*256;
  int t=clamp(age-104,0,24);return (60+53*t*t/(24*24))*256;
 }
 int ball_frame()const{if(age>=44&&age<92)return age<48?1:2;if(age>=92&&age<98)return 1;return age>=140&&age<150?3:0;}
 int ball_angle()const{if(age<44)return (age-24)*18;return age>=130&&age<148?((age/3)%2?10:-10):0;}
 int absorb_q8()const{return recall?clamp((age-56)*256/36,0,256):0;}
 int body_scale_q8()const{int scale=256-absorb_q8();return scale<16?16:scale;}
 bool body_visible(int who)const{return !recall||who!=loser||age<92;}
 bool beam_visible()const{return recall&&age>=48&&age<92;}
 int body_y_q8(int ordinary_y)const{int t=absorb_q8();return (ordinary_y*(256-t)+60*t);}
 bool glint_visible()const{return recall&&age>=140&&age<154;}
};
}
