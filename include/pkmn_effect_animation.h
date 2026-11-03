#pragma once
namespace pkmn {
inline constexpr int effect_frames = 4;
inline constexpr int effect_frame_ticks = 3;
inline constexpr int effect_lifetime_ticks = effect_frames * effect_frame_ticks;
// CombatCore increments tick after spawning; the first rendered age is zero.
inline int projectile_effect_frame(int tick, int born) {
    int age = tick - born - 1;
    return age > 0 ? (age / effect_frame_ticks) % effect_frames : 0;
}
}

namespace pkmn {
inline constexpr int super_effect_frames=8,super_effect_frame_ticks=2;
inline constexpr int power_punch_frames=6,super_impact_ticks=16;
inline int super_effect_frame(int tick,int born){int age=tick-born-1;return age>0?(age/super_effect_frame_ticks)%super_effect_frames:0;}
// Presentation follows the real startup/active/recovery windows.
inline int power_punch_frame(int age,int startup,int active){
 if(age<startup-2)return -1;
 if(age<startup)return age-startup+2;
 if(age<startup+active)return 2+(age-startup)*2/active;
 if(age<startup+active+4)return 4+(age-startup-active)/2;
 return -1;
}
}
