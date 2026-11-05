#include "pkmn_rival_cup.h"
#include "pkmn_arcade.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
using namespace pkmn;
static int checks = 0;
static void check(bool value, const char* message) {
    ++checks; if (!value) { std::fprintf(stderr, "%s\n", message); std::exit(1); }
}
static void event(CombatCore& core, EventKind kind, int owner, int target, bool super, int damage, bool followup) {
    ++core.event_frame; core.event_count = 1;
    core.events[0] = {kind, owner, target, 0, 0, super, 1, damage, followup};
}
int main() {
    for (int route = 0; route < cup_count; ++route) for (int fighter = 0; fighter < character_count; ++fighter) for (int level = 0; level < 3; ++level)
    for (int first = 0; first < cup_challengers; ++first) for (int second = 0; second < cup_challengers; ++second) {
        if (first == second) continue;
        RivalCupRun run; run.begin(route, fighter, level); CupProgress progress; CombatCore core;
        const auto& cup = cups[route];
        core.reset(fighter, cup.challengers[first].fighter); const auto checksum = core.checksum();
        check(run.active && !run.finished && !run.rival() && run.continues == 1 && run.style == 0, "Fresh Cup run waits for a choice");
        check(!run.choose_challenger(-1) && !run.choose_challenger(cup_challengers), "Out-of-range rivals and the Captain cannot be chosen");
        run.finish_round(1); check(!run.stage, "A choice is required to advance");
        for (int n = 0; n < cup_fights; ++n) {
            if (n < 2) {
                const int slot = n == 0 ? first : second;
                check(run.choose_challenger(slot) && !run.choose_challenger((slot + 1) % cup_challengers), "Choice locks until a win");
                check(run.rival() == &cup.challengers[slot] && !run.captain_round(), "Chosen challenger and authored location");
            } else {
                check(run.captain_round() && run.rival() == &cup.captain, "Captain follows two distinct wins");
                check(!run.choose_challenger(3 - first - second), "Remaining challenger cannot replace the Captain");
            }
            run.begin_round();
            for (int hit = 0; hit < 7; ++hit) {
                event(core, route == 1 ? EV_BLOCK : EV_HIT, route == 1 ? 1 : 0, route == 1 ? 0 : 1, route == 2, 5, route == 0);
                run.observe(core); int count = run.style; auto ticks = run.ticks;
                run.observe(core); check(run.style == count && run.ticks == ticks, "One observation per displayed combat frame");
            }
            run.finish_round(1); run.finish_round(1); check(run.stage == n + 1, "Duplicate completion does not advance twice");
            if (n < 2) {
                check(!run.can_choose(first) && !progress.complete(run), "Defeated challenger stays locked and early wins give no award");
            }
        }
        check(core.checksum() == checksum, "Progression observer cannot change combat");
        check(run.finished && run.styled() && run.style == cup.target, "Three victories and bounded style goal");
        check(progress.complete(run) && !progress.complete(run), "Only one award for a finished run");
        auto record = progress.record(route, level);
        check(record.clears == 1 && record.style == 1 && record.perfect == 1 && record.best_ticks == run.ticks && run.new_best, "Clear/style/perfect/time awarded");
        Progress original; original.complete(ArcadeRun());
        check(!original.record(fighter, level).clears && !original.no_continue(fighter), "Cup awards have no Arcade dependency");
        auto copy = encode_cup(progress, 7);
        check(valid_cup(copy) && decode_cup(copy).record(route, level).best_ticks == run.ticks, "All Cup/fighter/level combinations round trip");
    }
    for (int route = 0; route < cup_count; ++route) {
        RivalCupRun run; run.begin(route, 0, 1); run.choose_challenger(2);
        CombatCore core; core.reset(0, run.rival()->fighter);
        for (auto kind : {EV_MELEE, EV_FOLLOWUP, EV_BREAK, EV_RELEASE}) {
            event(core, kind, 0, 1, true, 5, true); run.observe(core);
        }
        event(core, EV_HIT, 1, 0, true, 5, true); run.observe(core);
        event(core, EV_BLOCK, 0, 1, false, 0, false); run.observe(core);
        event(core, EV_HIT, 0, 1, false, 0, true); run.observe(core);
        check(!run.style, "Whiffs, attempts, enemy hits, outgoing blocks, zero damage do not earn style");
        run.finish_round(3); int stage = run.stage; auto time = run.ticks;
        check(run.retry(3) && run.continues == 1 && run.stage == stage && run.ticks == time, "Draws retry for free without clearing time");
        check(run.rival_slot == 2 && !run.defeated && !run.choose_challenger(1), "Retry keeps the same challenger");
        run.begin_round(); check(run.retry(2) && !run.continues && run.used == 1 && !run.retry(2), "One paid continue, including pause restart");
        run.begin_round(); run.finish_round(2); check(run.stage == stage && !run.finished, "Loss never advances");
        CupProgress progress; check(!progress.complete(run), "Failed run grants no Cup award");
        run.begin_round(); run.finish_round(1);
        check(run.choose_challenger(0), "Winner chooses again after the retried challenger");
        run.begin_round(); run.finish_round(1);
        run.begin_round(); run.finish_round(1);
        check(progress.complete(run) && progress.record(route, 1).perfect == 0, "Retry clear cannot claim perfect");
        check(!run.retry(2) && !run.retry(3), "Finished run cannot retry a rival");
    }
    // Actual resolved hits, rather than button requests, feed each observer.
    for (int route = 0; route < 3; ++route) {
        RivalCupRun run; run.begin(route, 4, 0); run.choose_challenger(0); CombatCore core; core.reset(4, 1);
        core.f[0].x = 90 * 256; core.f[1].x = 112 * 256; core.f[0].sp = 100;
        for (int n = 0; n < 200; ++n) {
            Input a{}, b{};
            if (route == 1) { a.held = L; if (n == 0) b = {A, A}; }
            else if (route == 2) { if (n == 0) a = {R, R}; }
            else { if (n == 0) a = {A, A}; else if (core.can_followup(0, B)) a = {B, B}; }
            core.frame(a, b); run.observe(core);
        }
        check(run.style >= 1, "Real combat event earns each Cup objective");
    }
    CupProgress progress; progress.records[0] = {2, 1, 2, 1234};
    const auto old = encode_cup(progress, 41); progress.records[0] = {3, 2, 2, 1000}; const auto next = encode_cup(progress, 42);
    CupSave empty;
    check(newest_cup(empty, empty) == -1 && newest_cup(old, empty) == 0 && newest_cup(empty, next) == 1 && newest_cup(old, next) == 1, "Newest valid journal copy");
    for (unsigned bit = 0; bit < sizeof(CupSave) * 8; ++bit) {
        auto bad = next; bad.words[bit / 32] ^= 1u << (bit % 32);
        check(!valid_cup(bad) && newest_cup(old, bad) == 0, "Every single-bit corruption recovers the older copy");
    }
    for (size_t bytes = 0; bytes < sizeof(CupSave); ++bytes) {
        CupSave partial; std::memcpy(&partial, &next, bytes);
        check(newest_cup(old, partial) == 0 || std::memcmp(&partial, &next, sizeof(next)) == 0, "Truncated write cannot supersede good save");
    }
    for (int field = 0; field < 4; ++field) {
        auto bad = old; bad.words[3 + field] = field == 0 ? 10000 : field == 3 ? 0 : 3;
        bad.words[cup_save_words - 1] = cup_checksum(bad); check(!valid_cup(bad), "Checksummed inconsistent records rejected");
    }
    check(newest_cup(encode_cup(progress, 0xffffffffu), encode_cup(progress, 0)) == 1, "Sequence wraps safely");
    check(cup_save_offsets[0] >= save_offsets[1] + int(sizeof(SaveSlot)) && cup_save_offsets[1] >= cup_save_offsets[0] + int(sizeof(CupSave)), "Cup slots do not overlap legacy or current saves");
    std::printf("Cup: %d checks passed (702 ordered runs, real event goals, journal corruption/recovery)\n", checks);
}
