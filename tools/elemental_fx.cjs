// Pixel-authored elemental VFX. No external artwork or simulation randomness.
// All four sheets for a fighter share the same indexed palette.
const colors=[
 ['#191530','#795007','#B98415','#F4BA28','#FFE24C','#FFF4AC','#FFFFFF'],
 ['#29172B','#83271F','#BF3D22','#F57926','#FFB847','#FFE99A','#FFFFFF'],
 ['#142844','#204F93','#2C84CA','#49BADD','#83E5EF','#C9F7FF','#FFFFFF'],
 ['#18312C','#285D39','#43924C','#7BB94C','#B5E66D','#E5F9B6','#FFFFFF'],
 ['#30213C','#76516D','#B07D9F','#E5B3C9','#F2D888','#FFF1C5','#FFFFFF'],
 ['#29213C','#69506F','#A66B98','#DF9CC8','#BFCDDF','#EAF3FF','#FFFFFF'],
 ['#22193E','#533284','#8054BE','#B181EF','#D5B5FA','#EEE0FF','#FFFFFF'],
 ['#3B203D','#924D83','#CF75AB','#EF9EC8','#FFD2E9','#FFF0FA','#FFFFFF'],
 ['#192C36','#355865','#5D9195','#93BABB','#D0C58A','#FFF1BA','#FFFFFF'],
 ['#321A24','#7D3322','#BB5927','#E88437','#FFBA61','#FFE3A0','#FFFFFF'],
 ['#201A38','#49305F','#78519D','#AC7CD0','#D1A9EE','#E6D4FF','#FFFFFF'],
 ['#172E45','#295D87','#378BC1','#65BBDC','#A9E8F4','#D8FBFF','#FFFFFF'],
 ['#142D3F','#1D5B7D','#2A8DA4','#57BBC6','#9CE1DA','#D4FFF0','#FFFFFF']
];

// Integer rasterization avoids antialiased fringe colors at GBA resolution.
function pen(ctx,palette){
 const dot=(x,y,c,w=1)=>{ctx.fillStyle=palette[c];ctx.fillRect(Math.round(x),Math.round(y),w,w);};
 function line(x0,y0,x1,y1,c,w=1){
  x0=Math.round(x0);y0=Math.round(y0);x1=Math.round(x1);y1=Math.round(y1);
  const dx=Math.abs(x1-x0),sx=x0<x1?1:-1,dy=-Math.abs(y1-y0),sy=y0<y1?1:-1;let err=dx+dy;
  for(;;){dot(x0-Math.floor(w/2),y0-Math.floor(w/2),c,w);if(x0===x1&&y0===y1)break;const e=err*2;if(e>=dy){err+=dy;x0+=sx;}if(e<=dx){err+=dx;y0+=sy;}}
 }
 function poly(points,c){
  const ys=points.map(p=>p[1]);
  for(let y=Math.min(...ys);y<=Math.max(...ys);y++){
   const xs=[];
   for(let i=0,j=points.length-1;i<points.length;j=i++){
    const a=points[j],b=points[i];if((a[1]<=y&&b[1]>y)||(b[1]<=y&&a[1]>y))xs.push(a[0]+(y-a[1])*(b[0]-a[0])/(b[1]-a[1]));
   }
   xs.sort((a,b)=>a-b);for(let i=0;i+1<xs.length;i+=2)line(Math.ceil(xs[i]),y,Math.floor(xs[i+1]),y,c);
  }
 }
 function disk(x,y,rx,ry,c){for(let yy=-ry;yy<=ry;yy++)for(let xx=-rx;xx<=rx;xx++)if(xx*xx*ry*ry+yy*yy*rx*rx<=rx*rx*ry*ry)dot(x+xx,y+yy,c);}
 function chain(points,c,w=1){for(let i=1;i<points.length;i++)line(...points[i-1],...points[i],c,w);}
 function star(x,y,r,c){poly([[x,y-r],[x+2,y-2],[x+r,y],[x+2,y+2],[x,y+r],[x-2,y+2],[x-r,y],[x-2,y-2]],c);}
 function ring(x,y,rx,ry,c,w=1){const pts=[];for(let i=0;i<=20;i++){const a=i*Math.PI/10;pts.push([x+Math.cos(a)*rx,y+Math.sin(a)*ry]);}chain(pts,c,w);}
 return{dot,line,poly,disk,chain,star,ring};
}

