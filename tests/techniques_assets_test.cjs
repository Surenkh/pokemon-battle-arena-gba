const assert=require('assert'),fs=require('fs'),sharp=require('sharp');
(async()=>{
 const imported=JSON.parse(fs.readFileSync('content/sprite_shadow_burst.json'));
 assert.equal(imported.width,32);assert.equal(imported.height,32);
 const bmp=fs.readFileSync('graphics/shadow_burst.bmp');assert.equal(bmp.readUInt16LE(28),4);
 const image=sharp('docs/shadow_burst.png');const meta=await image.metadata();assert.equal(meta.width,32);assert.equal(meta.height,256);
 const phases=[];
 for(let frame=0;frame<8;frame++){
  const data=await sharp('docs/shadow_burst.png').extract({left:0,top:frame*32,width:32,height:32}).ensureAlpha().raw().toBuffer();
  let visible=0;
  for(let y=0;y<32;y++)for(let x=0;x<32;x++)if(data[(y*32+x)*4+3]>127){
   visible++;assert(x>=2&&x<=30&&y>=6&&y<=26,'Warning and blast stay inside their advertised footprint');
  }
  assert(visible>40,'Every phase is readable at native resolution');
  if(frame<4)assert(visible<230,'Warning remains an outline');else assert(visible>350,'Active blast is visibly filled');
  phases.push(data.toString('base64'));
 }
 assert.equal(new Set(phases).size,8,'All eight warning/blast phases are distinct');
 console.log('PASS technique art: eight distinct native phases, outlined warning, filled blast and bounded footprint.');
})();
