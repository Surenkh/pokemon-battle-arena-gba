#pragma once
#include "pkmn_combat_core.h"

namespace pkmn {
// Observes a complete nearby jump/squeeze dodge without changing combat.
class SuperEvasion {
    uint32_t tracked_super = 0;
    bool approached = false;
    bool evading = false;
    bool awarded = false;

public:
    void reset() { *this = SuperEvasion(); }

    bool observe(const CombatCore& core) {
        const auto& shot = core.p[3];
        const auto& player = core.f[0];
        if(!shot.active || !shot.super || shot.owner != 1) return false;
        if(tracked_super != shot.id) {
            tracked_super = shot.id;
            approached = evading = awarded = false;
        }
        int dx = shot.x - player.x;
        if((shot.vx < 0 && dx > 0) || (shot.vx > 0 && dx < 0)) approached = true;
        int half_width = collision_super_hitbox[2] + collision_standing_hurtbox[2];
        int proximity = (half_width + 8) * 256;
        if(approached && dx >= -proximity && dx <= proximity &&
           (!core.grounded(player) || core.low(player))) evading = true;
        bool passed = shot.vx < 0 ? dx < -half_width * 256 : dx > half_width * 256;
        // A hit, including an invulnerability contact, consumes the shot first.
        if(approached && evading && passed && !awarded) {
            awarded = true;
            return true;
        }
        return false;
    }
};
}
