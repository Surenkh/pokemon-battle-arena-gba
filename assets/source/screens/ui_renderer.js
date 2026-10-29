(function(global){
'use strict';
const W=240,H=160;
const C={ink:'#edf8f0',muted:'#9db8c6',gold:'#ffda6b',goldDark:'#b57e36',blue:'#78b9df',navy:'#101d32',dark:'#050d1c',panel:'#16283e',line:'#365573',green:'#a7d982',purple:'#bdaddc'};
const GLYPHS={
A:[14,17,17,31,17,17,17],B:[30,17,17,30,17,17,30],C:[14,17,16,16,16,17,14],D:[30,17,17,17,17,17,30],E:[31,16,16,30,16,16,31],F:[31,16,16,30,16,16,16],G:[14,17,16,23,17,17,15],H:[17,17,17,31,17,17,17],I:[14,4,4,4,4,4,14],J:[7,2,2,2,18,18,12],K:[17,18,20,24,20,18,17],L:[16,16,16,16,16,16,31],M:[17,27,21,21,17,17,17],N:[17,25,21,19,17,17,17],O:[14,17,17,17,17,17,14],P:[30,17,17,30,16,16,16],Q:[14,17,17,17,21,18,13],R:[30,17,17,30,20,18,17],S:[15,16,16,14,1,1,30],T:[31,4,4,4,4,4,4],U:[17,17,17,17,17,17,14],V:[17,17,17,17,17,10,4],W:[17,17,17,21,21,21,10],X:[17,17,10,4,10,17,17],Y:[17,17,10,4,4,4,4],Z:[31,1,2,4,8,16,31],
'0':[14,17,19,21,25,17,14],'1':[4,12,4,4,4,4,14],'2':[14,17,1,2,4,8,31],'3':[30,1,1,14,1,1,30],'4':[2,6,10,18,31,2,2],'5':[31,16,16,30,1,1,30],'6':[14,16,16,30,17,17,14],'7':[31,1,2,4,8,8,8],'8':[14,17,17,14,17,17,14],'9':[14,17,17,15,1,1,14],
' ':[0,0,0,0,0,0,0],'!':[4,4,4,4,4,0,4],'.':[0,0,0,0,0,0,4],':':[0,4,0,0,4,0,0],'-':[0,0,0,31,0,0,0],'/':[1,2,2,4,8,8,16],'>':[16,8,4,2,4,8,16],'<':[1,2,4,8,4,2,1],'+':[0,4,4,31,4,4,0],"'":[4,4,0,0,0,0,0]
};
function str(s){return String(s).normalize('NFD').replace(/[\u0300-\u036f]/g,'').toUpperCase();}
function tw(s,scale=1){return Math.max(0,str(s).length*6*scale-scale);}
function text(ctx,s,x,y,color=C.ink,scale=1,align='left'){
s=str(s);if(align==='center')x-=Math.floor(tw(s,scale)/2);if(align==='right')x-=tw(s,scale);
ctx.fillStyle=color;
for(let i=0;i<s.length;i++){const g=GLYPHS[s[i]]||GLYPHS[' '];for(let row=0;row<7;row++)for(let col=0;col<5;col++)if(g[row]&(1<<(4-col)))ctx.fillRect(Math.round(x+i*6*scale+col*scale),Math.round(y+row*scale),scale,scale);}
}
function title(ctx,s,x,y,color,scale=3){
const start=x-Math.floor(tw(s,scale)/2);
for(const [dx,dy]of[[-1,0],[1,0],[0,-1],[0,1],[2,2]])text(ctx,s,start+dx,y+dy,dy===2?'#030919':'#2a5079',scale);
text(ctx,s,start,y,color,scale);
}
function box(ctx,x,y,w,h,fill=C.panel,border=C.line){
ctx.fillStyle=C.dark;ctx.fillRect(x,y,w,h);ctx.fillStyle=border;ctx.fillRect(x+1,y,w-2,h);ctx.fillRect(x,y+1,w,h-2);
ctx.fillStyle=fill;ctx.fillRect(x+2,y+2,w-4,h-4);ctx.fillStyle='rgba(255,255,255,.09)';ctx.fillRect(x+2,y+2,w-4,1);
}
function button(ctx,label,x,y,w,h=18,state='normal'){
const focused=state==='focused',pressed=state==='pressed';
box(ctx,x,y,w,h,pressed?'#8b642a':focused?'#e8b94e':'#234664',focused||pressed?'#ffe79a':'#73a6c5');
if(!pressed){ctx.fillStyle=focused?'#fff0b6':'#96c7e0';ctx.fillRect(x+3,y+3,w-6,1);}
text(ctx,label,x+Math.floor(w/2),y+Math.floor((h-7)/2)+(pressed?1:0),focused||pressed?'#172035':C.ink,1,'center');
}
function star(ctx,x,y,color=C.gold,size=1){
ctx.fillStyle=color;ctx.fillRect(x+size*2,y,size,size*5);ctx.fillRect(x,y+size*2,size*5,size);ctx.fillRect(x+size,y+size,size*3,size*3);
}
function backdrop(ctx,kind,assets,t=0){
ctx.drawImage(assets.arena,0,0,W,H);
ctx.fillStyle=kind==='defeat'?'rgba(8,16,42,.86)':kind==='victory'?'rgba(9,20,35,.78)':kind==='selector'?'rgba(9,20,35,.93)':'rgba(10,21,43,.88)';
ctx.fillRect(0,0,W,H);
if(kind==='title'){
for(let i=0;i<20;i++){const x=(i*41+11)%W,y=(i*29+3)%H;ctx.globalAlpha=.15+(Math.sin(t*1.7+i)*.5+.5)*.23;ctx.fillStyle=i%3===0?C.gold:'#94d6dc';ctx.fillRect(x,y,1,1);}ctx.globalAlpha=1;
ctx.fillStyle='rgba(84,159,157,.08)';for(let i=0;i<5;i++)ctx.fillRect(0,38+i*20,W,1);
}
if(kind==='selector'){ctx.strokeStyle='#23374d';for(let i=0;i<8;i++){ctx.beginPath();ctx.moveTo(i*40-40,0);ctx.lineTo(i*40+100,H);ctx.stroke();}}
}
function character(assets,id){return assets.manifest.characters.find(c=>c.id===id)||assets.manifest.characters[0];}
function fighter(ctx,assets,id,x,y,face='right',size=64,alpha=1,tilt=0){
const c=character(assets,id),scale=size/64;
ctx.save();ctx.translate(Math.round(x),Math.round(y));ctx.scale(face==='left'?-1:1,1);ctx.rotate(tilt);ctx.globalAlpha=alpha;
ctx.drawImage(assets.sprites[c.id],-c.anchor.x*scale,-c.anchor.y*scale,size,size);ctx.restore();
}
function nameplate(ctx,name,x,y,w,hp=1,flip=false){
box(ctx,x,y,w,25,'#122639','#6085a1');
text(ctx,name,flip?x+w-5:x+5,y+4,C.ink,1,flip?'right':'left');
ctx.fillStyle='#030a11';ctx.fillRect(x+4,y+15,w-8,6);ctx.fillStyle='#3d5c4d';ctx.fillRect(x+5,y+16,w-10,4);
ctx.fillStyle=hp>.3?'#b2e87c':'#f29e67';ctx.fillRect(x+5,y+16,Math.round((w-10)*Math.max(0,Math.min(1,hp))),4);
}
function screen(ctx,kind,assets,state={}){
const selected=character(assets,state.selected||'pikachu'),opponent=character(assets,state.opponent||(selected.id==='charmander'?'squirtle':'charmander'));
const t=state.time||0,controls=[],cards=[];
ctx.save();ctx.imageSmoothingEnabled=false;ctx.clearRect(0,0,W,H);
function act(id,label,x,y,w,focus=false){button(ctx,label,x,y,w,18,state.pressed===id?'pressed':state.hover===id||focus?'focused':'normal');controls.push({id,label,x,y,width:w,height:18});}
if(kind==='title'){
backdrop(ctx,kind,assets,t);
title(ctx,'POKEMON',120,7,C.gold,3);text(ctx,'BATTLE ARENA',120,34,'#b5d9e0',1,'center');
const bob=Math.round(Math.sin(t*Math.PI*2/4.8)*1.5);
ctx.drawImage(assets.hero,16,41+bob,208,92);
for(let i=0;i<7;i++){const phase=(t/1.6+i*.23)%1;if(phase<.4){ctx.globalAlpha=1-phase/.4;star(ctx,12+(i*37)%218,48+(i*19)%69,i%2?C.gold:'#a1eff7',1);ctx.globalAlpha=1;}}
act('start','START',76,134,88,true);
if(Math.sin(t*3)>.0)text(ctx,'ENTER / TAP',120,153,'#9fb7c9',1,'center');
}else if(kind==='selector'){
backdrop(ctx,kind,assets,t);text(ctx,'CHOOSE YOUR POKEMON',120,7,C.gold,1,'center');ctx.fillStyle='#526c82';ctx.fillRect(8,20,224,1);
assets.manifest.characters.forEach((c,i)=>{
const x=8+(i%3)*76,y=26+Math.floor(i/3)*54,isSelected=c.id===selected.id;
box(ctx,x,y,72,50,isSelected?'#2b3b45':'#172a3d',isSelected?C.gold:'#3b566c');
if(isSelected){ctx.fillStyle='#645437';ctx.fillRect(x+3,y+3,66,34);text(ctx,'>',x+4,y+4,C.gold);}
ctx.fillStyle=c.accent_color;ctx.fillRect(x+61,y+4,6,3);
ctx.drawImage(assets.sprites[c.id],x+14,y-2,44,44);
text(ctx,c.name,x+36,y+40,isSelected?C.gold:C.ink,1,'center');
cards.push({id:c.id,x,y,width:72,height:50});
});
act('back','BACK',8,136,56);act('fight','FIGHT AS '+selected.name,72,136,160,true);
}else if(kind==='battle'){
ctx.drawImage(assets.arena,0,0,W,H);
nameplate(ctx,selected.name,4,4,94,state.player_hp===undefined?.88:state.player_hp);
nameplate(ctx,opponent.name,142,4,94,state.opponent_hp===undefined?.68:state.opponent_hp,true);
box(ctx,103,4,34,25,'#1c2c3c','#d2c394');text(ctx,String(state.timer===undefined?60:state.timer).padStart(2,'0'),120,9,C.gold,2,'center');
fighter(ctx,assets,selected.id,64,120-Math.round(Math.max(0,Math.sin(t*4))), 'right');
fighter(ctx,assets,opponent.id,176,120-Math.round(Math.max(0,Math.sin(t*4+1))), 'left');
ctx.fillStyle='rgba(7,19,29,.89)';ctx.fillRect(0,141,W,19);text(ctx,'A/D MOVE  W JUMP  J/K ATTACK',120,147,'#b9d4d8',1,'center');
}else if(kind==='victory'||kind==='defeat'){
const win=kind==='victory';backdrop(ctx,kind,assets,t);
if(win){
for(let i=0;i<24;i++){ctx.fillStyle=[C.gold,'#9ddac5','#e8928e','#a3bde9'][i%4];const x=(i*37+13)%W,y=Math.round(((i*23+t*13)%124)+5);ctx.fillRect(x,y,2+(i%2),i%3===0?3:1);}
}else{
ctx.fillStyle='rgba(121,144,184,.16)';for(let i=0;i<12;i++){const x=(i*27+9)%W,y=Math.round((i*19+t*9)%132);ctx.fillRect(x,y,1,3);}
}
title(ctx,win?'VICTORY!':'DEFEAT',120,12,win?C.gold:'#b8c6e2',3);
text(ctx,win?selected.name+' WINS!':'YOU WILL BE BACK!',120,40,win?'#dbeeb3':'#a4b5cb',1,'center');
ctx.fillStyle=win?'#83683b':'#344057';ctx.fillRect(91,123,58,5);ctx.fillStyle=win?'#ad8a43':'#435069';ctx.fillRect(97,121,46,2);
fighter(ctx,assets,selected.id,120,122-Math.round(Math.sin(t*3)*(win?1:0)), 'right',80,win?1:.73,win?0:-.08);
for(let i=0;i<3;i++)star(ctx,108+i*9,51,win?C.gold:'#536078');
act('rematch',win?'REMATCH':'TRY AGAIN',8,136,104,true);act('selector','CHANGE POKEMON',120,136,112);
}else{ctx.restore();throw new Error('Unknown screen: '+kind);}
ctx.restore();return{screen:kind,selected:selected.id,opponent:opponent.id,controls,cards};
}
function transition(current,action,selected='pikachu'){
if(action==='start')return'selector';
if(action==='back'||action==='menu')return'title';
if(action==='fight'||action==='rematch')return'battle';
if(action==='selector')return'selector';
if(action==='victory'||action==='defeat')return action;
return current;
}
const api={width:W,height:H,palette:C,screen,transition,text,button,box,star,character};
if(typeof module==='object'&&module.exports)module.exports=api;
global.PokemonScreens=api;
})(typeof window!=='undefined'?window:globalThis);

