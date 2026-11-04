#pragma once

#include "pkmn_combat_core.h"
#include "pkmn_cup_defs.h"
#include <stdint.h>

namespace pkmn {

// A run owns only progression. Rivals use the ordinary CombatCore and CPU.
struct RivalCupRun {
    bool active = false, finished = false, awarded = false, new_best = false;
    int cup = 0, fighter = 0, difficulty = 1, stage = 0, last_stage = 0;
    int continues = 1, used = 0, result = 0, style = 0;
    uint32_t ticks = 0, seen_frame = ~0u;

    void begin(int route, int who, int level) {
        *this = RivalCupRun();
        active = true; cup = route; fighter = who; difficulty = level;
    }
    const CupDefinition& definition() const { return cups[cup]; }
    const CupRival& rival() const { return definition().rivals[stage < cup_rivals ? stage : cup_rivals - 1]; }
    bool styled() const { return style >= definition().target; }
    // Reset the event cursor every round; CombatCore starts its cursor again.
    void begin_round() { seen_frame = ~0u; result = 0; }
    void observe(const CombatCore& core) {
        if (!active || finished || result || seen_frame == core.event_frame) return;
        seen_frame = core.event_frame;
        if (ticks != 0xffffffffu) ++ticks;
        for (int n = 0; n < core.event_count && !styled(); ++n) {
            const auto& event = core.events[n];
            const bool hit = event.kind == EV_HIT && event.owner == 0 && event.target == 1 && event.damage > 0;
            if ((cup == 0 && hit && event.followup) ||
                (cup == 1 && event.kind == EV_BLOCK && event.target == 0 && event.owner == 1) ||
                (cup == 2 && hit && event.super)) ++style;
        }
    }
    void finish_round(int outcome) {
        if (!active || finished || result || outcome < 1 || outcome > 3) return;
        result = outcome; last_stage = stage;
        if (outcome == 1 && ++stage == cup_rivals) finished = true;
    }
    bool retry(int outcome) {
        if (!active || finished || (outcome != 2 && outcome != 3)) return false;
        if (outcome == 2) {
            if (!continues) return false;
            --continues; ++used;
        }
        return true;
    }
};

struct CupRecord { uint32_t clears = 0, style = 0, perfect = 0, best_ticks = 0; };
struct CupProgress {
    CupRecord records[cup_count * 3] = {};
    const CupRecord& record(int cup, int level) const { return records[cup * 3 + level]; }
    bool complete(RivalCupRun& run) {
        if (!run.active || !run.finished || run.awarded || !run.ticks) return false;
        auto& record = records[run.cup * 3 + run.difficulty];
        if (record.clears < 9999) ++record.clears;
        if (run.styled() && record.style < record.clears) ++record.style;
        if (!run.used && record.perfect < record.clears) ++record.perfect;
        run.new_best = !record.best_ticks || run.ticks < record.best_ticks;
        if (run.new_best) record.best_ticks = run.ticks;
        run.awarded = true;
        return true;
    }
};

// Independent journal: schema-4 Arcade/mastery/audio saves stay byte compatible.
inline constexpr int cup_save_offsets[2] = {4096, 5120};
inline constexpr int cup_save_words = 4 + cup_count * 3 * 4;
inline constexpr uint32_t cup_save_magic = 0x43555031;
struct CupSave { uint32_t words[cup_save_words] = {}; };
static_assert(sizeof(CupSave) == 160);
inline uint32_t cup_checksum(const CupSave& copy) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < cup_save_words - 1; ++i) for (int byte = 0; byte < 4; ++byte) {
        hash ^= (copy.words[i] >> (byte * 8)) & 255; hash *= 16777619u;
    }
    return hash;
}
inline CupSave encode_cup(const CupProgress& progress, uint32_t sequence) {
    CupSave copy; copy.words[0] = cup_save_magic; copy.words[1] = 1; copy.words[2] = sequence;
    for (int i = 0; i < cup_count * 3; ++i) {
        const auto& record = progress.records[i];
        copy.words[3 + i * 4] = record.clears; copy.words[4 + i * 4] = record.style;
        copy.words[5 + i * 4] = record.perfect; copy.words[6 + i * 4] = record.best_ticks;
    }
    copy.words[cup_save_words - 1] = cup_checksum(copy); return copy;
}
inline bool valid_cup(const CupSave& copy) {
    if (copy.words[0] != cup_save_magic || copy.words[1] != 1 || copy.words[cup_save_words - 1] != cup_checksum(copy)) return false;
    for (int i = 0; i < cup_count * 3; ++i) {
        const auto* record = &copy.words[3 + i * 4];
        if (record[0] > 9999 || record[1] > record[0] || record[2] > record[0] || bool(record[0]) != bool(record[3])) return false;
    }
    return true;
}
inline int newest_cup(const CupSave& a, const CupSave& b) {
    const bool av = valid_cup(a), bv = valid_cup(b);
    if (!av) return bv ? 1 : -1;
    if (!bv) return 0;
    const uint32_t delta = b.words[2] - a.words[2]; return delta && delta < 0x80000000u ? 1 : 0;
}
inline CupProgress decode_cup(const CupSave& copy) {
    CupProgress progress;
    for (int i = 0; i < cup_count * 3; ++i) progress.records[i] = {
        copy.words[3 + i * 4], copy.words[4 + i * 4], copy.words[5 + i * 4], copy.words[6 + i * 4]};
    return progress;
}

}
