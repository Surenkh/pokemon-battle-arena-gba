// Original, deterministic pixel scenery. All stages share a flat combat floor.
// Authored in code so clean builds reproduce the exact indexed native assets.
function paintArena(canvas,kind){
 const c=canvas(256,256),g=c.getContext('2d');
 const rect=(x,y,w,h,color)=>{g.fillStyle=color;g.fillRect(x,y,w,h);};
 const polygon=(points,color)=>{g.fillStyle=color;g.beginPath();points.forEach(([x,y],i)=>i?g.lineTo(x,y):g.moveTo(x,y));g.closePath();g.fill();};
 rect(0,0,256,256,'#101d32');
 if(kind==='frostpeak'){
  rect(0,42,240,72,'#537996');rect(0,42,240,22,'#3b587d');rect(187,49,14,13,'#e5edc7');
  for(const [x,y,h] of [[-20,106,36],[36,109,53],[95,107,42],[160,108,48],[212,112,41]]){
   polygon([[x,y],[x+h,y-h],[x+h*2,y]],'#8aaebf');polygon([[x+h,y-h],[x+h+13,y-h+20],[x+h+6,y-h+16],[x+h-4,y-h+24],[x+h-13,y-h+20]],'#eff7ec');
  }
  for(const [x,y,h] of [[9,114,29],[41,115,23],[199,114,31],[226,115,26]]){rect(x+5,y-5,3,10,'#294953');polygon([[x-4,y],[x+6,y-h],[x+16,y]],'#325663');polygon([[x,y-11],[x+6,y-h],[x+12,y-11]],'#adcbd0');}
  for(const [x,y]of[[9,49],[44,45],[71,67],[115,55],[171,63],[213,48],[230,73]])rect(x,y,3,1,'#dfefea');
  rect(0,112,240,5,'#e9f5e9');rect(0,117,240,3,'#b5d6d8');rect(0,120,240,23,'#658da5');
  for(let x=0;x<240;x+=24){rect(x+3,122,1,8,'#466c88');rect(x+3,129,12,1,'#466c88');rect(x+15,130,1,12,'#466c88');rect(x+7,134,6,1,'#b5d6d8');}
 }else if(kind==='ruins'){
  rect(0,42,240,74,'#234047');rect(0,42,240,22,'#263751');rect(191,48,15,15,'#cad2a2');rect(197,45,14,16,'#263751');
  polygon([[0,96],[34,81],[61,92],[102,75],[144,94],[184,82],[240,92],[240,117],[0,117]],'#2c4b4b');
  for(const [x,top,h]of[[20,66,47],[92,57,56],[151,64,49],[222,59,54]]){rect(x,top,8,h,'#536c63');rect(x+2,top,2,h,'#6c8471');rect(x-3,top-3,14,5,'#83937a');rect(x-2,top+h-3,12,4,'#7a8a70');rect(x+5,top+7,1,12,'#344e4d');}
  for(const x of[58,179]){rect(x,83,3,21,'#655d48');polygon([[x-2,84],[x+1,72],[x+6,84]],'#c88646');polygon([[x,84],[x+2,77],[x+4,84]],'#ffe59a');}
  rect(0,112,240,5,'#65745e');rect(0,117,240,3,'#b1b493');rect(0,120,240,23,'#516459');
  for(let x=0;x<240;x+=24){rect(x,120,1,23,'#334b48');rect(x+3,123,16,1,'#7e8d70');rect(x+3,133,18,1,'#334b48');rect(x+13,134,1,9,'#334b48');}
  for(const x of[8,37,215,234]){rect(x,110,1,8,'#7f9564');rect(x+1,111,3,2,'#7f9564');}
 }else if(kind==='volcanic'){
  const sky=['#30243f','#49304c','#674254','#8d5655','#b36d58'];
  sky.forEach((color,i)=>rect(0,42+i*12,240,12,color));
  rect(188,52,18,18,'#e5a06a');rect(185,57,24,8,'#e5a06a');
  polygon([[0,82],[24,65],[42,79],[67,60],[88,82],[112,67],[149,87],[182,67],[208,79],[240,61],[240,112],[0,112]],'#49384b');
  polygon([[54,105],[123,55],[138,57],[194,104]],'#332e3d');
  polygon([[95,90],[123,55],[138,57],[160,82],[136,73],[128,62],[117,76]],'#61505a');
  polygon([[118,61],[123,55],[138,57],[145,64],[136,67],[125,65]],'#ffcc70');
  polygon([[127,65],[137,65],[139,76],[151,83],[151,89],[171,101],[161,103],[143,91],[141,84],[131,78]],'#ed9254');
  rect(130,65,4,9,'#ffe4a0');rect(137,76,3,6,'#ffc064');rect(144,84,4,6,'#ffc064');
  // Stacked smoke pixels are moved by a bounded scanline offset at runtime.
  for(let i=0;i<6;i++){const x=124+(i%3)*3,y=52-i*5;rect(x,y,12+i*2,6,i%2?'#4b3e54':'#59435a');}
  rect(0,99,240,14,'#d67545');
  for(let y=100;y<112;y+=3)for(let x=(y%2)*7;x<240;x+=21){rect(x,y,12,1,'#efb365');rect(x+5,y+1,5,1,'#ffcf79');}
  polygon([[0,111],[17,101],[33,103],[42,110],[66,106],[88,112],[127,109],[161,111],[183,104],[207,108],[222,101],[240,105],[240,144],[0,144]],'#302c36');
  rect(0,113,240,4,'#6d4d48');rect(0,117,240,3,'#b87a50');rect(0,120,240,23,'#392c32');
  for(let x=0;x<240;x+=24){rect(x+2,114,15,1,'#a66d51');rect(x+3,122,1,7,'#211f2a');rect(x+4,128,11,1,'#211f2a');rect(x+16,132,1,9,'#211f2a');rect(x+8,136,5,1,'#765044');}
  for(const [x,y] of [[10,89],[38,96],[80,92],[175,84],[204,94],[229,86]]){rect(x,y,2,2,'#efbd70');rect(x+1,y-2,1,1,'#f9dd98');}
 }else{
  const sky=['#426d93','#608ead','#87afbf','#b4d1ce'];
  sky.forEach((color,i)=>rect(0,42+i*11,240,11,color));
  rect(31,50,16,16,'#fae6ae');rect(28,55,22,6,'#fae6ae');
  for(const [x,y,w] of [[68,50,24],[158,55,32],[205,45,24]]){rect(x,y,w,3,'#cee0d7');rect(x+5,y-3,w-11,3,'#cee0d7');}
  polygon([[0,81],[17,73],[28,66],[43,76],[63,82],[86,80],[103,69],[116,73],[135,82],[170,80],[188,67],[200,72],[218,79],[240,72],[240,94],[0,94]],'#517b81');
  polygon([[0,87],[24,79],[51,85],[78,82],[98,86],[128,81],[150,86],[181,79],[205,84],[240,81],[240,99],[0,99]],'#37696e');
  rect(0,85,240,28,'#357d8a');rect(0,96,240,17,'#338e92');
  for(let y=87;y<111;y+=3)for(let x=(y%4)*9;x<240;x+=29){rect(x,y,14,1,y%2?'#6db6ae':'#8cc7ba');rect(x+6,y+1,5,1,'#469ca0');}
  rect(0,109,240,6,'#c4b58d');rect(0,115,240,5,'#dacc9d');rect(0,120,240,23,'#7d806f');
  for(let x=0;x<240;x+=24){rect(x,120,1,23,'#525f60');rect(x+2,120,19,2,'#a7aa8c');rect(x+3,130,18,1,'#5e6c66');rect(x+12,131,1,12,'#5e6c66');rect(x+4,137,5,1,'#a0a58a');}
  for(const x of [3,12,223,234]){rect(x,99,2,13,'#4d7755');rect(x+2,103,2,8,'#769c67');rect(x-2,106,2,5,'#a0ad70');}
  for(const [x,y] of [[70,62],[77,63],[151,48],[158,49]])rect(x,y,3,1,'#365c75');
 }
 return c;
}
function masteryBadge(canvas,save){
 const c=canvas(8,32),g=c.getContext('2d');
 const colors=[['#3a5364','#607485'],['#aa693e','#edb86e'],['#829eae','#d3e6e4'],['#bd8b36','#ffe08a']];
 const shape=['0011100','0111110','1111111','1111111','0111110','0011100'];
 for(let n=0;n<4;n++){
  g.fillStyle=colors[n][0];g.fillRect(2,n*8+5,2,3);g.fillRect(5,n*8+5,2,3);
  for(let y=0;y<6;y++)for(let x=0;x<7;x++)if(shape[y][x]==='1'){g.fillStyle=y<2?colors[n][1]:colors[n][0];g.fillRect(x+1,n*8+y,1,1);}
  g.fillStyle=colors[n][1];for(let i=0;i<n;i++)g.fillRect(2+i*2,n*8+3,1,1);
 }
 save('mastery_badge',c,'sprite',8,8);
}
module.exports={paintArena,masteryBadge};
