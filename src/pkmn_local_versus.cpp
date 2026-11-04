#include "pkmn_local_versus.h"

extern "C" {
    volatile pkmn::VersusMailbox pkmn_versus_input;
    volatile int pkmn_versus_debug[32] = {};
    // The Lua adapter locates this ROM descriptor instead of hard-coding RAM addresses.
    extern const PkmnVersusDescriptor pkmn_versus_descriptor = {
        {'P', 'K', 'M', 'N', 'V', 'S', '0', '1'},
        1,
        &pkmn_versus_input,
        sizeof(pkmn::VersusMailbox)
    };
}

#ifdef __arm__
static_assert(sizeof(PkmnVersusDescriptor) == 20);
#endif
