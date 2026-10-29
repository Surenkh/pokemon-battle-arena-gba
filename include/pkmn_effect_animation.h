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
