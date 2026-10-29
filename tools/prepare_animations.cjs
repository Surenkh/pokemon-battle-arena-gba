// Existing credited animation artwork -> native GBA cells. No synthesized poses.
const fs=require('fs'),path=require('path'),sharp=require('sharp');
const root=path.resolve(__dirname,'..'),dir=path.join(root,'assets/source/animations');
const source=JSON.parse(fs.readFileSync(path.join(dir,'source_manifest.json')));
const names=['pikachu','charmander','squirtle','bulbasaur','eevee','meowth'];
const clips=['Idle','Walk','Attack','Shoot','Hop','Charge','Guard','Hurt','Squeeze','Land','Victory','Faint','Down'];
const mapping={Guard:'Cringe',Squeeze:'Sleep',Land:'Sit',Victory:'Pose',Down:'Laying'};
const clear={r:0,g:0,b:0,alpha:0};
const header=['#pragma once','#include <stdint.h>','namespace pkmn {','enum Clip { '+clips.map(n=>'CLIP_'+n.toUpperCase()).join(', ')+', CLIP_COUNT };','struct AnimationClip { int first,count,hit,back; uint8_t duration[16]; };'];
const manifest={version:1,cell:[64,64],anchor:[32,58],source_revision:fs.readFileSync(path.join(dir,'UPSTREAM_REVISION'),'utf8').trim(),characters:{}};
function bounds(data,w,h){let l=w,t=h,r=0,b=0;for(let y=0;y<h;y++)for(let x=0;x<w;x++)if(data[(y*w+x)*4+3]>127){l=Math.min(l,x);t=Math.min(t,y);r=Math.max(r,x+1);b=Math.max(b,y+1);}if(r<=l||b<=t)throw Error('Empty frame');return{left:l,top:t,width:r-l,height:b-t};}
(async()=>{let overview=[];
 for(const [ch,name]of names.entries()){
  const frames=[],defs=[],info={clips:{},frame_count:0};
  for(const clip of clips){let src=mapping[clip]||clip;if(clip==='Squeeze'&&name==='squirtle')src='Withdraw';const a=source[name][src],file=path.join(dir,name,a.source+'-Anim.png');const meta=await sharp(file).metadata(),rows=meta.height/a.height,row=rows===8?2:0;if(rows!==1&&rows!==8)throw Error('Invalid direction count');const first=frames.length;
   for(let frame=0;frame<a.durations.length;frame++){
    const region={left:frame*a.width,top:row*a.height,width:a.width,height:a.height};
    const rgba=await sharp(file).extract(region).ensureAlpha().raw().toBuffer(),bb=bounds(rgba,a.width,a.height);
    const offsets=await sharp(path.join(dir,name,a.source+'-Offsets.png')).extract(region).ensureAlpha().raw().toBuffer();let centerX=a.width/2;
    for(let i=0;i<offsets.length;i+=4)if(offsets[i+3]===255&&offsets[i+1]===255){centerX=(i/4)%a.width;break;}
    const scale=Math.min(2,60/bb.width,54/bb.height),w=Math.max(1,Math.round(bb.width*scale)),h=clip==='Squeeze'?10:Math.max(1,Math.round(bb.height*scale));const left=Math.max(2,Math.min(62-w,32-Math.round((centerX-bb.left)*scale)));
    const tile=await sharp(rgba,{raw:{width:a.width,height:a.height,channels:4}}).extract(bb).resize(w,h,{kernel:'nearest'}).png().toBuffer();
    frames.push(await sharp({create:{width:64,height:64,channels:4,background:clear}}).composite([{input:tile,left,top:58-h}]).png().toBuffer());
   }
   const hit=Math.min(a.hit,a.durations.length-1),back=Math.min(Math.max(a.return,hit+1),a.durations.length);defs.push(`{${first},${a.durations.length},${hit},${back},{${a.durations.join(',')}}}`);info.clips[clip]={first,count:a.durations.length,hit,back,durations:a.durations,source:a.source,direction:row};
  }
  info.frame_count=frames.length;manifest.characters[name]=info;
  await sharp({create:{width:64,height:64*frames.length,channels:4,background:clear}}).composite(frames.map((input,i)=>({input,left:0,top:i*64}))).png().toFile(path.join(dir,'packed_'+name+'.png'));
  header.push(`inline constexpr AnimationClip animation_${name}[CLIP_COUNT] = {${defs.join(',\n')}};`);for(let i=0;i<frames.length;i++)overview.push({input:frames[i],left:i*64,top:ch*64});console.log(name,frames.length,'frames');
 }
 header.push('inline constexpr const AnimationClip* animations[6] = {'+names.map(n=>'animation_'+n).join(',')+'};','}');fs.writeFileSync(path.join(root,'include/pkmn_animation_data.h'),header.join('\n')+'\n');fs.writeFileSync(path.join(dir,'manifest.json'),JSON.stringify(manifest,null,2)+'\n');
 const max=Math.max(...Object.values(manifest.characters).map(c=>c.frame_count));await sharp({create:{width:max*64,height:384,channels:4,background:'#203047'}}).composite(overview).png().toFile(path.join(root,'docs/animation_atlas.png'));
})().catch(e=>{console.error(e);process.exit(1);});
