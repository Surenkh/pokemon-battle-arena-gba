# Iteration 01: elemental attack identity

Status: **implemented and tested in the v0.3.0 candidate**.

The original design targets below are retained for reference. The restored renderer uses 32×32 normal/impact/guard sheets, 32×16 super sheets and 16×16 muzzle sheets, each with four frames. Five sheets share a palette per fighter. This pass does not add a separate trail system or elemental sound set. See [actual test results](../ELEMENTAL_ATTACKS_TEST_REPORT.md).
Working branch: `feat/elemental-attack-identity`.
Playable baseline: v0.2.0. Candidate next release: v0.3.0, only after a rebuilt ROM passes validation.

## Goal

A player should recognize who fired an attack from its silhouette, animation, and impact, even without seeing the attacker. Electricity should crackle, water should splash, and fire should flare. Color alone is insufficient.

The first pass changes presentation. Keep existing damage, stamina cost, projectile speed, collision, startup/recovery, and super charging rules until a separate balance change is reviewed. Do not quietly introduce stun, burn, slowing, elemental matchups, extra hits, or homing alongside the visual change.

## Six attack identities

| Fighter | Normal attack | Hit effect | Super attack | Silhouette and movement |
| --- | --- | --- | --- | --- |
| Pikachu | Spark Bolt | Branched electric star | Thunder Lance | Angular zigzag core, alternating branches; super is a brighter traveling lance |
| Charmander | Ember Shot | Short flame bloom and embers | Flame Burst | Tapered flame with a trailing tail; expanding flame petals stay decorative |
| Squirtle | Water Pulse | Crown splash and droplets | Hydro Surge | Rounded water core, rippling outline, detached droplets; super is a narrow traveling surge |
| Bulbasaur | Razor Leaf | Split leaf fragments | Verdant Lance | Rotating pair of pointed leaves; super has a braided vine core |
| Eevee | Swift Star | Four-point shimmer | Radiant Swift | Clean star core, alternating glints; super adds a gold ring and star trail |
| Meowth | Claw Wave | Three diagonal scratches | Fury Claw | Three parallel silver arcs with a magenta edge; super is a layered traveling slash |

These are presentation labels, not promises of new Pokémon moves or changes to attack range. Meowth's slash wave preserves the baseline projectile behavior; converting it into melee would be a separate balance task. Candidate colors and frame plans live in [attack_visuals.json](../../design/attack_visuals.json).

## Readable attack sequence

1. **Windup:** attach the elemental cue to the existing firing pose. Use an explicit per-character muzzle anchor. Mirror the anchor with facing; never place it from the full sprite canvas edge.
2. **Travel:** render the projectile core at the simulation projectile position. Its solid center follows the actual hitbox. Small trails and droplets have no collision and should be visibly fainter than that core.
3. **Contact:** consume the simulation's confirmed outcome once. A hit emits the character's impact; a block emits a shared guard ring plus a small elemental burst. A miss expires without a hit flash.
4. **Recovery:** stop emitting when the existing action recovers. The effect must not imply another damaging hit.

Loop the four travel frames using simulation/render timing already present in the game. Play a four-frame impact once and reclaim it. No simulation RNG should be consumed for decorative particles; visual variation must use a separate deterministic counter or local visual seed.

## Supers and fairness

Use the existing super startup as the warning window. Give it a stable charged core, a short expanding outline, and one clear sound cue once audio is available. Do not add full-screen flashing or hide the opponent behind a bloom.

Each super bypasses block exactly as the baseline rules require. The new art must preserve both jump and squeeze evasion wherever the baseline permits them. A decorative wave must not fill the vertical gap the player needs to read. Size the art around the real collision shape; do not enlarge hitboxes to match a prettier sprite.

Hitstop and camera shake belong to the next feedback pass. If later introduced, define which simulation clocks pause and preserve the same warning and evasion opportunities for both sides.

## Integration architecture

Resolve these seams against the restored code before naming new engine classes:

