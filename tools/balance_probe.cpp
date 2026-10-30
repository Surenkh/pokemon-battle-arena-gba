#include "pkmn_combat_core.h"
#include <algorithm>
#include <cstdio>
#include <vector>
using namespace pkmn;

struct Stats {
    int matches=0,wins=0,losses=0,draws=0,ticks=0,low=0,neutral=0,waiting=0;
    int dealt=0,taken=0,melee=0,shots=0,hits=0,blocks=0,charges=0,releases=0;
};

// Deterministic CPU evidence, not a human balance score. No writes to gameplay.
int main(int argc,char**argv) {
    Stats stats[6];std::vector<int> lengths;int timeouts=0,rounds=0;
    uint32_t digest=2166136261u;
    for(int level=0;level<3;++level)for(int a=0;a<6;++a)for(int b=0;b<6;++b)
    for(int seed=1;seed<=20;++seed) {
        CombatCore c;c.reset(a,b,seed);CpuController left,right;
        left.reset(level,seed*23);right.reset(level,seed*37);
        for(int display=0;display<7000&&!c.outcome;++display) {
            int old_tick=c.tick,hp[2]={c.f[0].hp,c.f[1].hp};
            c.frame(left.input(c,0),right.input(c,1));
            for(int i=0;i<2;++i) {
                auto&s=stats[c.f[i].character];s.taken+=hp[i]-c.f[i].hp;
                s.dealt+=hp[1-i]-c.f[1-i].hp;
                if(c.tick!=old_tick) {
                    ++s.ticks;s.low+=c.f[i].sta<fire_stamina_cost*60;
                    if(c.f[i].action==NONE){++s.neutral;s.waiting+=c.f[i].sta<fire_stamina_cost*60;}
                }
            }
            for(int n=0;n<c.event_count;++n) {
                const auto&e=c.events[n];auto&s=stats[c.f[e.owner].character];
                s.melee+=e.kind==EV_MELEE;s.shots+=e.kind==EV_SHOT;
                s.hits+=e.kind==EV_HIT;s.blocks+=e.kind==EV_BLOCK;
                s.charges+=e.kind==EV_CHARGE;s.releases+=e.kind==EV_RELEASE;
            }
        }
        if(!c.outcome){std::fprintf(stderr,"Unfinished probe round\n");return 1;}
        ++rounds;timeouts+=c.remaining<=0;lengths.push_back(c.tick);
        digest=(digest^c.checksum())*16777619u;
        for(int i=0;i<2;++i) {
            auto&s=stats[c.f[i].character];++s.matches;
            s.draws+=c.outcome==3;s.wins+=c.outcome==i+1;s.losses+=c.outcome==2-i;
        }
    }
    std::sort(lengths.begin(),lengths.end());long long total=0;
    for(int n:lengths)total+=n;
    std::printf("{\n  \"label\":\"%s\",\n  \"rounds\":%d,\n  \"seeds_per_ordered_matchup_and_level\":20,\n  \"difficulty_levels\":3,\n  \"timeouts\":%d,\n  \"median_active_ticks\":%d,\n  \"mean_active_ticks\":%.2f,\n  \"replay_digest\":%u,\n  \"characters\":[\n",argc>1?argv[1]:"candidate",rounds,timeouts,lengths[lengths.size()/2],double(total)/rounds,digest);
    for(int ch=0;ch<6;++ch) {
        const auto&s=stats[ch];
        std::printf("    {\"character\":\"%s\",\"appearances\":%d,\"wins\":%d,\"losses\":%d,\"draws\":%d,\"win_percent\":%.2f,\"low_stamina_tick_percent\":%.2f,\"neutral_wait_percent\":%.2f,\"damage_dealt\":%d,\"damage_taken\":%d,\"melee_started\":%d,\"shots_released\":%d,\"clean_hits\":%d,\"blocked_hits\":%d,\"super_starts\":%d,\"super_releases\":%d}%s\n",characters[ch].name,s.matches,s.wins,s.losses,s.draws,100.0*s.wins/s.matches,100.0*s.low/s.ticks,s.neutral?100.0*s.waiting/s.neutral:0.,s.dealt,s.taken,s.melee,s.shots,s.hits,s.blocks,s.charges,s.releases,ch==5?"":",");
    }
    std::puts("  ],\n  \"method\":\"2160 CPU-vs-CPU rounds across all ordered matchups and all difficulties. Mirrored roster order and fixed seeds; CPU policies may favor particular styles. Human balance remains unverified.\"\n}");
}
