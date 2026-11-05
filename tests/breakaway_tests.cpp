#include "pkmn_animation.h"
#include "pkmn_combat_feel.h"
#include "pkmn_practice.h"
#include <cstdlib>
#include <iostream>
#include <initializer_list>

using namespace pkmn;

static int checks = 0;
static void check(bool condition, const char* what) {
    ++checks;
    if(!condition) {
        std::cerr << "FAIL " << what << '\n';
        std::exit(1);
    }
}

static constexpr Input escape{L | R, R};
static constexpr Input hold_escape{L | R, 0};

static void press(CombatCore& core, int who, Input keys) {
    core.frame(who == 0 ? keys : Input{}, who == 1 ? keys : Input{});
}

static bool event(const CombatCore& core, EventKind kind, int owner) {
    for(int i = 0; i < core.event_count; ++i)
        if(core.events[i].kind == kind && core.events[i].owner == owner) return true;
    return false;
}

static void ordinary_hit(CombatCore& core, int defender) {
    press(core, 1 - defender, {A, A});
    for(int i = 0; i < 20 && core.f[defender].action != HITSTUN; ++i) core.frame({}, {});
    check(core.f[defender].action == HITSTUN && core.hitstop > 0 &&
          event(core, EV_HIT, 1 - defender), "real normal melee hits the defender");
}

static void start_escape(CombatCore& core, int who) {
    const int tick = core.tick;
    press(core, who, escape);
    check(core.f[who].pending == REQUEST_BREAKAWAY && core.tick == tick, "fresh chord queues during hitstop");
    // The higher-priority escape survives a different buffered action.
    while(core.hitstop) press(core, who, {B, B});
    press(core, who, hold_escape);
    check(core.f[who].action == BREAKAWAY && event(core, EV_BREAKAWAY, who), "first live tick starts escape");
}

int main() {
    for(int fighter = 0; fighter < character_count; ++fighter) for(int who = 0; who < 2; ++who) {
        CombatCore core;
        core.reset(who == 0 ? fighter : 0, who == 1 ? fighter : 0);
        core.f[0].x = 100 * 256;
        core.f[1].x = 120 * 256;
        auto& v = core.f[who];
        v.sp = (fighter % 2) ? 100 : breakaway_power_cost;
        ordinary_hit(core, who);
        check(core.can_breakaway(who), "every fighter can escape ordinary hitstun on either side");
        const int x = v.x, hp = v.hp, sta = v.sta, sp = v.sp, protection = v.invuln_until;
        start_escape(core, who);
        check(v.sp == sp - breakaway_power_cost && v.hp == hp && v.sta == sta, "only the SP cost is paid");
        check(v.invuln_until == protection, "escape grants no extra protection");
        check(v.pending == 0 && v.vy < 0 && fighter_pose(core, who).clip == CLIP_HOP, "starts a backhop");
        CombatFeel feel;
        feel.update(core, true);
        check(feel.status(core, who) == 15 && feel.flash[who] > 0, "escape event drives the flash and label");
        for(int i = 1; i < breakaway_travel_ticks; ++i) press(core, who, hold_escape);
        const int travel = breakaway_travel_ticks * breakaway_speed_q8;
        check(v.x == x + (who ? travel : -travel), "exact mirrored travel");
        check(core.recovering(who), "attacker can punish recovery");
        while(core.tick < v.ends) press(core, who, hold_escape);
        core.frame({}, {});
        check(v.action == NONE && v.sp == sp - breakaway_power_cost, "holding the chord never repeats the escape");
    }

    CombatCore core;
    core.reset(0, 0);
    core.f[0].sp = 100;
    press(core, 0, escape);
    check(core.f[0].action == BLOCK && core.f[0].sp == 100, "mistimed chord cannot accidentally launch a super");
    core.frame({}, {});
    press(core, 0, {R, R});
    check(core.f[0].action == SUPER && core.f[0].sp == 0, "R alone still spends the full bar on a super");

    core.reset(0, 0);
    core.f[0].sp = 100;
    core.f[1].sp = 100;
    press(core, 1, {R, R});
    for(int i = 0; i < 100 && core.f[0].action != HITSTUN; ++i) core.frame({}, {});
    check(core.f[0].action == HITSTUN && core.f[0].ends - core.f[0].began == reactions_super_hitstun_ticks,
          "real super lands");
    press(core, 0, escape);
    check(core.f[0].pending == 0 && core.f[0].sp == 100, "super hitstun rejects escape");

    for(Action reaction : {BLOCKSTUN, GUARD_BREAK}) {
        core.reset(0, 0);
        core.f[0].sp = 100;
        core.f[0].action = reaction;
        core.f[0].ends = reactions_normal_hitstun_ticks;
        press(core, 0, escape);
        check(core.f[0].action == reaction && core.f[0].sp == 100, "guard reactions reject escape");
    }

    core.reset(0, 0);
    core.f[0].x = world_min_fighter_x * 256;
    core.f[1].x = core.f[0].x + 20 * 256;
    core.f[0].sp = breakaway_power_cost - 1;
    ordinary_hit(core, 0);
    press(core, 0, escape);
    check(!core.f[0].pending && core.f[0].sp == 59, "59 SP cannot queue escape");
    core.f[0].sp = breakaway_power_cost;
    while(core.hitstop) press(core, 0, hold_escape);
    press(core, 0, hold_escape);
    check(core.f[0].action == HITSTUN && core.f[0].sp == 60, "held shoulder keys cannot trigger later");
    press(core, 0, escape);
    check(core.f[0].action == BREAKAWAY && core.f[0].x == world_min_fighter_x * 256 &&
          core.f[0].y < world_floor_y * 256, "wall stops horizontal travel and still allows the hop");
    core.reset(0, 0);
    check(core.f[0].action == NONE && !core.f[0].pending && core.f[0].sp == 0, "reset clears escape state");

    // Practice must work even on a fighter without a signature technique.
    PracticeSession lesson;
    lesson.cycle_mode(1, -1);
    check(lesson.mode == DUMMY_BREAKAWAY, "Breakaway is reachable on any fighter");
    lesson.cycle_mode(1, -1);
    check(lesson.mode == DUMMY_EVADE, "a fighter without a technique skips only that lesson");
    lesson.cycle_mode(1, 1);
    lesson.begin_round();
    core.training = true;
    lesson.prepare(core);
    for(int i = 0; i < 80 && !core.can_breakaway(0); ++i) {
        core.frame({}, lesson.input(core));
        lesson.observe(core);
    }
    check(lesson.feedback == BREAKAWAY_NOW && core.can_breakaway(0), "a resolved hit displays the lesson cue");
    core.frame(escape, lesson.input(core));
    lesson.observe(core);
    for(int i = 0; i < 10 && !lesson.completed(); ++i) {
        core.frame({}, lesson.input(core));
        lesson.observe(core);
    }
    check(lesson.completed() && core.f[0].sp == 40, "resolved escape completes the lesson");
    lesson.begin_round();
    check(!lesson.completed() && lesson.feedback == BREAKAWAY_PROMPT, "lesson retry clears completion");
    std::cout << "PASS " << checks << " Breakaway checks across all fighters and both sides\n";
}
