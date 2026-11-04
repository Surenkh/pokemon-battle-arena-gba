#pragma once

#include "pkmn_combat_core.h"
#include "pkmn_roster_navigation.h"
#include "pkmn_arenas.h"

namespace pkmn {

inline constexpr uint32_t versus_input_magic = 0x56534931;
inline constexpr int versus_input_timeout = 3;
inline constexpr int mode_count = 8;

// Keep existing mode IDs stable while putting Local Versus beside Practice.
inline int adjacent_mode(int mode, bool forward) {
    constexpr int next[] = {1, 2, 7, 4, 5, 6, 0, 3};
    constexpr int previous[] = {6, 0, 1, 7, 3, 4, 5, 2};
    return forward ? next[mode] : previous[mode];
}

struct VersusMailbox {
    uint32_t magic = 0;
    uint32_t sequence = 0;
    uint32_t held[2] = {};
};
static_assert(sizeof(VersusMailbox) == 16);

class VersusInput {
    uint32_t last_sequence = 0;
    uint16_t previous[2] = {};
    int missing_frames = versus_input_timeout;
    bool armed = false;
    Input inputs[2] = {};

public:
    bool connected() const { return missing_frames < versus_input_timeout; }
    Input player(int who) const { return inputs[who]; }

    void poll(const volatile VersusMailbox& mailbox) {
        const uint32_t sequence = mailbox.sequence;
        const uint32_t magic = mailbox.magic;
        const uint16_t held[2] = {
            uint16_t(mailbox.held[0] & 1023),
            uint16_t(mailbox.held[1] & 1023)
        };
        const bool complete = sequence && !(sequence & 1) &&
            sequence == mailbox.sequence && magic == versus_input_magic;
        const bool fresh = complete && sequence != last_sequence;
        const bool was_connected = connected();

        if (fresh) {
            last_sequence = sequence;
            missing_frames = 0;
        } else if (missing_frames < versus_input_timeout) {
            ++missing_frames;
        }

        inputs[0] = inputs[1] = Input{};
        if (!connected()) {
            previous[0] = previous[1] = 0;
            armed = false;
            return;
        }
        if (!fresh) {
            if (armed) {
                inputs[0].held = previous[0];
                inputs[1].held = previous[1];
            }
            return;
        }
        if (!was_connected) armed = false;
        if (!armed) {
            armed = !held[0] && !held[1];
            previous[0] = held[0];
            previous[1] = held[1];
            return;
        }
        for (int who = 0; who < 2; ++who) {
            inputs[who].held = held[who];
            inputs[who].pressed = held[who] & ~previous[who];
            previous[who] = held[who];
        }
    }
};

class LocalVersus {
public:
    bool active = false;
    int fighters[2] = {0, 1};
    bool ready[2] = {};
    int wins[2] = {};
    int arena = WOODLAND;
    int result = 0;

    void open() {
        active = true;
        reset_set();
    }

    void reset_set() {
        wins[0] = wins[1] = 0;
        result = 0;
        unready();
    }

    void unready() { ready[0] = ready[1] = false; }
    bool both_ready() const { return ready[0] && ready[1]; }
    bool complete() const { return wins[0] == 2 || wins[1] == 2; }

    void choose(int who, Input keys) {
        if (keys.pressed & B) {
            ready[who] = false;
            return;
        }
        if (ready[who]) return;
        int& fighter = fighters[who];
        if (keys.pressed & LEFT) fighter = (fighter + character_count - 1) % character_count;
        else if (keys.pressed & RIGHT) fighter = (fighter + 1) % character_count;
        else if (keys.pressed & UP) fighter = roster_up(fighter);
        else if (keys.pressed & DOWN) {
            fighter = roster_has_down(fighter) ? fighter + roster_columns : fighter % roster_columns;
        }
        if (keys.pressed & A) ready[who] = true;
    }

    void finish_round(int outcome) {
        result = outcome;
        if (outcome == 1 || outcome == 2) ++wins[outcome - 1];
    }
};

}

extern "C" {
    extern volatile pkmn::VersusMailbox pkmn_versus_input;
    extern volatile int pkmn_versus_debug[32];
    struct PkmnVersusDescriptor {
        char magic[8];
        uint32_t protocol;
        volatile pkmn::VersusMailbox* mailbox;
        uint32_t mailbox_bytes;
    };
    extern const PkmnVersusDescriptor pkmn_versus_descriptor;
}
