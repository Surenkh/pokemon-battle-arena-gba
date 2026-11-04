const fs = require('fs');
const path = require('path');
const {tiny} = require('./selection_art.cjs');

async function buildCupArt({root, bg, save, ui, canvas}) {
  const cups = JSON.parse(fs.readFileSync(path.join(root, 'config/cups.json')));
  const accents = ['#93dbcd', '#ffda6b', '#bdaddc'];
  for (let i = 0; i < cups.length; ++i) {
    const cup = cups[i];
    const scene = await bg('screens/layers/selector_background.png');
    const ctx = scene.getContext('2d');
    ui.box(ctx, 4, 3, 232, 154, '#122639', '#6085a1');
    ui.text(ctx, '<', 12, 10, accents[i]);
    ui.text(ctx, cup.name, 120, 9, accents[i], 2, 'center');
    ui.text(ctx, '>', 221, 10, accents[i]);
    cup.rivals.forEach((rival, j) => {
      const x = 8 + j * 76;
      ui.box(ctx, x, 43, 72, 49, '#101d32', accents[i]);
      tiny(ctx, rival.fighter, x + 36, 84);
      ui.text(ctx, rival.name, x + 36, 33, accents[i], 1, 'center');
      if (j < 2) ui.text(ctx, '>', x + 70, 64, accents[i]);
    });
    ui.text(ctx, cup.goal, 120, 98, accents[i], 1, 'center');
    ui.text(ctx, 'CLEAR', 12, 111, '#9db8c6');
    ui.text(ctx, 'STYLE', 85, 111, '#9db8c6');
    ui.text(ctx, 'PERF', 158, 111, '#9db8c6');
    ui.text(ctx, 'BEST', 12, 123, '#9db8c6');
    ui.text(ctx, '3 RIVALS / 1 CONTINUE', 228, 123, '#9db8c6', 1, 'right');
    ui.text(ctx, '<> CUP  UP/DOWN LEVEL', 120, 137, '#9db8c6', 1, 'center');
    ui.text(ctx, 'A CHOOSE POKEMON  B MENU', 120, 148, '#edf8f0', 1, 'center');
    save('cup_board_' + i, scene, 'background');
  }
  const result = await bg('screens/layers/selector_background.png');
  const ctx = result.getContext('2d');
  ui.box(ctx, 4, 4, 232, 152, '#122639', '#6085a1');
  ui.box(ctx, 8, 80, 224, 42, '#101d32', '#365573');
  ui.box(ctx, 8, 125, 224, 15, '#15283b', '#365573');
  save('cup_result', result, 'background');
  const panel = canvas(64, 32);
  const panelContext = panel.getContext('2d');
  panelContext.fillStyle = '#122639'; panelContext.fillRect(0, 0, 64, 32);
  panelContext.fillStyle = '#6085a1'; panelContext.fillRect(0, 0, 64, 1); panelContext.fillRect(0, 31, 64, 1);
  save('cup_cue_panel', panel, 'sprite', 64, 32);
}
module.exports = {buildCupArt};
