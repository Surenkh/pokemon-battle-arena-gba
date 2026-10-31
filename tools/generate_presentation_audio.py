"""Original tracker title music and restrained menu/round/recall PCM cues."""
from pathlib import Path
import hashlib,json,math,struct,wave
ROOT=Path(__file__).resolve().parents[1];RATE=16384

def chord(notes,duration=.12,level=.42,kind='soft'):
 values=[]
 for n in range(round(duration*RATE)):
  t=n/RATE;x=t/duration;attack=min(1,t/.006);release=min(1,(duration-t)/.025)
  v=0
  for note in notes:
   hz=440*2**((note-69)/12);phase=2*math.pi*hz*t
   v+=math.sin(phase)+(.16*math.sin(phase*2) if kind=='bright' else .05*math.sin(phase*3))
  values.append(level*v/max(1,len(notes))*attack*release*(1-.48*x))
 return values

def sequence(notes,step=.09,level=.42,kind='soft'):
 return [v for note in notes for v in chord(note if isinstance(note,list) else [note],step,level,kind)]

def sweep(start,end,duration,level=.4):
 values=[];phase=0
 for n in range(round(duration*RATE)):
  t=n/RATE;x=t/duration;phase+=2*math.pi*(start+(end-start)*x)/RATE
  env=min(1,t/.008,(duration-t)/.025)
  values.append(level*(math.sin(phase)+.12*math.sin(phase*2))*env*(.8+.2*math.sin(x*math.pi)))
 return values

def save(name,values):
 samples=[round(max(-.65,min(.65,v))*32767) for v in values]
 file=ROOT/'audio'/f'{name}.wav'
 with wave.open(str(file),'wb') as w:
  w.setparams((1,2,RATE,0,'NONE','not compressed'));w.writeframes(struct.pack('<'+'h'*len(samples),*samples))
 (ROOT/'content'/f'sound_{name}.json').write_text(json.dumps(dict(id=name,type='sound',name=name,path=f'audio/{name}.wav',format='wav'),indent=2)+'\n')
 return dict(name=name,samples=len(samples),rate=RATE,peak=max(abs(v) for v in samples),sha256=hashlib.sha256(file.read_bytes()).hexdigest())

cues={
 'ui_move':chord([76],.045,.40), 'ui_confirm':sequence([74,81],.065,.36),
 'ui_back':sequence([72,67],.055,.30), 'ui_pause':chord([62,69],.12,.32),
 'ui_resume':sequence([69,74],.065,.32), 'round_count':chord([62],.085,.32),
 'round_fight':sequence([[62,69],[66,74]],.1,.42,'bright'),
 'round_ko':sequence([[50,57],[45,52]],.13,.40),
 'ball_open':sweep(480,1250,.16,.30), 'ball_absorb':sweep(280,1800,.58,.26),
 'ball_seal':sequence([86,[74,81]],.085,.33,'bright'),
 'result_win':sequence([[62,66],[66,69],[69,73],[74,78,81]],.22,.42,'bright'),
 'result_lose':sequence([[62,66],[59,62],[57,61],[50,57]],.22,.34),
 'result_draw':sequence([[62,69],[64,71],[62,69]],.20,.32),
}
report=[save(name,values) for name,values in cues.items()]

# Four-pattern, three-voice ProTracker module: 16 bars at 112 BPM (~34.29 s).
# Newly composed melody/harmony; no Pokémon recordings or existing song data.
melodies=[
 [62,66,69,71,73,69,66,64,62,66,69,74,73,71,69,66],
 [66,62,59,69,66,64,62,66,69,71,74,73,71,69,66,64],
 [59,62,67,69,71,69,67,64,62,67,69,71,74,71,69,67],
 [61,64,69,71,73,71,69,66,64,69,73,71,69,64,66,64]]
roots=[38,35,31,33];thirds=[4,3,4,4]
samples=[]
for k in range(3):
 vals=[]
 for n in range(64):
  p=2*math.pi*n/64
  v=(2/math.pi*math.asin(math.sin(p))*.70 if k==0 else math.sin(p)*.72 if k==1 else (.7*math.sin(p)+.16*math.sin(p*2))*.58)
  vals.append(round(v*127)&255)
 samples.append(bytes(vals))
mod=bytearray(b'Leaflight Circuit'.ljust(20,b'\0'))
for i in range(31):
 data=samples[i] if i<3 else b'';name=[b'Glass triangle',b'Soft bass',b'Quiet arpeggio'][i] if i<3 else b''
 mod+=name.ljust(22,b'\0')+struct.pack('>HBBHH',len(data)//2,0,[40,28,17][i] if i<3 else 0,0,len(data)//2 if data else 1)
mod+=bytes([4,0])+bytes([0,1,2,3])+bytes(124)+b'M.K.'
def cell(note=0,instrument=0,effect=0,param=0):
 period=round(428*2**((48-note)/12)) if note else 0
 assert 0<=period<4096
 return bytes([(instrument&0xf0)|((period>>8)&15),period&255,((instrument&15)<<4)|(effect&15),param])
for p in range(4):
 rows=[[cell() for _ in range(4)] for _ in range(64)]
 for i,note in enumerate(melodies[p]):rows[i*4][0]=cell(note,1);rows[i*4+3][0]=cell(effect=12,param=0)
 for row in [0,16,32,48]:rows[row][1]=cell(roots[p]+(7 if row%32 else 0),2)
 arp=[roots[p]+24,roots[p]+24+thirds[p],roots[p]+31,roots[p]+36]
 for row in range(0,64,2):rows[row][2]=cell(arp[(row//2)%4],3)
 if p==0:
  rows[0][0]=cell(melodies[0][0],1,15,6);rows[0][3]=cell(effect=15,param=112)
  for channel,pan in enumerate([128,88,168]):rows[1][channel]=cell(effect=8,param=pan)
 mod+=b''.join(c for row in rows for c in row)
mod+=b''.join(samples)
file=ROOT/'audio/title_theme.mod';file.write_bytes(mod)
(ROOT/'content/music_title_theme.json').write_text(json.dumps(dict(id='title_theme',type='music',name='title_theme',path='audio/title_theme.mod',format='mod'),indent=2)+'\n')
(ROOT/'docs/presentation_audio_assets.json').write_text(json.dumps(dict(generator='tools/generate_presentation_audio.py',provenance='Original composition, wavetables and deterministic PCM synthesis; no external recordings',music=dict(name='Leaflight Circuit',file='audio/title_theme.mod',patterns=4,active_channels=3,tempo_bpm=112,nominal_loop_seconds=4*64*6*2.5/112,sha256=hashlib.sha256(mod).hexdigest()),effects=report),indent=2)+'\n')
print('Generated original 34.29-second title module and',len(report),'presentation cues.')
