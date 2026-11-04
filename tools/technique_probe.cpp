#include "pkmn_combat_core.h"
#include <cstdio>
using namespace pkmn;
// Compile against the previous source/include path with this macro for a paired
// baseline. Gameplay is driven only by the same seeded public CPU policies.
struct PairStats {int rounds=0,p1=0,p2=0,draws=0,ticks=0,brakes=0,armed=0,burst_hits=0;uint32_t digest=2166136261u;};
int main(){
 PairStats pairs[character_count][character_count]{};int total=0,brakes=0,armed=0,hits=0;
 for(int a=0;a<character_count;a++)for(int b=0;b<character_count;b++){
  auto& stats=pairs[a][b];
  for(int level=0;level<3;level++)for(int seed=1;seed<=20;seed++){
   CombatCore core;core.reset(a,b,seed);CpuController left,right;left.reset(level,seed*23);right.reset(level,seed*37);
   for(int display=0;display<7000&&!core.outcome;display++){
    core.frame(left.input(core,0),right.input(core,1));
#ifndef PKMN_TECHNIQUE_PROBE_BASELINE
    for(int n=0;n<core.event_count;n++){stats.brakes+=core.events[n].kind==EV_AIR_BRAKE;stats.armed+=core.events[n].kind==EV_ORB_ARM;stats.burst_hits+=core.events[n].kind==EV_BURST_HIT;}
#endif
   }
   if(!core.outcome){std::fprintf(stderr,"Unfinished signature probe round\n");return 1;}
   stats.rounds++;stats.p1+=core.outcome==1;stats.p2+=core.outcome==2;stats.draws+=core.outcome==3;stats.ticks+=core.tick;stats.digest=(stats.digest^core.checksum())*16777619u;total++;
  }
  brakes+=stats.brakes;armed+=stats.armed;hits+=stats.burst_hits;
 }
 std::printf("{\n\"status\":\"passed\",\n\"rounds\":%d,\n\"seeds_per_pair_and_level\":20,\n\"difficulty_levels\":3,\n\"air_brakes\":%d,\n\"orbs_armed\":%d,\n\"burst_hits\":%d,\n\"pairs\":[\n",total,brakes,armed,hits);
 for(int a=0;a<character_count;a++)for(int b=0;b<character_count;b++){
  const auto&s=pairs[a][b];std::printf("{\"p1\":%d,\"p2\":%d,\"rounds\":%d,\"p1_wins\":%d,\"p2_wins\":%d,\"draws\":%d,\"total_active_ticks\":%d,\"replay_digest\":%u,\"air_brakes\":%d,\"orbs_armed\":%d,\"burst_hits\":%d}%s\n",a,b,s.rounds,s.p1,s.p2,s.draws,s.ticks,s.digest,s.brakes,s.armed,s.burst_hits,a==character_count-1&&b==character_count-1?"":",");
 }
 std::puts("],\n\"method\":\"Seeded CPU inputs across every ordered matchup and all three difficulties. Human matchup balance remains unverified.\"\n}");
}