function travel(p,id,f,superAttack){
 const y=superAttack?8:16,x=16;
 if(id===0){
  const pts=superAttack?[[2,y+2],[9,y-3],[13,y+2],[21,y-2],[29,y]]:[[7,y+2],[12,y-3],[15,y+2],[20,y-3],[24,y]];
  p.chain(pts,1,5);p.chain(pts,4,3);p.chain(pts,7);
  p.chain([[12,y],[10,y-(f%2?6:4)],[7,y-4]],5);
  p.chain([[21,y],[24,y+(f%2?5:3)],[27,y+3]],4);
  p.dot(5+f*2,y-5+(f%2),6);
 }else if(id===1){
  const tail=superAttack?2:6,tip=superAttack?30:24;
  p.poly([[tail,y-3-f%2],[14,y-5],[tip,y],[14,y+5],[tail,y+4],[10,y]],1);
  p.poly([[tail+2,y-2],[16,y-4],[tip-1,y],[16,y+4],[tail+2,y+2],[13,y]],3);
  p.poly([[12,y-2],[22,y-2],[tip-2,y],[18,y+3]],5);p.line(16,y,tip-4,y,7);
  p.dot(tail+(f*3)%6,y-6,4,2);p.dot(tail+2+(f*2)%5,y+5,3);
 }else if(id===2){
  const rx=superAttack?10:6;
  p.disk(x+2,y,rx,superAttack?4:6,1);p.disk(x+2,y,rx-1,superAttack?3:5,3);
  p.line(x,y-3,x+4,y-3,5);p.line(x+4,y-2,x+6,y-2,6);p.dot(x+4,y-1,7);
  p.line(4+f,y-2,9+f,y-2,4);p.line(5+(f*2)%4,y+3,9+(f*2)%4,y+3,5);
  p.dot(3+f,y+1,5,2);p.line(x-2,y+2,x+3,y+2+(f%2),4);
 }else if(id===3){
  const d=f%2?1:0;
  if(superAttack){p.chain([[2,8],[8,5+d],[14,9],[21,5],[29,8]],2,3);p.chain([[3,7],[10,10-d],[17,6],[25,10]],4);}
  const leaves=f%2?[[[10,21],[10,13],[20,10],[20,16]],[[13,10],[22,13],[23,21],[16,19]]]:[[[7,18],[12,10],[22,12],[18,17]],[[10,23],[13,16],[24,16],[20,23]]];
  const dy=superAttack?-8:0;
  for(const leaf of leaves){const pts=leaf.map(([a,b])=>[f>=2?32-a:a,b+dy]);p.poly(pts,1);p.poly(pts.map(([a,b])=>[a+1,b]),4);p.line(pts[0][0]+2,pts[0][1]-1,pts[2][0]-2,pts[2][1]+1,6);}
 }else if(id===4){
  if(superAttack){p.star(7,8,4,3);p.line(1,6,10,6,4);p.line(3,11,12,11,5);}
  p.star(x+3,y,superAttack?7:8,1);p.star(x+3,y,superAttack?6:7,5);p.star(x+3,y,f%2?4:3,7);
  p.dot(6+f,y-5,4);p.dot(8+(f%2)*2,y+5,6,2);
 }else if(id===6){
  p.ring(x+1,y,5+f,superAttack?3:5+(f%2),2,2);p.ring(x+1,y,3+f,superAttack?2:3,5);p.star(x+1,y,2,7);
  p.line(3+f,y-2,9+f,y-2,4);p.line(4+f,y+3,10+f,y+3,6);if(superAttack)p.ring(25,y,3,5,4);
 }else if(id===7){
  const n=17+(f%2),stem=superAttack?5:9;
  p.disk(n-2,y+3,3+(f>>1),2,2);p.disk(n-2,y+2,2,1,6);p.line(n+1,y+3,n+1,y-stem,5,2);p.line(n+1,y-stem,n+7,y-stem+2+f,6,2);
  if(superAttack){p.ring(7,y,3+f,4,4);p.dot(27,y+f-2,7,2);}else p.star(6+f,y-3,2,5);
 }else if(id===8){
  p.poly([[11,y-4],[19,y-6],[26,y-3],[26,y+4],[18,y+6],[11,y+3]],1);
  p.poly([[13,y-3],[20,y-4],[24,y-2],[24,y+3],[18,y+4],[13,y+2]],4);
  for(let k=0;k<3;k++)p.line(16+k*3,y-4,16+k*3,y,6);p.line(20,y+1,24,y+1,7);
  p.line(1+f,y-3,9+f,y-3,5);p.line(3+f,y+4,10+f,y+4,3);p.dot(8+f*2,y-6,6,2);
 }else if(id===9){
  for(const dy of[-3,3]){p.poly([[4,y+dy+2],[12,y+dy-4-f%2],[26,y+dy],[12,y+dy+4]],2);p.poly([[10,y+dy+1],[16,y+dy-2],[24,y+dy],[15,y+dy+2]],5);p.dot(5+f*2,y+dy-4,4);}
 }else if(id===10){
  p.disk(18,y,6,5,1);p.disk(18,y,5,4,3);p.ring(18,y,9,superAttack?5:8,5);p.star(18,y,2,7);p.dot(6+f,y-5,4,2);p.dot(27-f,y+5,6);
 }else if(id===11){
  p.poly([[6,y],[17,y-7],[29,y],[17,y+7]],2);p.poly([[12,y],[18,y-4],[25,y],[18,y+4]],5);p.line(7,y,28,y,7);p.line(2+f,y-3,8+f,y-3,4);p.line(3+f,y+4,9+f,y+4,5);
 }else if(id===12){
  p.chain([[6,y-5],[13,y-7],[26,y-3],[22,y],[25,y+4],[12,y+7],[6,y+5]],3,3);for(let i=0;i<3;i++){p.poly([[11+i*4,y-5],[13+i*4,y-1],[15+i*4,y-5]],6);p.poly([[11+i*4,y+5],[13+i*4,y+1],[15+i*4,y+5]],5);}p.dot(4+f,y,4,2);
 }else{
  for(let i=0;i<3;i++){
   const yy=y+(i-1)*(superAttack?4:5),off=(f+i)%2;
   const pts=[[superAttack?3:9,yy+3],[16,yy+1],[superAttack?29:24,yy-3+off]];
   p.chain(pts,1,3);p.chain(pts,4,2);p.chain(pts,7);
  }
  p.dot(7+f*3,y-6,5);p.dot(8+f*2,y+6,6);
 }
}

