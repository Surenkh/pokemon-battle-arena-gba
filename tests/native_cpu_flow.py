"""v0.6 CPU integration: physical inputs and read-only native diagnostics."""
from pathlib import Path
import hashlib,json,os,re,subprocess,time
root=Path(__file__).resolve().parents[1];out=root/'tests/native_artifacts/cpu';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(root/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name):return int(re.search(r'\b([0-9a-f]{8})\s+128\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug');cpu=symbol('pkmn_cpu_debug');feel=int(re.search(r'\b([0-9a-f]{8})\s+64\s+OBJECT\s+GLOBAL[^\n]+pkmn_feel_debug',symbols).group(1),16)
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(root/'tests/mgba_driver')),str(root/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('w'),text=True,bufsize=1)
checks=0;started=time.time();rounds=[];seen=[0,0,0];intent_mask=0;plan_mask=0;capture_index=0

def cmd(value):
 p.stdin.write(value+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Native emulator stopped')
 return line
def frames(n,k=0):cmd(f'f {n} {k}')
def press(k):frames(2,k);frames(2)
def read(address):return list(map(int,cmd(f's {address:08x}').split()[1:]))
def state():return read(main)
def info():return read(cpu)
def check(ok,message):
 global checks
 checks+=1
 if not ok:raise AssertionError(message+' state='+str(state())+' cpu='+str(info()))
def change():
 for _ in range(24):
  if state()[1]!=5:break
  frames(10)
 if state()[1]==6:press(16);press(1);check(state()[1]==1,'Result returns to selector');return
 press(8);check(state()[1]==4,'Pause opens');before=info();frames(30);check(info()==before,'Pause freezes the complete CPU policy');press(128);press(128);press(1);check(state()[1]==1,'Return to selector')
try:
 frames(90);press(8)
 for level in range(3):
  if state()[2]<3:press(128)
  press(128)
  while state()[4]!=level:press(16)
  press(64)
  attempts=0
  while seen[level]!=63 and attempts<50:
   attempts+=1
   target=(attempts+level)%6
   while state()[2]!=target:press(16)
   press(1);frames(185);v=state();rival=v[3]
   if seen[level]&(1<<rival):change();continue
   check(v[1]==3,'Native battle starts')
   check(info()[0]==0x43505536,'CPU diagnostics present in ROM')
   check(info()[14]==[20,12,8][level] and info()[15]==[9,6,4][level],'Actual difficulty profile')
   # Initial spacing alone cannot exercise resource recovery. Observe a sustained
   # bout, including defensive exchanges and the end of a stamina cycle.
   for step in range(720):
    v=state()
    if v[1]!=3:break
    gap=abs(v[13]-v[12])//256
    keys=0
    if step%48==0:keys=2
    elif step%18==0:keys=1
    elif step%90==45:keys=64
    elif gap>36:keys=16 if v[13]>v[12] else 32
    elif step%60<12:keys=512
    frames(1,keys)
    if step%8==0:
     v=state();d=info();intent_mask|=1<<d[1];plan_mask|=1<<d[12]
     check(d[4]+d[5]+d[6]<=6,'Memory is bounded in native ROM')
     check(18<=d[2]<=94,'Fighter-aware target distance is bounded')
     check(d[3]<=max(0,v[5]-[20,12,8][level]),'Native observations respect reaction delay')
    if level==2 and rival==0 and step<180 and step%6==0:
     cmd('d '+str(out/f'preview_{capture_index:03}.ppm'));capture_index+=1
   d=info();check(d[7]>0,'CPU made decisions');check(d[4]+d[5]+d[6]>0,'CPU observed actual player attacks')
   change();press(1);frames(185) # Same selection, a fresh fair match for completion stress.
   result=cmd(f'b 7000 {main:08x} {feel:08x}');v=state();d=info()
   check(v[1]==6 and v[20] in (1,2,3),'Complete native round reaches result')
   check(v[22]==0,'No missed frames with new CPU policy')
   rounds.append(dict(level=level,observed_rival=rival,stress_rival=v[3],player=v[2],outcome=v[20],active_ticks=v[5],decisions=d[7],defense_requests=d[8],punish_requests=d[9],followup_requests=d[10],hesitations=d[11],peak_cpu_fraction=v[21]/4096,missed_display_frames=v[22]))
   seen[level]|=1<<rival
   press(16);press(1);check(state()[1]==1,'Result change returns to selector')
   print('Native CPU coverage',level,bin(seen[level]),flush=True)
  check(seen[level]==63,'All six native CPU styles exercised at this difficulty')
 check(intent_mask&(1<<0) and intent_mask&(1<<2) and intent_mask&(1<<3),'Approach, pressure and recovery observed')
 check(plan_mask==7,'All three bounded plans observed')
 report=dict(status='passed',checks=checks,rounds=rounds,observed_cpu_masks=seen,intent_mask=intent_mask,plan_mask=plan_mask,peak_cpu_fraction=state()[21]/4096,missed_display_frames=state()[22],wall_seconds=round(time.time()-started,2),rom_sha256=hashlib.sha256((root/'out/PokemonBattleArena.gba').read_bytes()).hexdigest(),method='mGBA 0.10.2; all six CPU styles at all three levels through physical buttons and read-only telemetry. No game-memory writes.')
 (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='rounds'},indent=2))
finally:
 if p.poll() is None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
