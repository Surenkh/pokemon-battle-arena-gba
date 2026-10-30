// Reproducible comparisons against a frozen v0.5 Normal CPU on both sides.
#include "pkmn_combat_core.h"
#include "../tests/support/v05_cpu.h"
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
using namespace pkmn;
struct Stats {
 int rounds=0,wins=0,losses=0,draws=0,timeouts=0,ticks=0,low=0,damage=0,taken=0;
 int melee=0,shots=0,blocks=0,followups=0,punishes=0,defenses=0,hesitations=0;
};
int main(int argc,char**argv){
 const int first_seed=argc>1?std::atoi(argv[1]):1;
 Stats levels[3],fighters[3][6];uint32_t digest=2166136261u;
 for(int level=0;level<3;++level)for(int a=0;a<6;++a)for(int b=0;b<6;++b)
 for(int seed=first_seed;seed<first_seed+10;++seed)for(int side=0;side<2;++side){
  CombatCore c;c.reset(side?b:a,side?a:b,seed);CpuController candidate;V05CpuController frozen;
  candidate.reset(level,seed*23);frozen.reset(1,seed*37);
  Stats s;
  for(int frame=0;frame<7000&&!c.outcome;++frame){
   int tick=c.tick,hp=c.f[side].hp,other=c.f[1-side].hp;
   Input left=side?frozen.input(c,0):candidate.input(c,0),right=side?candidate.input(c,1):frozen.input(c,1);
   c.frame(left,right);s.damage+=other-c.f[1-side].hp;s.taken+=hp-c.f[side].hp;
   if(c.tick!=tick){++s.ticks;s.low+=c.f[side].sta<fire_stamina_cost*60;}
   for(int n=0;n<c.event_count;++n){const auto&e=c.events[n];
    s.blocks+=e.kind==EV_BLOCK&&e.target==side;
    if(e.owner!=side)continue;
    s.melee+=e.kind==EV_MELEE;s.shots+=e.kind==EV_SHOT;s.followups+=e.kind==EV_FOLLOWUP;s.punishes+=e.kind==EV_PUNISH;
   }
  }
  if(!c.outcome){std::fprintf(stderr,"Unfinished CPU probe\n");return 1;}
  s.rounds=1;s.wins=c.outcome==side+1;s.losses=c.outcome==2-side;s.draws=c.outcome==3;s.timeouts=c.remaining<=0;
  s.defenses=candidate.diagnostics.defenses;s.hesitations=candidate.diagnostics.hesitations;
  digest=(digest^c.checksum())*16777619u;
  for(auto*target:{&levels[level],&fighters[level][a]}){
#define ADD(k) target->k+=s.k
   ADD(rounds);ADD(wins);ADD(losses);ADD(draws);ADD(timeouts);ADD(ticks);ADD(low);ADD(damage);ADD(taken);
   ADD(melee);ADD(shots);ADD(blocks);ADD(followups);ADD(punishes);ADD(defenses);ADD(hesitations);
#undef ADD
  }
 }
 auto print=[](const char*name,const Stats&s){std::printf("{\"name\":\"%s\",\"rounds\":%d,\"wins\":%d,\"losses\":%d,\"draws\":%d,\"win_percent\":%.2f,\"timeouts\":%d,\"active_ticks\":%d,\"low_stamina_tick_percent\":%.2f,\"damage_dealt\":%d,\"damage_taken\":%d,\"melee_started\":%d,\"shots_released\":%d,\"blocked_hits\":%d,\"followups_started\":%d,\"punish_hits\":%d,\"defense_requests\":%d,\"hesitations\":%d}",name,s.rounds,s.wins,s.losses,s.draws,100.*s.wins/s.rounds,s.timeouts,s.ticks,100.*s.low/s.ticks,s.damage,s.taken,s.melee,s.shots,s.blocks,s.followups,s.punishes,s.defenses,s.hesitations);};
 std::printf("{\n\"first_seed\":%d,\n\"status\":\"passed\",\"rounds\":2160,\"replay_digest\":%u,\"method\":\"Candidate at each difficulty versus frozen v0.5 Normal on the v0.6 combat core; all 36 ordered matchups, 10 fixed seeds, both sides. No stat differences. CPU evidence does not establish human balance.\",\n\"difficulties\":[",first_seed,digest);
 const char*names[3]={"Easy","Normal","Hard"};
 for(int l=0;l<3;++l){if(l)std::printf(",");print(names[l],levels[l]);}
 std::printf("],\n\"fighters_by_difficulty\":[");
 for(int l=0;l<3;++l){if(l)std::printf(",");std::printf("{\"difficulty\":\"%s\",\"fighters\":[",names[l]);for(int a=0;a<6;++a){if(a)std::printf(",");print(characters[a].name,fighters[l][a]);}std::printf("]}");}
 std::puts("]\n}");
}
