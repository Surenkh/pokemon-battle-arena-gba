#include "pkmn_local_versus.h"
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

using namespace pkmn;
static int checks = 0;
static void check(bool condition, const char* message) {
    ++checks;
    if (!condition) {
        std::fprintf(stderr, "%s\n", message);
        std::exit(1);
    }
}

int main() {
    VersusMailbox mailbox;
    VersusInput bridge;
    bridge.poll(mailbox);
    check(!bridge.connected(), "An absent adapter stays disconnected");
    mailbox.magic = versus_input_magic;
    mailbox.sequence = 2;
    bridge.poll(mailbox);
    check(bridge.connected(), "A complete neutral packet connects");
    mailbox.sequence = 4;
    mailbox.held[0] = A | LEFT;
    mailbox.held[1] = B | RIGHT;
    bridge.poll(mailbox);
    check(bridge.player(0).pressed == (A | LEFT), "P1 receives its own simultaneous input");
    check(bridge.player(1).pressed == (B | RIGHT), "P2 receives its own simultaneous input");
    bridge.poll(mailbox);
    check(!bridge.player(0).pressed && !bridge.player(1).pressed, "Duplicate packets cannot repeat actions");
    mailbox.sequence = 6;
    bridge.poll(mailbox);
    check(bridge.player(0).held == (A | LEFT) && !bridge.player(0).pressed, "A held key has no extra press edge");
    mailbox.sequence = 7;
    mailbox.held[0] = R;
    bridge.poll(mailbox);
    check(bridge.player(0).held == (A | LEFT), "An incomplete packet cannot leak changed buttons");
    bridge.poll(mailbox);
    bridge.poll(mailbox);
    check(!bridge.connected(), "Three missing frames disconnect the adapter");
    check(!bridge.player(0).held && !bridge.player(1).held, "Disconnect clears both players");
    mailbox.sequence = 8;
    mailbox.held[0] = A;
    mailbox.held[1] = B;
    bridge.poll(mailbox);
    check(!bridge.player(0).pressed && !bridge.player(1).pressed, "Reconnect cannot replay held confirmation keys");
    bridge.poll(mailbox);
    check(!bridge.player(0).held && !bridge.player(1).held, "Unarmed stale packets cannot leak held keys");
    mailbox.sequence = 10;
    mailbox.held[0] = mailbox.held[1] = 0;
    bridge.poll(mailbox);
    mailbox.sequence = 0xfffffffe;
    mailbox.held[1] = A;
    bridge.poll(mailbox);
    check(bridge.player(1).pressed == A, "A fresh press works after both players release");
    mailbox.sequence = 2;
    mailbox.held[1] = R;
    bridge.poll(mailbox);
    check(bridge.player(1).pressed == R, "Sequence wrap preserves fresh input");

    LocalVersus session;
    session.open();
    for (int player = 0; player < 2; ++player) {
        for (int fighter = 0; fighter < character_count; ++fighter) {
            for (int button : {LEFT, RIGHT, UP, DOWN}) {
                session.fighters[player] = fighter;
                const int other = session.fighters[1 - player];
                session.choose(player, Input{uint16_t(button), uint16_t(button)});
                check(session.fighters[player] >= 0 && session.fighters[player] < character_count, "Sparse-grid navigation always chooses a valid fighter");
                check(session.fighters[1 - player] == other, "One player's navigation cannot move the other's cursor");
            }
        }
        session.choose(player, Input{A, A});
        const int chosen = session.fighters[player];
        session.choose(player, Input{RIGHT, RIGHT});
        check(session.ready[player] && session.fighters[player] == chosen, "Ready locks that player's fighter");
        session.choose(player, Input{B, B});
        check(!session.ready[player] && session.fighters[player] == chosen, "Back unlocks without changing fighter");
    }
    session.fighters[0] = session.fighters[1] = 7;
    session.choose(0, Input{A, A});
    session.choose(1, Input{A, A});
    check(session.both_ready(), "Mirror matches can be confirmed by both players");
    session.finish_round(3);
    check(!session.wins[0] && !session.wins[1], "A draw awards no win");
    session.finish_round(2);
    check(session.wins[1] == 1 && !session.complete(), "P2 receives its own round win");
    session.finish_round(2);
    check(session.complete() && session.wins[1] == 2, "First to two completes the set");
    session.arena = arena_count - 1;
    session.reset_set();
    check(!session.complete() && !session.both_ready() && !session.wins[1], "Rematch resets the set and confirmations");
    check(session.fighters[0] == 7 && session.fighters[1] == 7 && session.arena == arena_count - 1, "Rematch retains both fighters and the location");

    int mode = 0;
    unsigned seen = 0;
    for (int count = 0; count < mode_count; ++count) {
        seen |= 1u << mode;
        const int next = adjacent_mode(mode, true);
        check(adjacent_mode(next, false) == mode, "Menu navigation reverses without skipping a mode");
        mode = next;
    }
    check(mode == 0 && seen == 511, "All nine modes are reachable in one cycle");
    std::printf("Local Versus: %d checks passed\n", checks);
}