- **Combat simulation:** remains authoritative for projectile position, type, owner, damage, and hit/block/expiry outcomes.
- **Character presentation data:** maps a stable fighter ID to its effect assets, colors, anchors, and visual clip timing. The JSON in this branch is a design contract, not a runtime loader or a Butano import file.
- **Renderer:** selects an effect from fighter ID and normal/super attack type; it must not infer identity from the current sprite palette or edit combat state.
- **Transient effect pool:** bounded impact/trail instances, explicit expiry, and deterministic reuse. Spawn from confirmed events and remember event IDs if the renderer can observe an event across several display frames.
- **Resource loading:** load the two selected fighters' effect resources at battle setup, share identical common palettes, and release battle-only resources on result/menu transitions.

If the baseline exposes no event stream, add the smallest adapter for confirmed combat outcomes. Do not rewrite the combat core just to add particles. Keep CPU behavior unchanged during this pass so comparisons are meaningful.

## GBA asset and performance targets

These are proposed budgets, not measurements of a new build:

| Asset per fighter | Frame size | Frames | Raw 4 bpp tile bytes |
| --- | --- | --- | --- |
| Normal travel | 16 × 16 | 4 | 512 |
| Impact | 16 × 16 | 4 | 512 |
| Super travel | 32 × 32 | 4 | 2,048 |
| Total per fighter | | 12 | 3,072 |
| All six fighters | | 72 | 18,432 |

Counts exclude palettes, common cues, metadata, alignment, and conversion overhead. They are ROM tile-data estimates, not a claim that all frames fit concurrently in OBJ VRAM. Verify residency for the two active fighters together with characters, HUD, shadows, and current effects.

Use indexed 4 bpp sprites: at most 16 palette entries including the transparent entry. Keep a dark edge and bright core rather than relying on alpha blending. Preserve palette index order across an animation. Use the restored project's converter and pinned engine version instead of switching toolchains during this change.

Start with a shared pool of at most 12 decorative transient sprites. Budget live projectiles and warning cues separately; visual overflow should drop old trail/impact decoration before affecting an attack core or telegraph. Hidden sprites still hold engine resources. Audit palette use, total sprites, and concentration on a single scanline; avoid routine affine scaling and resource allocation during battle updates.

Target the baseline frame cadence with zero missed display frames in the native stress replay. Profile both fighters firing supers and impacts at the same time, under all six effect identities. If time or resource use rises too far, simplify decoration rather than changing gameplay timing.

Engine references: [asset imports](https://gvaliente.github.io/butano/import.html), [palette sharing and sprite limits](https://gvaliente.github.io/butano/faq.html). Check these APIs against the restored vendored version before implementation.

## Acceptance criteria

- All six normal attacks and supers are distinct in shape and motion; a grayscale inspection still distinguishes the six normal cores.
- Every normal attack has visible windup, travel, hit, block, and expiry outcomes at native resolution.
- Both facings and fighters spawn effects at the correct anchors; overlapping projectiles retain clear ownership and travel direction.
- Supers still bypass block and permit the baseline jump/squeeze evasion. Decorative particles never damage, interrupt, or charge super power.
- Pause, hitstop if already present, round transitions, and menu transitions leave no stale effects, palette leaks, or off-by-one impact replays.
- Record normal/super demonstrations for all six fighters, including a hit, a block where allowed, a jump dodge, and a squeeze dodge.
- Rerun the restored combat and movement suites, asset checks, and native emulator flow. Compare gameplay state traces against v0.2.0 for the presentation-only pass.
- Clean-checkout build succeeds with pinned dependencies. Capture resource peaks and native frame timing rather than inferring them from host tests.

## Work order and current blocker

1. Restore `PokemonBattleArena_Game_and_Source.zip` and verify the baseline clean build.
2. Inspect fighter IDs, shot/wave assets, rendering code, and combat-outcome seams. Adapt this design to those existing structures.
3. Implement Pikachu and Squirtle first: their angular and rounded cores provide a clear test of whether the renderer supports different identities.
4. Complete the other four profiles and confirmed hit/block/expiry effects.
5. Run acceptance checks, record native footage, and open the playable candidate for review.
6. Merge only after integration works; create a new tagged release and checksum for the new ROM.

The source is restored and implementation is complete: 30 effect sheets / 120 frames, per-character impacts and muzzle/guard effects, a bounded decorative pool, and a rebuilt ROM. See ../ELEMENTAL_ATTACKS_TEST_REPORT.md for validation and limitations.