function contact(p,id,f,blocked=false){
 const x=16,y=16,r=3+f*3;
 if(blocked){
  p.chain([[19-r,y-6],[15-r,y-3],[14-r,y+3],[18-r,y+6]],6,2);
  p.line(x,y-2,x+2+f,y-4-f,4);p.line(x,y+2,x+3+f,y+4+f,5);return;
 }
 if(id===0){for(let i=0;i<6;i++){const a=i*Math.PI/3;const dx=Math.round(Math.cos(a)*r),dy=Math.round(Math.sin(a)*r);p.chain([[x+dx,y+dy],[x+Math.round(dx*.4)+2,y+Math.round(dy*.4)-1],[x,y]],i%2?5:4,2);}if(f<2)p.star(x,y,4-f,7);}
 else if(id===1){for(let i=0;i<5;i++){const dx=(i-2)*(f+1);p.poly([[x+dx-2,y+3+f],[x+dx-1,y-3-f*2-(i%2)*2],[x+dx+3,y+1+f]],i%2?3:5);}if(f<2)p.disk(x,y,3-f,4-f,7);}
 else if(id===2){p.chain([[x-r,y+1],[x-r/2,y+3],[x,y+1],[x+r/2,y+3],[x+r,y+1]],4,2);for(let i=0;i<5;i++){const dx=(i-2)*(f+1);p.disk(x+dx,y-3-f-(i%2)*3,1,2,i%2?5:6);}if(f===0)p.disk(x,y,3,2,7);}
 else if(id===3){for(let i=0;i<4;i++){const a=i*Math.PI/2+f*.2,dx=Math.round(Math.cos(a)*r),dy=Math.round(Math.sin(a)*r);p.poly([[x+dx-2,y+dy],[x+dx,y+dy-3],[x+dx+3,y+dy],[x+dx,y+dy+2]],i%2?4:5);}if(f<2)p.star(x,y,3,6);}
 else if(id===4){for(let i=0;i<4;i++){const dx=(i%2?1:-1)*(3+f*2),dy=(i<2?-1:1)*(3+f*2);p.star(x+dx,y+dy,f<2?3:2,i%2?5:6);}if(f<2)p.star(x,y,4-f,7);}
 else if(id===6){p.ring(x,y,r,Math.max(2,r-2),4,2);p.star(x,y,f<2?4:2,7);p.line(x-r,y+f,x+r,y-f,6);}
 else if(id===7){for(let i=0;i<3;i++){const xx=x+(i-1)*(3+f*2);p.disk(xx,y+f,2,1,5);p.line(xx+2,y+f,xx+2,y-5-f,6);}if(f<2)p.star(x,y,3,7);}
 else if(id===8){for(let i=0;i<6;i++){const a=i*Math.PI/3;p.line(x+Math.cos(a)*3,y+Math.sin(a)*3,x+Math.cos(a)*r,y+Math.sin(a)*r,i%2?5:6,2);}if(f<2)p.disk(x,y,4-f,3-f,7);}
 else if(id===9){for(let i=0;i<4;i++)p.poly([[x+(i-2)*3,y+4],[x+(i-2)*3+1,y-r],[x+(i-2)*3+4,y+3]],i%2?4:6);}
 else if(id===10){p.ring(x,y,r,r-1,4,2);p.disk(x,y,3,3,2);p.star(x,y,2,7);}
 else if(id===11){p.poly([[x-r,y],[x,y-r],[x+r,y],[x,y+r]],4);p.star(x,y,Math.max(2,5-f),7);}
 else if(id===12){p.chain([[x-r,y-r],[x,y+f],[x+r,y-r]],5,2);p.chain([[x-r,y+r],[x,y-f],[x+r,y+r]],4,2);if(f<2)p.disk(x,y,3,2,7);}
 else{for(let i=0;i<3;i++)p.chain([[x-7+i*4,y+7-f],[x-3+i*4,y+1],[x+i*4,y-6-f]],f<2?7:4,2);}
}

