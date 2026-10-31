"""Validate original presentation PCM, MOD structure, and native art imports."""
from pathlib import Path
import array,hashlib,json,math,struct,wave
r=Path(__file__).resolve().parents[1];j=json.loads((r/'docs/presentation_audio_assets.json').read_text());checks=0
def check(value):
 global checks
 checks+=1
 assert value
for e in j['effects']:
 p=r/'audio'/(e['name']+'.wav');check(hashlib.sha256(p.read_bytes()).hexdigest()==e['sha256'])
 with wave.open(str(p)) as w:
  check(w.getnchannels()==1 and w.getsampwidth()==2 and w.getframerate()==16384)
  data=array.array('h',w.readframes(w.getnframes()));check(len(data)==e['samples'])
 check(max(abs(v) for v in data)<=int(.65*32767)+1)
 check(math.sqrt(sum(v*v for v in data)/len(data))>400)
 check(abs(data[0])<=2 and abs(data[-1])<300)
 q=json.loads((r/'content'/('sound_'+e['name']+'.json')).read_text());check(q['path']=='audio/'+e['name']+'.wav' and q['type']=='sound')
module=(r/'audio/title_theme.mod').read_bytes();check(module[1080:1084]==b'M.K.');check(module[950]==4)
check(len(module)==1084+4*1024+3*64);check(hashlib.sha256(module).hexdigest()==j['music']['sha256'])
# Three looping original instruments; fourth channel carries only tempo commands.
for i in range(31):
 length=struct.unpack_from('>H',module,20+i*30+22)[0];volume=module[20+i*30+25]
 check(length==(32 if i<3 else 0));check(0<=volume<=40)
for pattern in range(4):
 used=set()
 for row in range(64):
  for channel in range(4):
   a,b,c,d=module[1084+pattern*1024+row*16+channel*4:1088+pattern*1024+row*16+channel*4]
   instrument=(a&240)|(c>>4);period=((a&15)<<8)|b
   check(instrument<=3);check(period==0 or 56<=period<=1712)
   if period:used.add(channel)
 check(used=={0,1,2})
for name,width,height,frames in [('recall_ball',32,32,4),('recall_beam',32,64,1),('recall_glint',64,64,4)]:
 raw=(r/'graphics'/(name+'.bmp')).read_bytes();check(raw[:2]==b'BM' and struct.unpack_from('<H',raw,28)[0]==4)
 check(struct.unpack_from('<ii',raw,18)==(width,height*frames))
 item=json.loads((r/'content'/('sprite_'+name+'.json')).read_text());check(item['width']==width and item['height']==height)
print('PASS',checks,'presentation asset checks;',len(j['effects']),'PCM cues, original 3-voice MOD, ball/beam/glint sprite imports.')
