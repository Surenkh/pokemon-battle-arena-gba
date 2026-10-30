# v0.5.0 — Fighter Identity and Combat Depth

Build on the merged v0.4.0 release in `feat/fighter-depth`. The update adds six practical fighter traits, bounded hit-confirm follow-ups, clearer recovery counterplay, a measured stamina adjustment and an accessible practice mode. Keep the existing GBA controls, one arena, two combat audio voices and fixed collision/effect pools.

## Fighter contracts

| Fighter | Signature behavior | Cost or limitation |
| --- | --- | --- |
| Pikachu | Quick Step: a small forward movement during grounded melee startup | Commits to the attack; normal damage, guard and recovery rules apply |
| Charmander | Ember Pressure: ordinary fire consumes more guard stamina | Its existing slower attack and recovery remain punishable |
| Squirtle | Shell Guard and water control: more efficient guard impacts and stronger ordinary water pushback | Slow movement; supers still bypass guard |
| Bulbasaur | Vine Reach: longer melee reach with a visible vine cue | Existing slower startup and recovery remain |
| Eevee | Adaptable: can follow a confirmed melee with A or B | One follow-up only; normal costs and a defensive gap |
| Meowth | Punish: bonus melee damage against an opponent in attack recovery | Requires a real recovery hit; no bonus against neutral, startup, guard or stun |

All numerical properties live in config/balance.json and generate immutable C++ data. Both fighters obey the same collision, resource and follow-up rules. No type multipliers, random critical hits or damage-over-time effects are added.

## Follow-up contract

After an ordinary melee clean hit, press B during the end of its recovery to cancel into the fighter's ordinary projectile. Eevee may also use A. Blocks, misses and invulnerability rejection never grant the cancel. A follow-up cannot grant another follow-up. Attacks retain their startup and costs; ordinary protection remains active. Require at least two simulation ticks beyond the original hitstun duration before the follow-up's earliest possible contact, so the defender has an opportunity to block, jump or squeeze. The existing three-tick press queue still applies. A held button does not repeat.

The HUD shows the available follow-up and confirmed recovery punish. Practice feedback counts damaging follow-up contacts, rather than treating every accepted input as a successful combination.

## Stamina and evidence

Before editing the core, tools/balance_probe.cpp records 2,160 CPU-versus-CPU rounds: 20 fixed seeds for every ordered matchup and all three difficulty levels. Preserve the baseline report. Collect duration, timeouts, resource availability, per-fighter results, attacks and super starts/releases again after changes.

The initial baseline spends 76–80% of active ticks below the ordinary projectile's 20-stamina cost. Test reducing the regeneration delay from 30 to 24 ticks and a bounded CPU recovery policy based only on its own resources. Preserve HP, base damage, action costs, jump geometry, super costs and difficulty observation delays. Report automated results as automated results; they do not establish human matchup fairness.

## Practice mode

B on the title opens practice setup. Select the player from the same six cards, then choose an idle, blocking or attacking dummy and its character. R opens the selected fighter's guide in either selector. Avoid mirror opponents because the existing renderer shares character palettes.

Practice uses the same CombatCore rules. Disable the round timeout, start with full SP for super drills, preserve ordinary stamina costs and allow Select to reset the arena. KO automatically starts another practice round. The HUD shows last damage and the confirmed one/two-hit sequence. Pause freezes combat and has resume, reset, change and controls actions. Practice statistics are separate from authoritative gameplay checksums.

## Delivery gates

- Host tests cover every trait on both sides, exact guard costs, punish boundaries, mirrored reach geometry, blocked/missed/traded confirms, bounded chains, defensive gaps, practice input policies and deterministic replay.
- Native mGBA tests use real buttons and read-only telemetry. Exercise the new setup/guide/pause/reset flows, all dummy modes, actual trait/follow-up presentation, full native rounds and repeated transitions.
- Preserve existing animation, elemental, sound and combat regressions. Compare native CPU budget with v0.4.0 and require zero missed display frames in the tested runs.
- Clean-build from tracked source, package the ROM with checksum, preserve the previous release and open a GitHub PR with measured results and limitations.
