#include "pkmn_animation.h"
#include <cassert>
#include <cstdio>
using namespace pkmn;
int main(){int checks=0;
 for(int ch=0;ch<6;ch++){
  CombatCore c;c.reset(ch,5-ch);c.step({RIGHT,RIGHT},{});assert(c.f[0].vx>0&&c.f[0].vx<characters[ch].speed);assert(fighter_pose(c,0).clip==CLIP_WALK);checks+=2;
  bool visited[16]={};for(int i=0;i<32;i++){c.step({RIGHT,0},{});auto p=fighter_pose(c,0);if(p.clip==CLIP_WALK)visited[p.frame-animations[ch][CLIP_WALK].first]=true;}
  for(int i=0;i<animations[ch][CLIP_WALK].count;i++){assert(visited[i]);++checks;}
  c.reset(ch,5-ch);for(int i=0;i<6;i++)c.step({RIGHT,0},{});assert(c.f[0].vx==characters[ch].speed);++checks;
  c.step({},{});assert(c.f[0].vx>0&&c.f[0].vx<characters[ch].speed);++checks;
  for(int i=0;i<6;i++)c.step({},{});assert(c.f[0].vx==0);++checks;
  for(int i=0;i<6;i++)c.step({RIGHT,0},{});c.step({LEFT,LEFT},{});assert(c.f[0].vx>=0);++checks;
  for(int i=0;i<8;i++)c.step({LEFT,0},{});assert(c.f[0].vx==-characters[ch].speed);++checks;
  c.f[0].x=world_min_fighter_x*256;c.step({LEFT,0},{});assert(c.f[0].vx==0&&c.f[0].x==world_min_fighter_x*256);++checks;
  c.reset(ch,5-ch);c.step({UP,UP},{});assert(fighter_pose(c,0).clip==CLIP_HOP);++checks;
  for(int i=0;i<32;i++)c.step({},{});assert(fighter_pose(c,0).clip==CLIP_LAND);++checks;
  c.step({A,A},{});assert(fighter_pose(c,0).clip==CLIP_ATTACK);++checks;
  c.reset(ch,5-ch);c.step({A,A},{});const auto&a=animations[ch][CLIP_ATTACK];assert(fighter_pose(c,0).frame<a.first+a.hit);++checks;
  for(int i=0;i<characters[ch].melee_start;i++)c.step({},{});assert(fighter_pose(c,0).frame==a.first+a.hit);++checks;
  int frame=fighter_pose(c,0).frame,tick=c.tick;c.hitstop=4;for(int i=0;i<4;i++){c.frame({},{});assert(fighter_pose(c,0).frame==frame&&c.tick==tick);++checks;}
  c.reset(ch,5-ch);c.step({B,B},{});for(int i=0;i<characters[ch].fire_start;i++)c.step({},{});assert(c.p[0].active);assert(fighter_pose(c,0).frame==animations[ch][CLIP_SHOOT].first+animations[ch][CLIP_SHOOT].hit);checks+=2;
  c.reset(ch,5-ch);c.step({DOWN,DOWN},{});assert(fighter_pose(c,0).clip==CLIP_LAND);++checks;for(int i=0;i<3;i++)c.step({},{});assert(fighter_pose(c,0).clip==CLIP_SQUEEZE);++checks;
  c.reset(ch,5-ch);c.step({L,L},{});assert(fighter_pose(c,0).clip==CLIP_GUARD);c.step({uint16_t(L|UP),UP},{});assert(c.f[0].action==NONE&&c.f[0].vy<0);checks+=2;
  c.reset(ch,5-ch);c.f[0].sp=100;c.step({R,R},{});assert(fighter_pose(c,0).clip==CLIP_CHARGE);++checks;for(int i=0;i<30;i++)c.step({},{});assert(c.p[2].active&&fighter_pose(c,0).clip==CLIP_SHOOT);++checks;
  for(int clip=0;clip<CLIP_COUNT;clip++)for(int t=0;t<240;t++){auto p=loop_pose(ch,Clip(clip),t);const auto&a=animations[ch][clip];assert(p.frame>=a.first&&p.frame<a.first+a.count);++checks;}
 }
 {CombatCore c;c.reset(0,1);for(int i=0;i<5;i++)c.step({RIGHT,0},{});c.step({uint16_t(RIGHT|UP),UP},{});assert(c.f[0].vx>characters[0].speed*3/4);int v=c.f[0].vx;c.step({},{});assert(c.f[0].vx==v-motion_air_drag_q8);for(int i=0;i<40;i++)c.step({},{});assert(c.f[0].landed_at>0&&c.grounded(c.f[0]));checks+=3;}
 {CombatCore c;c.reset(0,1);c.f[0].x=100*256;c.f[1].x=116*256;c.step({A,A},{});for(int i=0;i<5;i++)c.step({},{});assert(c.f[1].vy<0&&c.f[1].kb>0);for(int i=0;i<20;i++)c.step({},{});assert(c.grounded(c.f[1])&&c.f[1].kb==0);checks+=2;}
 std::printf("PASS %d movement and animation checks.\n",checks);
}
