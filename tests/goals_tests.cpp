#include "pkmn_match_goals.h"
#include "pkmn_mastery.h"
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace pkmn;
static unsigned checks=0;
#define CHECK(value) do {++checks;assert(value);} while(0)

int main(){
 // Every valid pair of before/after counters, all fighters, all combinations
 // of existing/new/no perfect clear. The presentation cannot alter the save.
 for(int fighter=0;fighter<character_count;++fighter)
 for(unsigned f0=0;f0<=10;++f0)for(unsigned e0=0;e0<=5;++e0)
 for(unsigned f1=f0;f1<=10;++f1)for(unsigned e1=e0;e1<=5;++e1)
 for(int perfect=0;perfect<3;++perfect){
  Progress p;p.mastery[fighter]={f0,e0};
  if(perfect==2)p.record(fighter,2)={1,1,600};
  MatchGoals view;view.begin(p,fighter,false);CHECK(!view.ready());
  p.mastery[fighter]={f1,e1};if(perfect==1)p.record(fighter,1)={1,1,700};
  auto unchanged=encode_save(p,7);view.finish(p);const auto g=view.goal();
  CHECK(view.ready()&&view.who()==fighter);
  CHECK(g.kind!=GOAL_NONE);
  CHECK(g.advanced==(f1>f0||e1>e0||perfect==1));
  CHECK(g.before<=g.after&&g.after<=g.target);
  if(g.kind==GOAL_FOLLOWUPS){CHECK(g.after==f1&&g.target==10);CHECK(g.before==(g.advanced?f0:f1));}
  if(g.kind==GOAL_EVADES){CHECK(g.after==e1&&g.target==5);CHECK(g.before==(g.advanced?e0:e1));}
  if(g.kind==GOAL_NO_CONTINUE){CHECK(g.target==1&&g.after==unsigned(perfect!=0));}
  if(g.kind==GOAL_COMPLETE)CHECK(f1==10&&e1==5&&perfect==2);
  bool new_medal=(f0<10&&f1==10)||(e0<5&&e1==5)||perfect==1;
  if(new_medal)CHECK(g.advanced&&g.after==g.target);
  auto after=encode_save(p,7);CHECK(!std::memcmp(&unchanged,&after,sizeof(after)));
  // Re-reading results is idempotent, even if another fighter progresses.
  p.mastery[(fighter+1)%character_count]={10,5};view.finish(p);
  CHECK(view.goal().kind==g.kind&&view.goal().before==g.before&&view.goal().after==g.after);
 }
 // An actual damaging follow-up produces the displayed delta, including loss
 // paths: result selection never depends on the winner. Reset takes a new
 // baseline, so the prior match's increment is never presented twice.
 for(int fighter=0;fighter<character_count;++fighter){
  Progress p;MatchGoals view;MasterySession observer;CombatCore c;
  view.begin(p,fighter,false);c.reset(fighter,(fighter+1)%character_count);c.f[0].x=100*256;c.f[1].x=120*256;
  c.step({A,A},{});observer.observe(c,p);
  for(int t=0;t<40&&!c.can_followup(0,B);++t){c.step({},{});observer.observe(c,p);}
  CHECK(c.can_followup(0,B));c.step({B,B},{});observer.observe(c,p);
  for(int t=0;t<80;++t){c.step({},{});observer.observe(c,p);}
  CHECK(p.mastery[fighter].followups==1);auto checksum=c.checksum();view.finish(p);
  CHECK(view.goal().kind==GOAL_FOLLOWUPS&&view.goal().before==0&&view.goal().after==1&&view.goal().advanced);
  CHECK(c.checksum()==checksum);view.begin(p,fighter,false);view.finish(p);
  CHECK(!view.goal().advanced&&view.goal().before==1&&view.goal().after==1);
  view.begin(p,fighter,true);view.finish(p);CHECK(!view.ready()&&view.goal().kind==GOAL_NONE);
 }
 // A completed no-continue run supplies the third medal via actual records.
 Progress p;MatchGoals view;view.begin(p,3,false);ArcadeRun run;run.begin(3,0,42);
 for(int i=0;i<arcade_rivals;++i)run.advance();run.ticks=1200;p.complete(run);view.finish(p);
 CHECK(view.goal().kind==GOAL_NO_CONTINUE&&view.goal().before==0&&view.goal().after==1);
 // A new evade medal outranks an intermediate follow-up increment.
 p=Progress();p.mastery[0]={2,4};view.begin(p,0,false);p.mastery[0]={3,5};view.finish(p);
 CHECK(view.goal().kind==GOAL_EVADES&&view.goal().before==4&&view.goal().after==5);
 // Display the remaining goal, and a useful terminal state for a full set.
 p.mastery[0]={10,5};view.begin(p,0,false);view.finish(p);CHECK(view.goal().kind==GOAL_NO_CONTINUE&&!view.goal().advanced);
 p.record(0,0)={1,1,600};view.begin(p,0,false);view.finish(p);CHECK(view.goal().kind==GOAL_COMPLETE);
 // Scouting reads the rival AFTER advance, retains it on a paid loss/free
 // draw, and disappears at Champion or when no retry is available.
 unsigned runs=0;
 for(int fighter=0;fighter<character_count;++fighter)for(unsigned seed=0;seed<100;++seed){
  run.begin(fighter,int(seed%3),seed);++runs;
  for(int stage=0;stage<arcade_rivals;++stage){
   int rival=run.opponent();auto ticks=run.ticks;
   for(int outcome=1;outcome<=3;++outcome)CHECK(scouting_rival(run,outcome)==rival);
   CHECK(scouting_rival(run,0)==-1&&run.stage==stage&&run.ticks==ticks&&run.continues==2);
   run.continues=0;CHECK(scouting_rival(run,2)==-1&&scouting_rival(run,3)==rival);run.continues=2;
   run.advance();CHECK(scouting_rival(run,1)==(run.finished?-1:run.opponent()));
  }
  run.active=false;CHECK(scouting_rival(run,1)==-1);
 }
 for(int rival=0;rival<character_count;++rival){
  auto length=std::strlen(characters[rival].trait)+2+std::strlen(scouting_actions[rival]);
  CHECK(length<=28&&std::strlen(scouting_actions[rival])>0);
 }
 std::printf("PASS %u post-match goal checks; all counter/medal states, real follow-ups, per-match baselines, Practice exclusion, unchanged saves/combat, and %u read-only Arcade scouting runs.\n",checks,runs);
}
