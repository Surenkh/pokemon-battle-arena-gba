#pragma once
namespace pkmn {
enum Technique { TECHNIQUE_NONE, TECHNIQUE_AIR_BRAKE, TECHNIQUE_SHADOW_BURST };
inline constexpr int air_brake_stamina_cost=8;
inline constexpr int air_brake_duration_ticks=12;
inline constexpr int air_brake_max_fall_speed_q8=128;
inline constexpr int shadow_burst_stamina_cost=8;
inline constexpr int shadow_burst_startup_ticks=12;
inline constexpr int shadow_burst_active_ticks=4;
inline constexpr int shadow_burst_damage=10;
inline constexpr int shadow_burst_hitbox[4]={-14,-10,14,10};
}
