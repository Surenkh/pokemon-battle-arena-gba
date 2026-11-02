#include "pkmn_practice.h"
#include "pkmn_mastery.h"
#include "pkmn_round_presentation.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace pkmn;
static int checks = 0;
#define CHECK(value) do { ++checks; if(!(value)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #value); std::exit(1); } } while(0)

static void begin(CombatCore& core, PracticeSession& practice, int fighter, DummyMode mode) {
    core.reset(fighter, (fighter + 1) % character_count, 91);
    core.training = true;
    practice.mode = mode;
    practice.begin_round();
    practice.prepare(core);
}

static void tick(CombatCore& core, PracticeSession& practice, Input player = {}, bool block = false) {
    uint32_t before = core.checksum();
    Input rival = block ? Input{L, L} : practice.input(core);
    CHECK(core.checksum() == before);
    core.frame(player, rival);
    before = core.checksum();
    practice.observe(core);
    CHECK(core.checksum() == before);
    auto stats = practice.stats;
    auto feedback = practice.feedback;
    practice.observe(core);
    CHECK(practice.stats.hits == stats.hits && practice.stats.followups == stats.followups);
    CHECK(practice.feedback == feedback);
    Progress progress;
    MasterySession mastery;
    mastery.observe(core, progress);
    CHECK(progress.mastery[core.f[0].character].followups == 0);
    CHECK(progress.mastery[core.f[0].character].evades == 0);
}

int main() {
    for(int fighter = 0; fighter < character_count; ++fighter) {
        CombatCore core;
        PracticeSession practice;
        begin(core, practice, fighter, DUMMY_FOLLOWUP);
        CHECK(core.f[1].x - core.f[0].x == 20 * 256);
        CHECK(core.f[0].sp == 100 && core.f[1].sp == 100);
        CHECK(core.remaining == match_round_ticks);
        tick(core, practice, {A, A});
        bool linked = false;
        for(int frame = 0; frame < 140 && !practice.completed(); ++frame) {
            if(!linked && core.can_followup(0, B)) {
                int stamina = core.f[0].sta;
                tick(core, practice, {B, B});
                CHECK(core.f[0].sta == stamina - fire_stamina_cost * 60);
                CHECK(!practice.completed());
                linked = true;
            } else tick(core, practice);
        }
        CHECK(linked && practice.feedback == FOLLOWUP_COMPLETE);
        CHECK(practice.stats.followups == 1 && practice.stats.best_chain == 2);
        CHECK(core.remaining == match_round_ticks);
        practice.begin_round(true);
        CHECK(!practice.completed() && practice.stats.followups == 1);
        practice.begin_round();
        CHECK(practice.stats.followups == 0);

        // The same real super must support both defensive techniques.
        for(int jump = 0; jump < 2; ++jump) {
            begin(core, practice, fighter, DUMMY_EVADE);
            bool pressed = false;
            for(int frame = 0; frame < 180 && !practice.completed(); ++frame) {
                Input player;
                if(!pressed && core.f[1].action == SUPER && core.age(core.f[1]) >= 35) {
                    player = {uint16_t(jump ? UP : DOWN), uint16_t(jump ? UP : DOWN)};
                    pressed = true;
                }
                tick(core, practice, player);
            }
            CHECK(pressed && practice.feedback == EVADE_COMPLETE);
            CHECK(practice.super_started && practice.super_released);
            CHECK(core.f[0].hp == characters[fighter].hp && core.f[1].sp == 0);
            for(int frame = 0; frame < 80; ++frame) tick(core, practice);
            CHECK(practice.feedback == EVADE_COMPLETE);
            practice.begin_round();
            CHECK(!practice.super_started && !practice.super_released && !practice.completed());
        }

        begin(core, practice, fighter, DUMMY_EVADE);
        for(int frame = 0; frame < 170; ++frame) tick(core, practice, {L, L});
        CHECK(practice.feedback == EVADE_HIT && !practice.completed());
        CHECK(core.f[0].hp == characters[fighter].hp - characters[(fighter + 1) % character_count].super_damage);

        begin(core, practice, fighter, DUMMY_FOLLOWUP);
        tick(core, practice, {A, A}, true);
        for(int frame = 0; frame < 40; ++frame) tick(core, practice, {}, true);
        CHECK(practice.feedback == DRILL_BLOCKED && !practice.completed());
        begin(core, practice, fighter, DUMMY_FOLLOWUP);
        tick(core, practice, {B, B});
        for(int frame = 0; frame < 50; ++frame) tick(core, practice);
        CHECK(practice.feedback == FOLLOWUP_NEEDS_MELEE && !practice.completed());
        begin(core, practice, fighter, DUMMY_FOLLOWUP);
        core.f[0].sta = 0;
        tick(core, practice, {B, B});
        CHECK(practice.feedback == DRILL_NO_STAMINA && !practice.completed());
    }

    CombatCore core;
    PracticeSession practice;
    begin(core, practice, 0, DUMMY_FOLLOWUP);
    core.f[1].hp = characters[0].melee_damage + characters[0].fire_damage;
    tick(core, practice, {A, A});
    bool sent = false;
    for(int frame = 0; frame < 140 && !core.outcome; ++frame) {
        if(!sent && core.can_followup(0, B)) {
            tick(core, practice, {B, B});
            sent = true;
        } else tick(core, practice);
    }
    CHECK(core.outcome == 1 && core.p[0].id == 0 && practice.completed());

    begin(core, practice, 4, DUMMY_FOLLOWUP);
    core.f[1].x = core.f[0].x + 16 * 256;
    core.f[1].hp = characters[4].melee_damage * 2;
    tick(core, practice, {A, A});
    for(int frame = 0; frame < 100 && !core.can_followup(0, A); ++frame) tick(core, practice);
    CHECK(core.can_followup(0, A));
    tick(core, practice, {A, A});
    for(int frame = 0; frame < 50; ++frame) tick(core, practice);
    CHECK(core.outcome == 1 && practice.feedback == FOLLOWUP_USE_B && !practice.completed());

    begin(core, practice, 0, DUMMY_EVADE);
    core.f[0].x = 100 * 256;
    core.f[1].x = 120 * 256;
    while(core.tick < 50) tick(core, practice);
    tick(core, practice, {A, A});
    for(int frame = 0; frame < 130; ++frame) tick(core, practice);
    CHECK(practice.feedback == EVADE_INTERRUPTED && !practice.super_released && !practice.completed());

    core.reset(0, 1);
    uint32_t before = core.checksum();
    practice.prepare(core);
    CHECK(core.checksum() == before);
    for(const auto* text : practice_feedback_text) CHECK(std::strlen(text) <= 23);
    for(const auto* text : practice_mode_names) CHECK(std::strlen(text) <= 11);
    RoundPresentation presentation;
    core.outcome = 1;
    presentation.begin(core, true);
    CHECK(presentation.duration() == practice_retry_frames);
    presentation.begin(core, false);
    CHECK(presentation.duration() == RoundPresentation::recall_frames);
    std::printf("PASS %d practice checks; all nine fighters, real B follow-ups, jump/squeeze evasion, rejection, interruption, training exclusion and retry timing.\n", checks);
}
