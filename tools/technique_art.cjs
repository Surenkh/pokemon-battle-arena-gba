// The outlined warning and filled blast share the actual 28x20 burst footprint.
function buildTechniqueArt({canvas,save}) {
 const image=canvas(32,32*8),ctx=image.getContext('2d');
 for(let frame=0;frame<8;frame++) {
  const top=frame*32,active=frame>=4;
  for(let y=6;y<=26;y++)for(let x=2;x<=30;x++) {
   const distance=(x-16)**2/196+(y-16)**2/100;
   if(distance>1)continue;
   if(!active) {
    if(distance>.76)ctx.fillStyle=frame%2?'#d8a8ff':'#9963dc';
    else if(distance<.11)ctx.fillStyle=frame%2?'#8b4cc3':'#6d389a';
    else continue;
   } else {
    ctx.fillStyle=distance<.22?['#f8e8ff','#e4b8ff','#d394f0','#a86bdd'][frame-4]:
                  distance>.72?'#8547b8':(x+y+frame)%3?'#bd7fe4':'#e4b8ff';
   }
   ctx.fillRect(x,top+y,1,1);
  }
  // Four inward ticks make the stationary arming phase visibly different.
  if(!active) {
   ctx.fillStyle='#f0c8ff';
   const shift=frame%3;
   ctx.fillRect(5+shift,top+15,3,2);ctx.fillRect(24-shift,top+15,3,2);
   ctx.fillRect(15,top+8+shift,2,2);ctx.fillRect(15,top+22-shift,2,2);
  }
 }
 save('shadow_burst',image,'sprite',32,32);
}
module.exports={buildTechniqueArt};
