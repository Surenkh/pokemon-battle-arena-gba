// Native pixel artwork: four Poké Ball frames, a recall beam and seal glints.
exports.buildRoundEffects=({canvas,save})=>{
 const colors={edge:'#182339',red:'#e45b68',redLight:'#ff8b8b',white:'#ecf8f4',shade:'#9cb6c4',pink:'#ffb4d4',hot:'#fff2fa'};
 const balls=canvas(32,128),ctx=balls.getContext('2d');
 for(let f=0;f<4;f++){
  const shift=f===1?2:f===2?5:0;
  if(shift){ctx.fillStyle=colors.pink;ctx.fillRect(11,f*32+11,10,10);ctx.fillStyle=colors.hot;ctx.fillRect(14,f*32+12,4,8);}
  for(let y=-9;y<=9;y++)for(let x=-9;x<=9;x++){
   if(x*x+y*y>81)continue;
   const upper=y<0,yy=f*32+16+y+(upper?-shift:shift/2|0);
   let color=x*x+y*y>=65?colors.edge:Math.abs(y)<2?colors.edge:upper?(x<-2&&y<-3?colors.redLight:colors.red):(y>5?colors.shade:colors.white);
   ctx.fillStyle=color;ctx.fillRect(16+x,yy,1,1);
  }
  const cy=f*32+16+(shift/2|0);ctx.fillStyle=colors.edge;ctx.fillRect(13,cy-3,6,7);ctx.fillRect(12,cy-2,8,5);ctx.fillStyle=f===3?colors.pink:colors.white;ctx.fillRect(14,cy-2,4,5);ctx.fillRect(13,cy-1,6,3);ctx.fillStyle=f===3?colors.hot:colors.shade;ctx.fillRect(15,cy-1,2,3);
 }
 save('recall_ball',balls,'sprite',32,32);
 const beam=canvas(32,64),b=beam.getContext('2d');
 for(let y=0;y<64;y++){let half=3+Math.floor(y*10/63);for(let x=16-half;x<=16+half;x++)if(Math.abs(x-16)<3||(x+y)%2===0){b.fillStyle=Math.abs(x-16)<3?colors.hot:Math.abs(x-16)>half-2?colors.pink:'#f578b8';b.fillRect(x,y,1,1);}}
 save('recall_beam',beam,'sprite',32,64);
 const glint=canvas(64,256),g=glint.getContext('2d');
 for(let f=0;f<4;f++)for(let i=0;i<4;i++){const x=32+[-1,1,-1,1][i]*(9+f*4),y=f*64+32+[-1,-1,1,1][i]*(7+f*3);g.fillStyle=i%2?colors.hot:'#ffe58d';g.fillRect(x-3,y,7,1);g.fillRect(x,y-3,1,7);if(f<2)g.fillRect(x-1,y-1,3,3);}
 save('recall_glint',glint,'sprite',64,64);
};
