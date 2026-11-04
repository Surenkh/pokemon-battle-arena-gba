# Fighter Techniques — v1.3.0-rc.1 preview

Two Pokémon now have an extra timing choice using their existing controls.

| Technique | GBA input | Local Versus P1 | Local Versus P2 |
| --- | --- | --- | --- |
| Jigglypuff: Air Brake | Up again while falling | Up | W |
| Gastly: Shadow Burst | B to cast, B again to arm | K, then K | G, then G |

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

Choose Jigglypuff or Gastly in Practice. Open options with **Select**, then set
**Training → Technique**. The option appears for these two fighters.

Jigglypuff's lesson completes after a braked descent and landing. Gastly's lesson
requires a real burst hit: press the second B before the normal orb reaches the
rival, with the rival inside the ring. Select retries quickly with resources and
positions restored. Practice awards no solo mastery.

With the Local Versus keyboard adapter loaded, Player 1 uses **O for Select**
to open options or retry Practice. Solo play also works with ordinary emulator
GBA bindings. Local Versus uses the mGBA development adapter described in
`LOCAL_VERSUS.md`.

This is a two-fighter preview. Human timing, readability and matchup feedback
are still needed before expanding the techniques to more Pokémon.
