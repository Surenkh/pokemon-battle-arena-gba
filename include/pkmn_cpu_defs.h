#pragma once
namespace pkmn {
inline constexpr int cpu_history_ticks = 32;
inline constexpr int cpu_memory_actions = 6;
inline constexpr int cpu_memory_expire_ticks = 240;
inline constexpr int cpu_signature_zone_burst_percent = 10;
struct CpuDifficulty { int reaction_ticks,think_ticks,defense_percent,super_evade_percent,punish_percent,followup_percent,mistake_percent,hesitate_ticks,plan_ticks,adapt_percent; };
inline constexpr CpuDifficulty cpu_difficulties[3] = {
{20,9,40,45,25,25,22,12,90,0},
{12,6,72,72,65,72,10,6,72,6},
{8,4,90,88,90,90,4,4,60,12},
};
struct CpuStyle { int distance_px,melee_percent,fire_percent,guard_bonus,punish_bonus,recover_below,resume_at; };
inline constexpr CpuStyle cpu_styles[13] = {
{24,88,28,0,4,18,38},
{72,65,82,0,0,22,44},
{60,65,70,12,10,24,44},
{34,85,42,4,8,22,42},
{25,85,42,0,8,18,38},
{32,78,30,3,15,18,38},
{78,64,88,4,6,24,44},
{54,72,76,2,4,24,46},
{27,94,35,8,12,20,42},
{62,72,83,4,7,24,44},
{70,62,88,1,5,26,46},
{36,88,61,7,16,22,45},
{30,91,44,10,10,24,46},
};
}
