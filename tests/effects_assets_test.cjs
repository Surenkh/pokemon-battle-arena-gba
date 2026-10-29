const fs=require('fs'),path=require('path'),assert=require('assert'),crypto=require('crypto');
const root=path.resolve(__dirname,'..');
const sizes={shot:[32,32],wave:[32,16],hit:[32,32],cast:[16,16],guard:[32,32]};
const silhouettes=new Set();let frames=0;
for(let ch=0;ch<6;ch++){
 let sharedPalette;
 for(const[kind,[w,h]]of Object.entries(sizes)){
  const name=kind+'_'+ch,b=fs.readFileSync(path.join(root,'graphics',name+'.bmp'));
  assert.equal(b.toString('ascii',0,2),'BM');assert.equal(b.readInt32LE(18),w,name+' width');assert.equal(b.readInt32LE(22),h*4,name+' frames');assert.equal(b.readUInt16LE(28),4,name+' bpp');
  const meta=JSON.parse(fs.readFileSync(path.join(root,'content','sprite_'+name+'.json')));
  assert.equal(meta.width,w);assert.equal(meta.height,h);
  const palette=b.subarray(54,118);if(sharedPalette)assert(palette.equals(sharedPalette),name+' shares fighter palette');else sharedPalette=Buffer.from(palette);
  assert.deepEqual([...palette.subarray(0,3)],[255,0,255],name+' transparent index');
  const off=b.readUInt32LE(10),stride=Math.ceil(w*4/32)*4,hashes=[];
  for(let f=0;f<4;f++){
   const pixels=[];let opaque=0,core=0;
   for(let y=0;y<h;y++)for(let x=0;x<w;x++){
    const byte=b[off+(h*4-1-f*h-y)*stride+(x>>1)],ix=(x%2?byte:byte>>4)&15;
    assert(ix<8,name+' no unexpected palette entries');pixels.push(ix);if(ix)opaque++;
    if(ix&&Math.abs(x-w/2)<=3&&Math.abs(y-h/2)<=3)core++;
   }
   assert(opaque>=8,name+' visible frame '+f);if(kind==='shot'||kind==='wave')assert(core>0,name+' projectile core');
   hashes.push(crypto.createHash('sha256').update(Buffer.from(pixels)).digest('hex'));frames++;
   if(kind==='shot'&&f===0)silhouettes.add(pixels.map(p=>p?1:0).join(''));
  }
  if(kind==='shot'||kind==='wave')assert.equal(new Set(hashes).size,4,name+' four distinct travel frames');
 }
}
assert.equal(silhouettes.size,6,'Normal attacks differ in silhouette, not just color');
console.log(JSON.stringify({status:'passed',effects:30,frames,distinct_normal_silhouettes:silhouettes.size}));
