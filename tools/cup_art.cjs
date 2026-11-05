const fs = require('fs');
const path = require('path');

async function buildCupArt({root, bg, save, ui, canvas}) {
  const cups = JSON.parse(fs.readFileSync(path.join(root, 'config/cups.json')));
  const accents = ['#93dbcd', '#ffda6b', '#bdaddc'];
  for (let i = 0; i < cups.length; ++i) {
    const cup = cups[i];
    const scene = await bg('screens/layers/selector_background.png');
    const ctx = scene.getContext('2d');
    const accent = accents[i];
    ui.box(ctx, 4, 3, 232, 154, '#122639', '#6085a1');
    ui.text(ctx, '<', 12, 10, accent);
    ui.text(ctx, cup.name, 120, 9, accent, 2, 'center');
    ui.text(ctx, '>', 221, 10, accent);
    ui.box(ctx, 8, 38, 224, 47, '#101d32', accent);
    ui.text(ctx, 'CAPTAIN ' + cup.captain.name, 70, 45, accent);
    ui.text(ctx, cup.captain.fighter.toUpperCase(), 70, 57, '#edf8f0');
    ui.text(ctx, cup.captain.label, 70, 70, '#9db8c6');
    ui.box(ctx, 188, 45, 38, 31, '#122639', '#365573');
    ui.text(ctx, 'CREST', 207, 78, '#9db8c6', 1, 'center');
    ui.text(ctx, cup.goal, 120, 100, accent, 1, 'center');
    ui.text(ctx, 'CLEAR', 12, 111, '#9db8c6');
    ui.text(ctx, 'STYLE', 85, 111, '#9db8c6');
    ui.text(ctx, 'PERF', 158, 111, '#9db8c6');
    ui.text(ctx, 'BEST', 12, 123, '#9db8c6');
    ui.text(ctx, '1 CONTINUE', 228, 123, '#9db8c6', 1, 'right');
    ui.text(ctx, '<> CUP  UP/DOWN LEVEL', 120, 137, '#9db8c6', 1, 'center');
    ui.text(ctx, 'A CHOOSE POKEMON  B MENU', 120, 148, '#edf8f0', 1, 'center');
    save('cup_board_' + i, scene, 'background');
  }

  const picker = await bg('screens/layers/selector_background.png');
  const pickerContext = picker.getContext('2d');
  ui.box(pickerContext, 4, 4, 232, 152, '#122639', '#6085a1');
  ui.text(pickerContext, 'CHOOSE YOUR RIVAL', 120, 11, '#ffda6b', 2, 'center');
  for (let i = 0; i < 3; ++i) ui.box(pickerContext, 8 + i * 76, 39, 72, 62, '#101d32', '#365573');
  ui.box(pickerContext, 8, 107, 224, 31, '#101d32', '#365573');
  save('cup_picker', picker, 'background');

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

  const crests = canvas(16, 48);
  const crestContext = crests.getContext('2d');
  const symbols = [
    ['00011000', '00110000', '01100000', '11111110', '00011100', '00110000', '01100000', '11000000'],
    ['01111110', '11111111', '11011011', '11000011', '11011011', '01111110', '00111100', '00011000'],
    ['00011000', '00110010', '01100111', '11000010', '11000000', '11100001', '01111110', '00111100']
  ];
  for (let i = 0; i < cups.length; ++i) {
    const y = i * 16;
    crestContext.fillStyle = accents[i];
    crestContext.fillRect(4, y + 1, 8, 14);
    crestContext.fillRect(2, y + 3, 12, 10);
    crestContext.fillRect(1, y + 5, 14, 6);
    crestContext.fillStyle = '#101d32';
    crestContext.fillRect(4, y + 3, 8, 10);
    crestContext.fillRect(3, y + 5, 10, 6);
    crestContext.fillStyle = accents[i];
    symbols[i].forEach((row, dy) => {
      for (let x = 0; x < row.length; ++x) {
        if (row[x] === '1') crestContext.fillRect(4 + x, y + 4 + dy, 1, 1);
      }
    });
  }
  save('cup_crest', crests, 'sprite', 16, 16);
}
module.exports = {buildCupArt};
