#pragma once
#include "pkmn_combat_core.h"
#include "pkmn_super_evasion.h"

namespace pkmn {
enum DummyMode { DUMMY_IDLE, DUMMY_BLOCK, DUMMY_ATTACK, DUMMY_FOLLOWUP, DUMMY_EVADE, DUMMY_TECHNIQUE, DUMMY_BREAKAWAY, DUMMY_MODE_COUNT };
enum DrillFeedback {
    FOLLOWUP_PROMPT, FOLLOWUP_WAIT, FOLLOWUP_NEEDS_MELEE, FOLLOWUP_STARTED,
    FOLLOWUP_COMPLETE, EVADE_PROMPT, SUPER_WARNING, SUPER_FLYING,
    EVADE_COMPLETE, EVADE_HIT, EVADE_INTERRUPTED, EVADE_MISSED,
    DRILL_NO_STAMINA, DRILL_BLOCKED, FOLLOWUP_USE_B,
    BRAKE_PROMPT, BRAKE_ACTIVE, BRAKE_COMPLETE, ORB_PROMPT, ORB_WARNING,
    ORB_COMPLETE, ORB_MISSED, ORB_NORMAL_HIT, TECHNIQUE_INTERRUPTED,
    STEP_PROMPT, STEP_ACTIVE, STEP_COMPLETE, STEP_BLOCKED,
    ROLL_PROMPT, ROLL_ACTIVE, ROLL_COMPLETE,
    BREAKAWAY_PROMPT, BREAKAWAY_NOW, BREAKAWAY_COMPLETE, BREAKAWAY_MISSED
};
inline constexpr int practice_retry_frames = 6;
inline constexpr const char* practice_mode_names[] = {"FREE", "BLOCK", "ATTACK", "FOLLOW-UP", "EVADE SUPER", "TECHNIQUE", "BREAKAWAY"};
inline constexpr const char* practice_feedback_text[] = {
    "LAND A THEN FOLLOW B", "WAIT FOR FOLLOW B", "START WITH MELEE A", "FOLLOW-UP STARTED",
    "FOLLOW-UP LANDED!", "EVADE THE RIVAL SUPER", "WATCH THE SUPER CHARGE", "JUMP OR TIME DOWN",
    "SUPER EVADED!", "SUPER HIT - TRY AGAIN", "LET THE SUPER RELEASE", "TRY JUMP OR DOWN",
    "NEED STAMINA", "BLOCKED - TRY AGAIN", "USE B FOR THE FOLLOW-UP",
    "JUMP. UP WHILE FALLING", "AIR BRAKE! LAND SAFELY", "AIR BRAKE LANDED!",
    "B ORB. B NEAR RIVAL", "ORB ARMED. WATCH BURST", "SHADOW BURST HIT!",
    "TOO FAR. SELECT RETRY", "NORMAL HIT. B EARLIER", "HIT - SELECT TO RETRY",
    "DOUBLE TAP LEFT/RIGHT", "VOLT STEP! FINISH MOVE", "VOLT STEP COMPLETE!", "WALL. SELECT TO RETRY",
    "DOWN + LEFT/RIGHT", "ROLL UNDER THE SUPER", "SHELL ROLL EVADED!",
    "LET THE RIVAL HIT", "HOLD L. TAP R NOW", "BREAKAWAY! 60 SP USED", "MISSED. SELECT RETRY"
};
struct PracticeStats {
    int last_damage = 0, chain = 0, best_chain = 0, hits = 0, followups = 0, punishes = 0;
};

// Produces ordinary buttons and observes resolved events. Only prepare() sets
// initial training resources/positions, before the first simulation frame.
class PracticeSession {
    uint32_t last_event_frame = ~0u;
    uint32_t followup_shot_id = 0;
    SuperEvasion evasion;
    bool finished = false, followup_shot_seen = false, technique_started = false;
    int technique_x = 0;
    static int bounded(int value) { return value > 9999 ? 9999 : value; }

public:
    DummyMode mode = DUMMY_IDLE;
    int opponent = 1;
    PracticeStats stats;
    DrillFeedback feedback = FOLLOWUP_PROMPT;
    bool super_started = false, super_released = false;

    static bool has_technique(int fighter) { return characters[fighter].technique != TECHNIQUE_NONE; }
    static int available_modes(int fighter) { return has_technique(fighter) ? DUMMY_MODE_COUNT : DUMMY_MODE_COUNT - 1; }
    void cycle_mode(int fighter, int direction) {
        do {
            mode = DummyMode((int(mode) + DUMMY_MODE_COUNT + direction) % DUMMY_MODE_COUNT);
        } while(mode == DUMMY_TECHNIQUE && available_modes(fighter) < DUMMY_MODE_COUNT);
    }
    void validate_mode(int fighter) { if(mode == DUMMY_TECHNIQUE && !has_technique(fighter))mode=DUMMY_IDLE; }
    bool drill() const { return mode == DUMMY_FOLLOWUP || mode == DUMMY_EVADE || mode == DUMMY_TECHNIQUE || mode == DUMMY_BREAKAWAY; }
    bool completed() const { return feedback == FOLLOWUP_COMPLETE || feedback == EVADE_COMPLETE || feedback == BRAKE_COMPLETE || feedback == ORB_COMPLETE || feedback == STEP_COMPLETE || feedback == ROLL_COMPLETE || feedback == BREAKAWAY_COMPLETE; }
    const char* message() const { return practice_feedback_text[feedback]; }

