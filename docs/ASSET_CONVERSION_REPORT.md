# Native assets

Source PNG/GIF art and credits are retained under `assets/source/`. The converter uses the supplied screen renderer and roster PNGs, weighted median-cut palettes snapped to 15-bit display precision, and deterministic uncompressed indexed BMP output.

- Six fighter sprites: legal 64×64 hardware objects, 16-color palettes, foot anchor (32,58).
- Backgrounds: 256×256 with the intended 240×160 view preserved at the top left; no stretching.
- Title background and foreground ensemble share an identical 256-entry palette. The ensemble is a separate native background with a gentle bob; sparkle objects animate separately.
- Other backgrounds are scene-local and do not coexist with independently quantized 8-bit palettes.
- Bars use legal 64×32 object frames, scaled to the visible meter height. Projectiles, guard effect and focus indicator use legal object dimensions.
- Native UI uses an original 5×7 bitmap font packed into 8×8 glyph cells. Button labels and the arena HUD were adapted to native controls.
- Sound effects are original short synthesized WAV samples embedded by the native audio pipeline.

The build's graphics importer validated dimensions and emitted native headers. Battle background graphics are approximately 37 KB including palette/map metadata; title background plus ensemble approximately 58 KB. The title has two background layers; battle uses one. Title usage is higher than the battle target and remains within tiled BG storage, as exercised by native scene transitions.

Current animation level: 408 imported frames across 13 clips per character. Each walk cycle has four to seven source frames, including repeated poses, with three distinct rendered images. The runtime uses authored body/limb changes rather than one transformed image. Attack phases align with gameplay timing. Jump poses follow physics; compact squeeze artwork fits the ten-pixel active hurtbox.

`tools/prepare_animations.cjs` imports the included original PNG/XML-derived metadata into anchored 64×64 sheets and C++ clip data. `tools/convert_assets.cjs` emits indexed BMPs. Each fighter uses a shared 16-color palette across all its frames. Only the active frame occupies object tile memory. Effects use legal 16×16 dust, 32×32 impact and 32×8 shadow objects. Original generated bonus sheets remain unused reference art.

All 408 cells pass alpha bounds, ground anchor, palette and clip checks in `assets_validation.json`. Raw sheets, offsets, metadata and original CHUNSOFT credits are preserved under `assets/source/animations/`. See `ANIMATION_AND_PHYSICS.md` for the current runtime contract.
