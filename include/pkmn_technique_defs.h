#pragma once
namespace pkmn {
enum Technique { TECHNIQUE_NONE, TECHNIQUE_AIR_BRAKE, TECHNIQUE_SHADOW_BURST, TECHNIQUE_VOLT_STEP, TECHNIQUE_SHELL_ROLL };
inline constexpr int air_brake_stamina_cost=8;
inline constexpr int air_brake_duration_ticks=12;
inline constexpr int air_brake_max_fall_speed_q8=128;
inline constexpr int shadow_burst_stamina_cost=8;
inline constexpr int shadow_burst_startup_ticks=12;
inline constexpr int shadow_burst_active_ticks=4;
inline constexpr int shadow_burst_damage=10;
inline constexpr int shadow_burst_hitbox[4]={-14,-10,14,10};
inline constexpr int volt_step_stamina_cost=12;
inline constexpr int volt_step_double_tap_ticks=10;
inline constexpr int volt_step_startup_ticks=2;
inline constexpr int volt_step_active_ticks=6;
inline constexpr int volt_step_recovery_ticks=8;
inline constexpr int volt_step_speed_q8=1024;
inline constexpr int volt_step_cooldown_ticks=24;
inline constexpr int shell_roll_stamina_cost=8;
inline constexpr int shell_roll_motion_ticks=10;
inline constexpr int shell_roll_speed_q8=512;
}