    void begin_round(bool keep_stats = false) {
        last_event_frame = ~0u;
        evasion.reset();
        finished = super_started = super_released = false;
        followup_shot_id = 0;
        followup_shot_seen = false;
        technique_started = false;
        technique_x = 0;
        feedback = mode == DUMMY_BREAKAWAY ? BREAKAWAY_PROMPT : mode == DUMMY_EVADE ? EVADE_PROMPT : FOLLOWUP_PROMPT;
        if(!keep_stats) stats = PracticeStats{};
    }

    void prepare(CombatCore& core) {
        if(!core.training) return;
        validate_mode(core.f[0].character);
        core.f[0].sp = core.f[1].sp = 100;
        if(mode == DUMMY_TECHNIQUE){
            const int technique=characters[core.f[0].character].technique;
            feedback=technique==TECHNIQUE_AIR_BRAKE?BRAKE_PROMPT:technique==TECHNIQUE_VOLT_STEP?STEP_PROMPT:technique==TECHNIQUE_SHELL_ROLL?ROLL_PROMPT:ORB_PROMPT;
        }
        if(mode == DUMMY_FOLLOWUP || mode == DUMMY_BREAKAWAY) {
            core.f[0].x = 96 * 256;
            core.f[1].x = 116 * 256;
        }
    }

    Input input(const CombatCore& core) const {
        if(core.hitstop || core.outcome) return {};
        const auto& rival = core.f[1];
        if(mode == DUMMY_EVADE || (mode == DUMMY_TECHNIQUE && characters[core.f[0].character].technique == TECHNIQUE_SHELL_ROLL)) {
            if(!super_started && !finished && core.tick >= 45 && rival.action == NONE &&
               core.grounded(rival) && rival.sp == 100 && rival.sta >= super_stamina_cost * 60)
                return {R, R};
            return {};
        }
        if(mode == DUMMY_IDLE || mode == DUMMY_FOLLOWUP || mode == DUMMY_TECHNIQUE) return {};
        if(mode == DUMMY_BLOCK) return {L, 0};
        int delta = core.f[0].x - rival.x;
        int distance = (delta < 0 ? -delta : delta) / 256;
        if(mode == DUMMY_BREAKAWAY) {
            if(finished || feedback != BREAKAWAY_PROMPT || core.tick < 45 || rival.action != NONE) return {};
            if(distance < characters[rival.character].melee_reach + 3) return {A, A};
            return {uint16_t(delta > 0 ? RIGHT : LEFT), 0};
        }
        if(core.tick % practice_dummy_think_ticks == 0) {
            if(core.can_followup(1, A) && distance < characters[rival.character].melee_reach + 6) return {A, A};
            if(core.can_followup(1, B) && distance < 140) return {B, B};
        }
        if(rival.action != NONE && rival.action != BLOCK) return {};
        if(core.tick % practice_dummy_think_ticks == 0) {
            if(rival.sp == 100 && rival.sta >= super_stamina_cost * 60 && distance > 38 &&
               core.grounded(rival) && core.tick % practice_dummy_super_interval_ticks == 0) return {R, R};
            if(distance < characters[rival.character].melee_reach + 6 && rival.sta >= melee_stamina_cost * 60) return {A, A};
            if(distance > 48 && rival.sta >= fire_stamina_cost * 60 &&
               core.tick % practice_dummy_shot_interval_ticks == 0) return {B, B};
        }
        if(distance > characters[rival.character].melee_reach + 3) return {uint16_t(delta > 0 ? RIGHT : LEFT), 0};
        return {};
    }

