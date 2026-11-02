#pragma once
#include "pkmn_arcade.h"

namespace pkmn {

enum GoalKind { GOAL_NONE, GOAL_FOLLOWUPS, GOAL_EVADES, GOAL_NO_CONTINUE, GOAL_COMPLETE };

struct ResultGoal {
 GoalKind kind=GOAL_NONE;
 uint32_t before=0,after=0,target=0;
 bool advanced=false;
 const char* label()const{
  return kind==GOAL_FOLLOWUPS?"FOLLOW-UP HITS":kind==GOAL_EVADES?"SUPER EVADES":kind==GOAL_NO_CONTINUE?"NO-CONTINUE ARCADE":"";
 }
};

// Presentation-only snapshot. Progress remains authoritative and is never
// awarded here. A completed medal takes priority over an intermediate step.
class MatchGoals {
 FighterMastery initial;
 bool initial_perfect=false,eligible=false,finished=false;
 int fighter=0;
 ResultGoal result;

 static uint32_t capped(uint32_t value,uint32_t goal){return value<goal?value:goal;}
 void choose(GoalKind kind,uint32_t before,uint32_t after,uint32_t target){
  result={kind,before,after,target,after>before};
 }
public:
 void begin(const Progress& progress,int who,bool training){
  *this=MatchGoals();fighter=who;eligible=!training;
  initial=progress.mastery[who];initial_perfect=progress.no_continue(who);
 }
 void finish(const Progress& progress){
  if(!eligible||finished)return;
  finished=true;
  const auto& current=progress.mastery[fighter];
  uint32_t f0=capped(initial.followups,mastery_followup_goal),f1=capped(current.followups,mastery_followup_goal);
  uint32_t e0=capped(initial.evades,mastery_evade_goal),e1=capped(current.evades,mastery_evade_goal);
  bool perfect=progress.no_continue(fighter);
  if(!initial_perfect&&perfect)choose(GOAL_NO_CONTINUE,0,1,1);
  else if(f0<mastery_followup_goal&&f1==mastery_followup_goal)choose(GOAL_FOLLOWUPS,f0,f1,mastery_followup_goal);
  else if(e0<mastery_evade_goal&&e1==mastery_evade_goal)choose(GOAL_EVADES,e0,e1,mastery_evade_goal);
  else if(f1>f0)choose(GOAL_FOLLOWUPS,f0,f1,mastery_followup_goal);
  else if(e1>e0)choose(GOAL_EVADES,e0,e1,mastery_evade_goal);
  else if(f1<mastery_followup_goal)choose(GOAL_FOLLOWUPS,f1,f1,mastery_followup_goal);
  else if(e1<mastery_evade_goal)choose(GOAL_EVADES,e1,e1,mastery_evade_goal);
  else if(!perfect)choose(GOAL_NO_CONTINUE,0,0,1);
  else result.kind=GOAL_COMPLETE;
 }
 const ResultGoal& goal()const{return result;}
 bool ready()const{return finished;}
 int who()const{return fighter;}
};

// Paired with each rival's authored trait, never its current private AI state.
// trait + ": " + action fits within 28 native eight-pixel glyphs.
inline constexpr const char* scouting_actions[character_count]={
 "KEEP SOME SPACE", "JUMP FIRE", "USE SUPER", "BAIT THEN CLOSE",
 "BLOCK FIRST A", "AVOID MISSED A", "JUMP THE PULSE", "PUNISH LANDING", "LEAVE SPACE"
};

// Called after Arcade's existing advance. Wins scout the next rival; a draw
// or usable continue scouts the same rival. Finished/exhausted runs have none.
inline int scouting_rival(const ArcadeRun& run,int outcome){
 if(!run.active||run.finished)return -1;
 return (outcome==1||outcome==3||(outcome==2&&run.continues))?run.opponent():-1;
}

}
