#pragma once
#include "pkmn_rival_cup.h"
#include "bn_sram.h"
#include "bn_span.h"

namespace pkmn {
class CupStore {
    int slot = -1;
    uint32_t sequence = 0;
public:
    bool failed = false;
    CupProgress load() {
        CupSave copies[2];
        for (int i = 0; i < 2; ++i) bn::sram::read_offset(copies[i], cup_save_offsets[i]);
        slot = newest_cup(copies[0], copies[1]);
        if (slot < 0) return CupProgress();
        sequence = copies[slot].words[2]; return decode_cup(copies[slot]);
    }
    bool save(const CupProgress& progress) {
        const int target = slot == 0 ? 1 : 0, offset = cup_save_offsets[target];
        const auto next = encode_cup(progress, sequence + 1);
        const uint32_t invalid = 0;
        bn::sram::write_offset(invalid, offset);
        bn::span<const uint8_t> payload(reinterpret_cast<const uint8_t*>(&next), sizeof(CupSave) - 4);
        bn::sram::write_span_offset(payload, offset);
        bn::sram::write_offset(next.words[cup_save_words - 1], offset + sizeof(CupSave) - 4);
        CupSave verify; bn::sram::read_offset(verify, offset);
        failed = !valid_cup(verify);
        for (int i = 0; i < cup_save_words; ++i) if (verify.words[i] != next.words[i]) failed = true;
        if (!failed) { slot = target; ++sequence; }
        return !failed;
    }
};
}