// Eight-frame energy ribbons retain each fighter's original projectile motif.
function superTravel(ctx,p,id,f){
 const bend=Math.round(Math.sin(f*Math.PI/4)*4);
 p.chain([[3,16+bend],[10,12-bend],[20,19+bend],[28,16]],1,5);
 p.chain([[3,16+bend],[10,12-bend],[20,19+bend],[28,16]],4,3);
 p.chain([[3,16+bend],[10,12-bend],[20,19+bend],[28,16]],6);
 p.chain([[5,23-bend],[14,18+bend],[24,21-bend],[31,17]],4,2);
 p.line(2+f,8+(f%3),20+f,8+(f%3),5);
 p.ring(32,16,10+(f%2),7+(f%3),1,2);p.ring(32,16,9+(f%2),6+(f%3),4);
 ctx.save();ctx.translate(16,8);travel(p,id,f%4,true);ctx.restore();
 const angle=f*Math.PI/4;p.star(32+Math.round(Math.cos(angle)*11),16+Math.round(Math.sin(angle)*9),2,6);
 p.disk(32,16,2+(f%2),2,7);
}
function aura(p,f){
 p.ring(16,16,11+(f%2),12-(f%3),2);p.ring(16,16,8,9,4);
 for(let i=0;i<3;i++){const a=f*Math.PI/4+i*2*Math.PI/3;p.star(16+Math.round(Math.cos(a)*12),16+Math.round(Math.sin(a)*12),i?1:2,i?4+i:7);}
 p.line(13,16,19,16,6);p.line(16,13,16,19,5);
}
function punch(p,f){
 const x=[10,13,18,21,23,24][f],y=16;
 if(f<4){
  p.poly([[x-7,y-4],[x-3,y-7],[x+5,y-6],[x+9,y-2],[x+9,y+4],[x+3,y+7],[x-6,y+4]],1);
  p.poly([[x-5,y-3],[x-2,y-5],[x+4,y-4],[x+7,y-1],[x+7,y+3],[x+2,y+5],[x-4,y+3]],4);
  for(let k=0;k<3;k++)p.line(x-1+k*3,y-4,x-1+k*3,y,6);
  p.line(x+2,y+1,x+6,y+1,7);
 }
 for(let i=0;i<3;i++)p.line(Math.max(1,x-14),8+i*7,x-7,7+i*7,3+i);
 if(f>=2){const rad=4+(f-2)*3;p.chain([[x-2,16-rad],[x+3,16-rad/2],[x+5,16],[x+3,16+rad/2],[x-2,16+rad]],f<4?7:4,2);}
 if(f>=4){p.star(8+f,7,2,5);p.star(12+f,24,1,4);}
}
function burst(p,id,f){
 if(f<4)contact(p,id,f,false);
 const radius=3+f*2;p.ring(16,16,radius,Math.max(2,radius-2),f<3?6:4,2);
 for(let i=0;i<5;i++){const a=i*Math.PI*2/5+f*.12;p.line(16+Math.cos(a)*(radius-2),16+Math.sin(a)*(radius-2),16+Math.cos(a)*(radius+2),16+Math.sin(a)*(radius+2),i%2?5:6);}
 if(f<3)p.star(16,16,5-f,7);
}
function buildElementalEffects({canvas,save}){
 for(let id=0;id<colors.length;id++){
  const rgb=colors[id].map(c=>[1,3,5].map(i=>parseInt(c.slice(i,i+2),16))),palette=[[255,0,255],...rgb];
  for(const[kind,w,h,count]of[['shot',32,32,4],['wave',64,32,8],['hit',32,32,4],['cast',16,16,4],['guard',32,32,4],['aura',32,32,8],['punch',32,32,6],['burst',32,32,8]]){
   const sheet=canvas(w,h*count),ctx=sheet.getContext('2d');
   for(let f=0;f<count;f++){
    ctx.save();ctx.translate(0,f*h);ctx.beginPath();ctx.rect(0,0,w,h);ctx.clip();const p=pen(ctx,['#FF00FF',...colors[id]]);
    if(kind==='shot')travel(p,id,f,false);
    else if(kind==='wave')superTravel(ctx,p,id,f);
    else if(kind==='aura')aura(p,f);
    else if(kind==='punch')punch(p,f);
    else if(kind==='burst')burst(p,id,f);
    else if(kind==='cast'){ctx.translate(-8,-8);contact(p,id,f);if(f>=2)p.star(16,16,3,f===2?6:4);}
    else contact(p,id,f,kind==='guard');
    ctx.restore();
   }
   save(kind+'_'+id,sheet,'sprite',w,h,palette);
  }
 }
}
module.exports={buildElementalEffects};
