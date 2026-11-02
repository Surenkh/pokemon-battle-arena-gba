#pragma once
#include "pkmn_combat_core.h"
#include "pkmn_super_evasion.h"

namespace pkmn {
enum DummyMode { DUMMY_IDLE, DUMMY_BLOCK, DUMMY_ATTACK, DUMMY_FOLLOWUP, DUMMY_EVADE, DUMMY_MODE_COUNT };
enum DrillFeedback {
    FOLLOWUP_PROMPT, FOLLOWUP_WAIT, FOLLOWUP_NEEDS_MELEE, FOLLOWUP_STARTED,
    FOLLOWUP_COMPLETE, EVADE_PROMPT, SUPER_WARNING, SUPER_FLYING,
    EVADE_COMPLETE, EVADE_HIT, EVADE_INTERRUPTED, EVADE_MISSED,
    DRILL_NO_STAMINA, DRILL_BLOCKED, FOLLOWUP_USE_B
};
inline constexpr int practice_retry_frames = 6;
inline constexpr const char* practice_mode_names[] = {"FREE", "BLOCK", "ATTACK", "FOLLOW-UP", "EVADE SUPER"};
inline constexpr const char* practice_feedback_text[] = {
    "LAND A THEN FOLLOW B", "WAIT FOR FOLLOW B", "START WITH MELEE A", "FOLLOW-UP STARTED",
    "FOLLOW-UP LANDED!", "EVADE THE RIVAL SUPER", "WATCH THE SUPER CHARGE", "JUMP OR TIME DOWN",
    "SUPER EVADED!", "SUPER HIT - TRY AGAIN", "LET THE SUPER RELEASE", "TRY JUMP OR DOWN",
    "NEED STAMINA", "BLOCKED - TRY AGAIN", "USE B FOR THE FOLLOW-UP"
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
    bool finished = false, followup_shot_seen = false;
    static int bounded(int value) { return value > 9999 ? 9999 : value; }

public:
    DummyMode mode = DUMMY_IDLE;
    int opponent = 1;
    PracticeStats stats;
    DrillFeedback feedback = FOLLOWUP_PROMPT;
    bool super_started = false, super_released = false;

    bool drill() const { return mode == DUMMY_FOLLOWUP || mode == DUMMY_EVADE; }
    bool completed() const { return feedback == FOLLOWUP_COMPLETE || feedback == EVADE_COMPLETE; }
    const char* message() const { return practice_feedback_text[feedback]; }

    void begin_round(bool keep_stats = false) {
        last_event_frame = ~0u;
        evasion.reset();
        finished = super_started = super_released = false;
        followup_shot_id = 0;
        followup_shot_seen = false;
        feedback = mode == DUMMY_EVADE ? EVADE_PROMPT : FOLLOWUP_PROMPT;
        if(!keep_stats) stats = PracticeStats{};
    }

    void prepare(CombatCore& core) const {
        if(!core.training) return;
        core.f[0].sp = core.f[1].sp = 100;
        if(mode == DUMMY_FOLLOWUP) {
            core.f[0].x = 96 * 256;
            core.f[1].x = 116 * 256;
        }
    }

    Input input(const CombatCore& core) const {
        if(core.hitstop || core.outcome) return {};
        const auto& rival = core.f[1];
        if(mode == DUMMY_EVADE) {
            if(!super_started && !finished && core.tick >= 45 && rival.action == NONE &&
               core.grounded(rival) && rival.sp == 100 && rival.sta >= super_stamina_cost * 60)
                return {R, R};
            return {};
        }
        if(mode == DUMMY_IDLE || mode == DUMMY_FOLLOWUP) return {};
        if(mode == DUMMY_BLOCK) return {L, 0};
        int delta = core.f[0].x - rival.x;
        int distance = (delta < 0 ? -delta : delta) / 256;
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
