#pragma once
#include "pkmn_combat_core.h"
#include "pkmn_animation_data.h"
namespace pkmn {
struct Pose {Clip clip;int frame;};
inline int clamp_frame(int v,int n){return v<0?0:v>=n?n-1:v;}
inline int clip_time(const AnimationClip& c,int t,bool loop){int total=0;for(int i=0;i<c.count;i++)total+=c.duration[i];if(t<0)t=0;if(loop)t%=total;else if(t>=total)return c.count-1;for(int i=0;i<c.count;i++){if(t<c.duration[i])return i;t-=c.duration[i];}return c.count-1;}
inline Pose loop_pose(int ch,Clip clip,int t,bool loop=true){const auto&a=animations[ch][clip];return{clip,a.first+clip_time(a,t,loop)};}
// Authored impact frame and gameplay hitbox begin on the same simulation step.
inline Pose attack_pose(int ch,Clip clip,int age,int startup,int active,int total){const auto&a=animations[ch][clip];int frame=0;if(age<startup)frame=a.hit?age*a.hit/startup:0;else if(age<startup+active)frame=a.hit+(age-startup)*(a.back-a.hit)/active;else{int recovery=total-startup-active;frame=a.back+(age-startup-active)*(a.count-a.back)/(recovery?recovery:1);}return{clip,a.first+clamp_frame(frame,a.count)};}
inline Pose fighter_pose(const CombatCore&core,int who){const auto&v=core.f[who];const auto&d=characters[v.character];int age=core.age(v)-1;if(age<0)age=0;
 if(v.action==HITSTUN||v.action==GUARD_BREAK)return loop_pose(v.character,CLIP_HURT,age,false);
 if(v.action==MELEE)return attack_pose(v.character,CLIP_ATTACK,age,d.melee_start,d.melee_active,d.melee_total);
 if(v.action==FIRE)return attack_pose(v.character,CLIP_SHOOT,age,d.fire_start,1,d.fire_total);
 if(v.action==SUPER){if(age<super_startup_ticks)return loop_pose(v.character,CLIP_CHARGE,age);return attack_pose(v.character,CLIP_SHOOT,age-super_startup_ticks+1,1,3,super_recovery_ticks+4);}
 if(v.action==BLOCK||v.action==BLOCKSTUN)return loop_pose(v.character,CLIP_GUARD,core.tick);
 if(v.action==SQUEEZE){if(age>=squeeze_startup_ticks&&age<squeeze_startup_ticks+squeeze_active_ticks)return loop_pose(v.character,CLIP_SQUEEZE,age-squeeze_startup_ticks);return loop_pose(v.character,CLIP_LAND,age,false);}
 if(!core.grounded(v)){const auto&a=animations[v.character][CLIP_HOP];int frame=v.vy<-256?2:v.vy<=256?4:7;return{CLIP_HOP,a.first+clamp_frame(frame,a.count)};}
 if(core.tick-v.landed_at<=motion_landing_pose_ticks)return loop_pose(v.character,CLIP_LAND,core.tick-v.landed_at,false);
 if(v.vx){const auto&a=animations[v.character][CLIP_WALK];return{CLIP_WALK,a.first+v.stride_q8*a.count/motion_walk_stride_q8};}
 return loop_pose(v.character,CLIP_IDLE,core.tick+who*19);
}
}