    void observe(const CombatCore& core) {
        if(last_event_frame == core.event_frame) return;
        last_event_frame = core.event_frame;
        for(int i = 0; i < core.event_count; ++i) {
            const auto& event = core.events[i];
            if(event.owner == 0) {
                if(event.kind == EV_PUNISH) stats.punishes = bounded(stats.punishes + 1);
                if(event.kind == EV_HIT) {
                    stats.last_damage = event.damage;
                    stats.hits = bounded(stats.hits + 1);
                    stats.chain = event.followup ? 2 : 1;
                    if(stats.chain > stats.best_chain) stats.best_chain = stats.chain;
                    if(event.followup) stats.followups = bounded(stats.followups + 1);
                }
            }
            if(!drill() || finished) continue;
            if(mode == DUMMY_BREAKAWAY) {
                if(event.owner == 0 && event.kind == EV_BREAKAWAY) {
                    feedback = BREAKAWAY_COMPLETE;
                    finished = true;
                } else if(event.target == 0 && event.kind == EV_HIT && !event.super) {
                    feedback = BREAKAWAY_NOW;
                }
                continue;
            }
            if(mode == DUMMY_TECHNIQUE) {
                if(event.target == 0 && event.kind == EV_HIT) { feedback=TECHNIQUE_INTERRUPTED;finished=true; }
                if(event.owner == 0) {
                    if(event.kind == EV_AIR_BRAKE) { technique_started=true;feedback=BRAKE_ACTIVE; }
                    if(event.kind == EV_ORB_ARM) { technique_started=true;feedback=ORB_WARNING; }
                    if(event.kind == EV_BURST_HIT) { feedback=ORB_COMPLETE;finished=true; }
                    if(event.kind == EV_VOLT_STEP) { technique_started=true;technique_x=core.f[0].x;feedback=STEP_ACTIVE; }
                    if(event.kind == EV_SHELL_ROLL) { technique_started=true;feedback=ROLL_ACTIVE; }
                    if(event.kind == EV_HIT && !technique_started && characters[core.f[0].character].technique == TECHNIQUE_SHADOW_BURST)feedback=ORB_NORMAL_HIT;
                }
                if(characters[core.f[0].character].technique == TECHNIQUE_SHELL_ROLL && event.owner == 1){
                    if(event.kind == EV_CHARGE)super_started=true;
                    if(event.kind == EV_RELEASE)super_released=true;
                }
            }
            if(event.owner == 0 && event.kind == EV_TIRED) feedback = DRILL_NO_STAMINA;
            if(mode == DUMMY_FOLLOWUP && event.owner == 0) {
                if(event.kind == EV_SHOT) {
                    followup_shot_id = event.attack;
                    followup_shot_seen = event.followup;
                    if(!event.followup) feedback = FOLLOWUP_NEEDS_MELEE;
                }
                if(event.kind == EV_HIT && event.damage > 0) {
                    // KO cleanup can clear the projectile in this same frame.
                    bool linked_shot = event.followup && followup_shot_seen && event.attack == followup_shot_id;
                    if(linked_shot) {
                        feedback = FOLLOWUP_COMPLETE;
                        finished = true;
                    } else if(event.followup) feedback = FOLLOWUP_USE_B;
                    else if(core.f[0].action == MELEE && core.f[0].confirmed_at >= 0) feedback = FOLLOWUP_WAIT;
                }
                if(event.kind == EV_FOLLOWUP) feedback = FOLLOWUP_STARTED;
                if(event.kind == EV_BLOCK) feedback = DRILL_BLOCKED;
            }
            if(mode == DUMMY_EVADE) {
                if(event.owner == 1 && event.kind == EV_CHARGE) {
                    super_started = true;
                    feedback = SUPER_WARNING;
                }
                if(event.owner == 1 && event.kind == EV_RELEASE) {
                    super_released = true;
                    feedback = SUPER_FLYING;
                }
                if(event.kind == EV_HIT && event.target == 0 && event.super) {
                    feedback = EVADE_HIT;
                    finished = true;
                }
            }
        }
        if(mode == DUMMY_TECHNIQUE && !finished && technique_started) {
            if(characters[core.f[0].character].technique == TECHNIQUE_AIR_BRAKE &&
               core.grounded(core.f[0]) && core.f[0].landed_at == core.tick-1) {
                feedback=BRAKE_COMPLETE;finished=true;
            } else if(characters[core.f[0].character].technique == TECHNIQUE_SHADOW_BURST && !core.p[0].active) {
                feedback=ORB_MISSED;finished=true;
            } else if(characters[core.f[0].character].technique == TECHNIQUE_VOLT_STEP && core.f[0].action == NONE) {
                const int delta=core.f[0].x-technique_x;
                feedback=(delta<0?-delta:delta)>=16*256?STEP_COMPLETE:STEP_BLOCKED;finished=true;
            }
        }
        if(mode == DUMMY_TECHNIQUE && characters[core.f[0].character].technique == TECHNIQUE_SHELL_ROLL && !finished){
            const bool evaded=evasion.observe(core);
            if(evaded && technique_started){feedback=ROLL_COMPLETE;finished=true;}
            else if(super_released && !core.p[3].active){feedback=EVADE_MISSED;finished=true;}
        }
        if(mode == DUMMY_BREAKAWAY && feedback == BREAKAWAY_NOW && !core.can_breakaway(0)) {
            feedback = BREAKAWAY_MISSED;
            finished = true;
        }
        if(mode != DUMMY_EVADE || finished) return;
        if(evasion.observe(core)) {
            feedback = EVADE_COMPLETE;
            finished = true;
        } else if(super_started && !super_released && core.f[1].action != SUPER) {
            feedback = EVADE_INTERRUPTED;
            finished = true;
        } else if(super_released && !core.p[3].active) {
            feedback = EVADE_MISSED;
            finished = true;
        }
    }
};
}
