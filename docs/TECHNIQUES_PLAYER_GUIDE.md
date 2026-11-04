# Fighter Techniques — v1.5.0

Four Pokémon have signature techniques using their existing controls.

| Technique | GBA input | Local Versus P1 | Local Versus P2 |
| --- | --- | --- | --- |
| Pikachu: Volt Step | Double tap Left or Right | Double tap an arrow | Double tap A or D |
| Squirtle: Shell Roll | Down plus Left/Right | Down plus an arrow | S plus A/D |
| Jigglypuff: Air Brake | Up again while falling | Up | W |
| Gastly: Shadow Burst | B to cast, B again to arm | K, then K | G, then G |

## Pikachu — Volt Step

Release and tap the same horizontal direction twice within **10 simulation ticks**.
On the ground, while free to move, the second tap starts a **12-stamina** dash.
There are **2 startup ticks**, **6 movement ticks** at four pixels per tick and
**8 recovery ticks**, followed by a **24-tick cooldown**. The direction commits
at activation. Walls and the rival's body stop movement; there is no teleport.

An electric cue, lean and VOLT STEP label announce it. Holding a direction does
not repeat the move. The dash adds no attack damage or invulnerability and
cannot cancel an attack, held guard, jump or stun. Opponents can intercept it
with melee or a projectile, or punish the recovery. A hit interrupts movement.

## Squirtle — Shell Roll

Press **Down while holding Left or Right** to add a roll to the usual squeeze.
It costs **20 stamina total**: the original twelve plus eight for movement.
After the existing three-tick startup, Squirtle moves two pixels per tick for
the first **10 active ticks**, up to twenty pixels. Direction commits at entry.
The original **24 low ticks, 9 recovery ticks and 24-tick cooldown** are retained.
**Down alone** still gives the original stationary twelve-stamina dodge.

The spinning body, water cue and SHELL ROLL label identify the move. There is no
invulnerability. A properly timed low roll can pass beneath a super; melee can
hit and interrupt it. Catch its startup/recovery or use Gastly's wide burst.
Walls and body collision still apply. Repeated Down cannot extend the dodge.

## Jigglypuff — Air Brake

Jump, release Up, then tap Up again during descent. Air Brake costs **8 stamina**
and slows falling for **12 simulation ticks**. It works once per jump and adds
no height or invulnerability. A musical-note cue and AIR BRAKE label show it
working. Landing restores its use. Attacks and stun prevent activation; a hit
ends the brake. An Up tap just before landing keeps the usual buffered jump.

Use the change in timing to make an opponent commit early. Against it, wait for
the landing or catch Jigglypuff with an aerial hit.

## Gastly — Shadow Burst

Cast an ordinary Shadow Orb with B. Tap B again while it travels to fix it in
place, paying **8 extra stamina**. An outlined ring warns for **12 ticks**, then
the area flashes for **4 active ticks**. A clean burst deals **10 damage**; the
ordinary traveling orb deals 12. One orb hits once. Holding B cannot arm it.

Time the burst to catch an approach, a landing or a squeeze beneath the normal
orb. The opponent can **block, jump, leave the ring or shoot it**. Blocking works
from either side. The warning itself is harmless. Arming does not shorten firing
recovery and cannot cancel another attack, guard or stun.

## Learn in Practice

Choose Pikachu, Squirtle, Jigglypuff or Gastly in Practice. Open options with **Select**, then set
**Training → Technique**. The option appears for these four fighters.

Pikachu's lesson requires a completed dash with room to move. Squirtle's requires
an actual rolling evasion of the rival's super; an ordinary stationary squeeze
does not complete it. Select retries with restored positions and resources.

Jigglypuff's lesson completes after a braked descent and landing. Gastly's lesson
requires a real burst hit: press the second B before the normal orb reaches the
rival, with the rival inside the ring. Select retries quickly with resources and
positions restored. Practice awards no solo mastery.

With the Local Versus keyboard adapter loaded, Player 1 uses **O for Select**
to open options or retry Practice. Solo play also works with ordinary emulator
GBA bindings. Local Versus uses the mGBA development adapter described in
`LOCAL_VERSUS.md`.

Human timing, readability, physical keyboard behavior and matchup balance
remain playtest questions. The release includes automated native validation;
it does not establish human fairness.
