# Native source assets

This folder retains the native regeneration inputs: animations/ (six original character sets, packed frames, manifests and credits), screens/layers/ (screen/title layers), and screens/ui_renderer.js (bitmap UI helpers). CREDITS.md, SOURCES.json and PROMPTS.json retain provenance.

Historical browser previews, optional generated character alternatives, static roster exports, enlarged exports and large reference paintings are omitted from Git because they are not native build inputs. Historical provenance records may refer to those optional files. All inputs required by tools/prepare_animations.cjs and tools/convert_assets.cjs are retained.

New attack effects are authored reproducibly in tools/elemental_fx.cjs.
