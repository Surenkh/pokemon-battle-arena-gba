"""Exercise the compiled ROM through button input and read-only telemetry."""
from pathlib import Path
import subprocess,os,re,json
r=Path(__file__).resolve().parents[1];out=r/'tests/native_artifacts/animation';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
address=int(re.search(r'\b([0-9a-f]{8})\s+128\s+OBJECT\s+GLOBAL[^\n]+pkmn_debug',symbols).group(1),16)
m=json.loads((r/'assets/source/animations/manifest.json').read_text())
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=open(out/'errors.log','w'),text=True,bufsize=1)
def command(s):
 p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Emulator stopped')
 return line
def frames(n,k=0):command(f'f {n} {k}')
def state():return list(map(int,command(f's {address:08x}').split()[1:]))
def press(k):frames(2,k);frames(2)
def capture(n):command('d '+str(out/(n+'.ppm')))
checks=0
def check(ok,msg):
 global checks
 checks+=1
 if not ok:raise AssertionError(msg+' '+str(state()))
report=[];frames(90);press(8);selected=0
for ch,name in enumerate(['pikachu','charmander','squirtle','bulbasaur','eevee','meowth']):
 while selected!=ch:press(16);selected=(selected+1)%6
 press(1)
 while state()[1]==2:frames(1)
 walk=set();hop=set();clips=set();video=0
 for i in range(24):
  frames(1,32);v=state();clips.add(v[27])
  if v[27]==1:walk.add(v[25])
  if ch==0 and i%2==0:capture(f'preview_{video:03}');video+=1
  check(0<=v[25]<m['characters'][name]['frame_count'],'Valid native walk frame')
 check(len(walk)==m['characters'][name]['clips']['Walk']['count'],'All walk frames rendered')
 frames(4);frames(1,64)
 for i in range(38):
  v=state();clips.add(v[27])
  if v[27]==4:hop.add(v[25])
  if ch==0 and i%2==0:capture(f'preview_{video:03}');video+=1
  frames(1)
 check(len(hop)>=2,'Separate rising/falling poses')
 for i in range(180):
  keys=16 if i<28 else 1 if i==28 else 2 if i in (55,110) else 128 if i==85 else 0
  frames(1,keys);v=state();clips.add(v[27]);check(0<=v[25]<m['characters'][name]['frame_count'],'Valid combat frame')
  if ch==0 and i%2==0:capture(f'preview_{video:03}');video+=1
  if v[1]!=3:break
 check(state()[1]==3,'Battle ongoing');press(8);before=state();frames(60);after=state();check(before[5:21]==after[5:21],'Pause freezes core');check(before[25:29]==after[25:29],'Pause freezes poses')
 press(128);press(128);press(1);check(state()[1]==1,'Selector returns')
 report.append({'character':name,'native_walk_frames':sorted(walk),'native_jump_frames':sorted(hop),'observed_clip_ids':sorted(clips),'peak_cpu_q12':after[21],'missed_frames':after[22]})
p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
result={'status':'passed','checks':checks,'characters':report,'method':'Native ROM, physical button input and read-only telemetry; no game memory writes.'};(out/'report.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
