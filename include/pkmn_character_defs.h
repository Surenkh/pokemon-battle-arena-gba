#pragma once
#include <stdint.h>
namespace pkmn {
struct CharacterDef { const char* name; int hp,sta,regen,speed,melee_damage,fire_damage,super_damage,melee_start,melee_active,melee_total,fire_start,fire_total,shot_speed; int melee_reach,melee_lunge,guard_percent,fire_guard_percent,fire_knockback,punish_bonus; bool melee_followup; const char* role; const char* trait; const char* tip1; const char* tip2; };
inline constexpr CharacterDef characters[6] = {
{"PIKACHU",95,105,26,384,8,12,32,4,3,18,8,26,896,21,128,100,100,512,0,0,"RUSH AND PRESSURE","QUICK STEP","MELEE STEPS FORWARD","FAST APPROACH. COMMIT TO A."},
{"CHARMANDER",100,90,24,307,10,14,36,6,3,23,10,32,768,21,0,100,125,512,0,0,"HEAVY FIRE","EMBER PRESSURE","FIRE DRAINS EXTRA GUARD","SLOW RECOVERY. KEEP SPACE."},
{"SQUIRTLE",115,110,22,269,7,11,30,6,3,22,10,30,704,21,0,80,100,768,0,0,"DEFENSE AND SPACE","SHELL CONTROL","EFFICIENT BLOCK. WATER PUSH.","SUPERS STILL IGNORE BLOCK."},
{"BULBASAUR",120,100,20,243,9,12,34,7,3,25,11,32,832,32,0,100,100,512,0,0,"REACH AND CONTROL","VINE REACH","LONG MELEE REACH","SLOW START. HOLD DISTANCE."},
{"EEVEE",100,115,28,358,7,10,31,4,3,17,8,26,832,21,0,100,100,512,0,1,"FLEXIBLE FOLLOW-UPS","ADAPTABLE","CLEAN A CAN FOLLOW WITH A/B","A TO A NEEDS CLOSE RANGE."},
{"MEOWTH",90,100,25,397,8,11,33,3,3,16,8,27,768,21,0,100,100,512,3,0,"CLOSE COUNTERATTACK","PUNISH","EXTRA MELEE DAMAGE IN RECOVERY","CATCH A MISSED ENEMY ATTACK."},
};
inline constexpr int world_width = 240;
inline constexpr int world_height = 160;
inline constexpr int world_floor_y = 120;
inline constexpr int world_min_fighter_x = 32;
inline constexpr int world_max_fighter_x = 208;
inline constexpr int world_player_spawn_x = 64;
inline constexpr int world_cpu_spawn_x = 176;
inline constexpr int resources_stamina_subunits_per_point = 60;
inline constexpr int resources_stamina_regen_delay_ticks = 24;
inline constexpr int resources_block_drain_subunits_per_tick = 12;
inline constexpr int resources_block_enter_min_stamina = 8;
inline constexpr int resources_super_power_max = 100;
inline constexpr int resources_super_power_start = 0;
inline constexpr int resources_melee_clean_hit_power = 18;
inline constexpr int resources_fire_clean_hit_power = 22;
inline constexpr int resources_guard_cost_numerator = 5;
inline constexpr int resources_guard_cost_denominator = 4;
inline constexpr int resources_cpu_recover_below_stamina = 12;
inline constexpr int resources_cpu_resume_stamina = 32;
inline constexpr int motion_gravity_q8 = 64;
inline constexpr int motion_jump_velocity_q8 = -1088;
inline constexpr int motion_max_fall_velocity_q8 = 1280;
inline constexpr int motion_air_speed_numerator = 3;
inline constexpr int motion_air_speed_denominator = 4;
inline constexpr int motion_normal_knockback_q8 = 512;
inline constexpr int motion_super_knockback_q8 = 768;
inline constexpr int motion_knockback_decay_q8 = 64;
inline constexpr int motion_ground_acceleration_q8 = 96;
inline constexpr int motion_ground_braking_q8 = 96;
inline constexpr int motion_ground_turn_acceleration_q8 = 144;
inline constexpr int motion_air_acceleration_q8 = 32;
inline constexpr int motion_air_drag_q8 = 8;
inline constexpr int motion_normal_hit_lift_q8 = -320;
inline constexpr int motion_super_hit_lift_q8 = -640;
inline constexpr int motion_landing_pose_ticks = 5;
inline constexpr int motion_walk_stride_q8 = 3072;
inline constexpr int reactions_normal_hitstun_ticks = 12;
inline constexpr int reactions_super_hitstun_ticks = 24;
inline constexpr int reactions_blockstun_ticks = 6;
inline constexpr int reactions_guard_break_ticks = 45;
inline constexpr int reactions_post_hit_invulnerability_ticks = 12;
inline constexpr int reactions_normal_hitstop_display_ticks = 4;
inline constexpr int reactions_super_hitstop_display_ticks = 8;
inline constexpr int reactions_block_hitstop_display_ticks = 2;
inline constexpr int input_buffer_ticks = 3;
inline constexpr int input_press_actions_repeat_while_held = 0;
inline constexpr int match_round_ticks = 3600;
inline constexpr int match_countdown_display_ticks = 180;
inline constexpr int match_resume_countdown_display_ticks = 60;
inline constexpr int match_result_delay_display_ticks = 60;
inline constexpr int match_cpu_mirror_matches = 0;
inline constexpr int followup_recovery_cut_ticks = 6;
inline constexpr int followup_escape_ticks = 2;
inline constexpr int practice_dummy_think_ticks = 6;
inline constexpr int practice_dummy_shot_interval_ticks = 90;
inline constexpr int practice_dummy_super_interval_ticks = 180;
inline constexpr int jump_stamina_cost = 18;
inline constexpr int melee_stamina_cost = 8;
inline constexpr int fire_stamina_cost = 20;
inline constexpr int fire_lifetime_ticks = 60;
inline constexpr int squeeze_stamina_cost = 12;
inline constexpr int squeeze_startup_ticks = 3;
inline constexpr int squeeze_active_ticks = 24;
inline constexpr int squeeze_recovery_ticks = 9;
inline constexpr int squeeze_cooldown_after_end_ticks = 24;
inline constexpr int super_stamina_cost = 25;
inline constexpr int super_power_cost = 100;
inline constexpr int super_startup_ticks = 30;
inline constexpr int super_active_ticks = 1;
inline constexpr int super_recovery_ticks = 24;
inline constexpr int super_projectile_speed_q8 = 1280;
inline constexpr int super_lifetime_ticks = 60;
inline constexpr int super_blockable = 0;
inline constexpr int super_tracking = 0;
inline constexpr int super_power_gain = 0;
inline constexpr int collision_standing_hurtbox[4] = {-7,-28,7,0};
inline constexpr int collision_squeezed_hurtbox[4] = {-8,-10,8,0};
inline constexpr int collision_standing_pushbox[4] = {-8,-24,8,0};
inline constexpr int collision_squeezed_pushbox[4] = {-8,-10,8,0};
inline constexpr int collision_right_melee_hitbox[4] = {5,-24,21,0};
inline constexpr int collision_fire_hitbox[4] = {-3,-3,3,3};
inline constexpr int collision_super_hitbox[4] = {-6,-6,6,6};
inline constexpr int collision_fire_spawn_forward_px = 12;
inline constexpr int collision_super_spawn_forward_px = 14;
inline constexpr int collision_projectile_spawn_above_feet_px = 18;
}
