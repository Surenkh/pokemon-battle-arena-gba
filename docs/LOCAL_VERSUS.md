# Local Versus preview — v1.3.0-rc.1

Two people choose Pokémon and fight in one running GBA game, on one screen.
This preview uses keyboard controls through an mGBA Lua input adapter.

## Start playing

1. Extract the complete preview player ZIP into a folder.
2. Use an **mGBA 0.11 development build with Lua raw keyboard input**. Official
   development downloads are at https://mgba.io/downloads.html. The adapter was
   validated against upstream commit `a67176cd3e6bf60a5016d7adc94357142dd12617`.
3. Open `PokemonBattleArena.gba` in mGBA.
4. Open **Tools → Scripting**, then load `local_versus.lua` from the extracted ZIP.
5. Return focus to the game window. Player 1 presses **J** to open the mode menu,
   then chooses **Local Versus**. Its chooser should show **CONNECTED**.
6. Each player moves their own cursor and presses their own Ready button.
   Both confirmations start the fight. First to two round wins takes the set.

The raw `input` API is a development-build feature. Stable mGBA 0.10.x reports
an explicit compatibility error when this adapter is loaded. Solo modes can be
played with the emulator's ordinary GBA bindings when the adapter is absent.

## Keyboard controls

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Move / browse fighters | Arrow keys | W A S D |
| Jump | Up | W |
| Squeeze | Down | S |
| Melee / Ready / confirm | J | F |
| Fire / unready / Back | K | G |
| Hold block | U | Q |
| Super at full SP | I | E |
| Breakaway at 60 SP during ordinary hitstun | Hold U + tap I | Hold Q + tap E |
| Pause / resume | Enter | T |
| Select / options / Practice retry | O | — |

On the chooser, Player 1 uses **U / I** to change location before either player
readies. A **LOCK** label appears after one confirmation. Back unlocks that
player's choice. Player 1 Back exits to the mode menu when both are unready.
All thirteen Pokémon, mirror matches and all five locations are available.
Blue **1** and gold **2** markers identify the fighters during a mirror match.

The adapter supplies Player 1's controls throughout the menus and solo modes
while loaded. Player 2 controls belong to Local Versus. Results accept either
player's confirmation. Next Round keeps the score, Rematch Set resets it, and
Change Pokémon returns to the shared chooser.

## Pause, reconnect and keyboard focus

Either player can pause. Attacks pressed in Pause or the countdown are discarded.
If the adapter stops updating for three game frames, the match pauses. Reconnect,
release both control sets, then confirm Resume. The paused fight stays in place.
Loading the script again replaces its callbacks.

When an adapter is missing, the chooser says **LOAD ADAPTER**. Load the script
and check the Scripting error log. The emulator's ordinary B binding can leave
the chooser while disconnected. Keep keyboard focus on the game during play.
Some keyboards limit simultaneous key combinations. Real keyboard rollover,
operating-system shortcuts, the Qt GUI and physical gamepads need user testing.
This preview implements the two keyboard layouts above.

## Implementation and validation

The ROM publishes a versioned descriptor containing the address of a dedicated
16-byte input mailbox. The script discovers it in ROM, checks its protocol and
RAM bounds, and writes held buttons with a sequence guard. The game derives
press edges and rejects incomplete/stale packets. Inputs are sampled through
mGBA's `keysRead` callback. One `CombatCore` receives both input streams.

Local Versus does not award solo records or mastery. Its set scores remain in
memory. Mirror-match body palettes are independent so hit flashes and recall
tint belong to the correct fighter.

The native runner uses one upstream mGBA development core, its real InputContext
and Lua engine, and the shipped script. Host key events follow the same API used
by the Qt frontend. The script writes only the declared input mailbox. Actual
hits, KOs and set wins come from gameplay inputs. Native captures show the real
framebuffer. Physical keyboard/Qt GUI sessions and controller routing remain
open preview checks.

The source repository includes validation in `docs/LOCAL_VERSUS_TEST_REPORT.md`
and native test setup in `docs/NATIVE_TESTING.md`.
