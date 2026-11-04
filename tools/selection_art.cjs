// Native-size, one-screen setup art. All names remain readable without paging.
const fs=require('fs'),path=require('path');
const {loadImage}=require('@napi-rs/canvas');
const glyphs={A:[2,5,7,5,5],B:[6,5,6,5,6],C:[3,4,4,4,3],D:[6,5,5,5,6],E:[7,4,6,4,7],F:[7,4,6,4,4],G:[3,4,5,5,3],H:[5,5,7,5,5],I:[7,2,2,2,7],J:[1,1,1,5,2],K:[5,5,6,5,5],L:[4,4,4,4,7],M:[5,7,7,5,5],N:[5,7,7,7,5],O:[2,5,5,5,2],P:[6,5,6,4,4],Q:[2,5,5,3,1],R:[6,5,6,5,5],S:[3,4,2,1,6],T:[7,2,2,2,2],U:[5,5,5,5,7],V:[5,5,5,5,2],W:[5,5,7,7,5],X:[5,5,2,5,5],Y:[5,5,2,2,2],Z:[7,1,2,4,7],' ':[0,0,0,0,0]};
function tiny(ctx,text,x,y,color='#edf8f0'){text=text.toUpperCase();x-=Math.floor((text.length*4-1)/2);ctx.fillStyle=color;for(const c of text){const rows=glyphs[c]||glyphs[' '];rows.forEach((row,dy)=>{for(let dx=0;dx<3;dx++)if(row&(4>>dx))ctx.fillRect(x+dx,y+dy,1,1);});x+=4;}}
async function buildSelection({root,source,names,canvas,save,bg,ui}){
 const layout=JSON.parse(fs.readFileSync(path.join(root,'config/selection.json'))),manifest=JSON.parse(fs.readFileSync(path.join(source,'animations/manifest.json')));
 const portraits=await Promise.all(names.map(async name=>({image:await loadImage(path.join(source,'animations/packed_'+name+'.png')),first:manifest.characters[name].clips.Idle.first})));
 function card(ctx,who,x,y,body){ui.box(ctx,x,y,layout.card_width,layout.card_height,'#142a3c','#365573');if(body){const p=portraits[who];ctx.drawImage(p.image,0,p.first*64,64,64,x+14,y,16,16);}tiny(ctx,names[who],x+22,y+19);}
 for(const [name,title,training]of[['selector','QUICK BATTLE',false],['arcade_selector','ARCADE',false],['cup_selector','CUP FIGHTER',false],['practice_setup','PRACTICE',true],['versus_selector','LOCAL VERSUS',false]]){
  const scene=await bg('screens/layers/selector_background.png'),ctx=scene.getContext('2d');
  ctx.fillStyle='#0c1c2c';ctx.fillRect(0,0,240,16);ui.text(ctx,title,10,5,'#ffda6b');
  for(let who=0;who<names.length;++who)card(ctx,who,layout.grid_x+(who%layout.columns)*layout.step_x,layout.grid_y+Math.floor(who/layout.columns)*layout.step_y,true);
  if (name === 'versus_selector') {
   for (const y of [108,120,132]) {
    ctx.fillStyle = y === 120 ? '#112336' : '#15283b';
    ctx.fillRect(8, y - 6, 224, 12);
   }
   ui.text(ctx, 'LOCATION', 12, 129, '#d9e9e9');
   ctx.fillStyle = '#365573';
   ctx.fillRect(8, 140, 224, 1);
   ui.text(ctx, 'P1 ARROWS J/K', 8, 144, '#6cc5ef');
   ui.text(ctx, 'P2 WASD F/G', 128, 144, '#ffda6b');
   ui.text(ctx, 'U/I LOCATION  ENTER/T PAUSE', 120, 154, '#9db8c6', 1, 'center');
   save(name, scene, 'background');
   continue;
  }
  const rows=training?layout.practice_rows:layout.quick_rows,labels=training?['TRAINING','RIVAL','LOCATION','FIGHTER INFO']:['DIFFICULTY',name==='cup_selector'?'CUP ROUTE':'LOCATION','FIGHTER INFO'];
  labels.forEach((label,i)=>{const y=rows[i+1];ctx.fillStyle=i%2?'#112336':'#15283b';ctx.fillRect(8,y-6,224,12);ui.text(ctx,label,24,y-3,'#d9e9e9');if(i<labels.length-1&&!(['arcade_selector','cup_selector'].includes(name)&&i===1)){ui.text(ctx,'<',108,y-3,'#93dbcd');ui.text(ctx,'>',226,y-3,'#93dbcd');}else if(i===labels.length-1)ui.text(ctx,'>',226,y-3,'#93dbcd');});
  ctx.fillStyle='#365573';ctx.fillRect(8,150,224,1);
  if(name==='arcade_selector'){tiny(ctx,'ALL FIVE ARENAS',188,81,'#9db8c6');tiny(ctx,'ROTATE',188,91,'#ffda6b');}
  if(name==='cup_selector'){tiny(ctx,'THREE SET RIVALS',188,81,'#9db8c6');tiny(ctx,'FIXED ROUTE',188,91,'#ffda6b');}
  save(name,scene,'background');
 }
 const plate=canvas(64,32*names.length),ctx=plate.getContext('2d');for(let who=0;who<names.length;++who)card(ctx,who,10,who*32+3,false);save('selected_card',plate,'sprite',64,32);
 const previews=canvas(64,32*5),pc=previews.getContext('2d');pc.imageSmoothingEnabled=true;
 for(const [i,name]of['arena','arena_volcanic','arena_shoreline','arena_frostpeak','arena_ruins'].entries()){
  const image=await loadImage(path.join(root,'docs/'+name+'.png'));pc.fillStyle='#93dbcd';pc.fillRect(0,i*32,64,32);pc.drawImage(image,0,44,240,96,1,i*32+1,62,30);
 }
 save('arena_previews',previews,'sprite',64,32);

 const cursors = canvas(64, 64);
 const cursorContext = cursors.getContext('2d');
 const colors = ['#6cc5ef', '#ffda6b'];
 for (let player = 0; player < 2; ++player) {
  const inset = player;
  cursorContext.strokeStyle = colors[player];
  cursorContext.lineWidth = 1;
  cursorContext.strokeRect(10.5 + inset, player * 32 + 3.5 + inset, 43 - inset * 2, 25 - inset * 2);
 }
 save('versus_cursor', cursors, 'sprite', 64, 32);

 const markers = canvas(8, 16);
 const markerContext = markers.getContext('2d');
 const digits = [[2,6,2,2,7],[6,1,2,4,7]];
 for (let player = 0; player < 2; ++player) {
  markerContext.fillStyle = '#07101e';
  markerContext.fillRect(0, player * 8, 8, 8);
  markerContext.fillStyle = colors[player];
  markerContext.fillRect(0, player * 8 + 7, 8, 1);
  digits[player].forEach((row, y) => {
   for (let x = 0; x < 3; ++x) {
    if (row & (4 >> x)) markerContext.fillRect(2 + x, player * 8 + y + 1, 1, 1);
   }
  });
 }
 save('versus_marker', markers, 'sprite', 8, 8);

 for (const name of ['versus_pause', 'versus_result']) {
  const scene = await bg('screens/layers/selector_background.png');
  const context = scene.getContext('2d');
  ui.box(context, 4, 4, 232, 152, '#122639', '#6085a1');
  if (name === 'versus_pause') {
   ui.text(context, 'LOCAL VERSUS', 120, 13, '#ffda6b', 2, 'center');
   ['RESUME', 'RESTART SET', 'CHANGE POKEMON', 'MODE MENU'].forEach((label, i) => {
    ui.text(context, label, 120, 62 + i * 18, '#edf8f0', 1, 'center');
   });
   ui.text(context, 'J/F SELECT  K/G BACK', 120, 145, '#9db8c6', 1, 'center');
  } else {
   ui.text(context, 'LOCAL VERSUS', 120, 13, '#ffda6b', 2, 'center');
   ui.box(context, 12, 127, 216, 12, '#15283b', '#365573');
   ui.box(context, 12, 142, 216, 12, '#15283b', '#365573');
  }
  save(name, scene, 'background');
 }
}
module.exports={buildSelection,tiny,glyphs};
