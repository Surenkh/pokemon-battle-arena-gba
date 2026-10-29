from pathlib import Path
import subprocess,os,re,json,time
PROJ=Path(__file__).resolve().parents[1];ROOT=PROJ;WORK=PROJ/'tests/native_artifacts';WORK.mkdir(parents=True,exist_ok=True)
elf=PROJ/'.gbastudio/PokemonBattleArena.elf'
symbols=subprocess.check_output(['readelf','-sW',str(elf)],text=True)
address=int(re.search(r'\b([0-9a-f]{8})\s+128\s+OBJECT\s+GLOBAL[^\n]+pkmn_debug',symbols).group(1),16)
env=dict(os.environ,LD_LIBRARY_PATH=os.environ.get('LD_LIBRARY_PATH',''))
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(PROJ/'tests/mgba_driver')),str(PROJ/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=open(WORK/'emulator_errors.log','w'),text=True,bufsize=1,env=env)
log=[];checks=0
def cmd(s):
 p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Emulator exited: '+str(p.poll()))
 log.append(line);return line
def frames(n,k=0):return cmd(f'f {n} {k}')
def press(k):frames(2,k);frames(2)
def state():return list(map(int,cmd(f's {address:08x}').split()[1:]))
def check(x,message):
 global checks
 checks+=1
 if not x:raise AssertionError(message+' '+str(state()))
def dump(name):cmd('d '+str(WORK/(name+'.ppm')))
def native_action(button,expected):
 for attempt in range(12):
  for wait in range(80):
   v=state()
   if v[16] in [0,1] and v[14]>=120*256:break
   frames(2)
  if button==512:frames(4,button)
  else:press(button)
  if state()[16] in expected:return
  frames(18)
 check(False,'Native action '+str(button))
def pause_select():
 press(8);check(state()[1]==4,'Pause opens');press(128);press(128);press(1);check(state()[1]==1,'Change returns to selector')
t=time.time();frames(90);check(state()[1]==0,'Title boot');dump('title_final');frames(24);dump('title_animated');press(4);check(state()[1]==7,'Controls');dump('controls');press(2);press(8);check(state()[1]==1,'Start opens selector')
selected=0;roster=[]
for ch in range(6):
 while selected!=ch:press(16);selected=(selected+1)%6
 check(state()[2]==ch,'Selector character');dump('select_'+str(ch));press(1);v=state();check(v[1]==2,'Countdown');check(v[6]==[95,100,115,120,100,90][ch],'Character life');check(v[8]==[105,90,110,100,115,100][ch]*60,'Character stamina');check(v[10]==0,'Meter reset');frames(185);check(state()[1]==3,'Battle starts');press(64);check(state()[14]<120*256,'Jump moves native fighter');frames(40);native_action(128,[4]);dump('squeeze_'+str(ch));frames(38);native_action(2,[3]);frames(10);dump('fire_'+str(ch));frames(30);native_action(512,[1,7]);frames(1);press(8);before=state();check(before[1]==4,'Pause state');frames(120);after=state();check(before[5:21]==after[5:21],'Pause freezes combat');dump('pause');press(8);check(state()[1]==8,'Resume countdown');frames(65);check(state()[1]==3,'Resume fight');dump('battle_'+str(ch));roster.append({'id':ch,'cpu':state()[3],'controls':'passed'});pause_select()
# Difficulty row is reachable, changes the actual stored setting, and returns.
press(128);press(128);press(16);check(state()[4]==2,'Difficulty Hard');press(32);check(state()[4]==1,'Difficulty Normal');press(32);check(state()[4]==0,'Difficulty Easy');press(16);check(state()[4]==1,'Difficulty restore');press(64)
# Complete real native rounds with an external input driver. No game state writes.
press(1);frames(185);matches=[];total_auto_frames=0
while len(matches)<20 or total_auto_frames<36000:
 output=cmd(f'a 6000 {address:08x}');n=int(re.search(r'frames=(\d+)',output).group(1));total_auto_frames+=n;v=state();check(v[1]==6,'A complete round reaches results');check(v[20] in (1,2,3),'Valid actual outcome');matches.append({'round':len(matches)+1,'outcome':v[20],'hp':[v[6],v[7]],'supers':int(re.search(r'supers=(\d+)',output).group(1)),'frames':n,'peak_q12':v[21],'missed_frames':v[22]})
 if len(matches)==1:dump('result_final')
 if len(matches)%5==0:print('Native matches',len(matches),'battle frames',total_auto_frames,flush=True)
 old=v[2:4];press(1);v=state();check(v[1]==2,'Rematch countdown');check(v[2:4]==old,'Rematch preserves characters');check(v[10]==v[11]==0,'Rematch clears both SP meters');check(v[8]==[105,90,110,100,115,100][v[2]]*60,'Rematch fills stamina');frames(185)
# Force no inputs rather than cheating HP: CPU must be able to defeat idle human.
frames(5000);v=state();check(v[1]==6 and v[20]==2,'Idle player naturally loses');dump('defeat_final');press(16);press(1);check(state()[1]==1,'Result Change returns selector')
cmd('q') if False else None
p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
report={'status':'passed','checks':checks,'emulator':'mGBA 0.10.2 libmgba software renderer','native_roster':roster,'native_rounds':matches,'battle_frames':total_auto_frames,'nominal_battle_minutes':round(total_auto_frames/3600,2),'observed_super_activations':sum(x['supers'] for x in matches),'peak_cpu_fraction':max(x['peak_q12'] for x in matches)/4096,'missed_display_frames':max(x['missed_frames'] for x in matches),'wall_seconds':round(time.time()-t,2),'method':'Native ROM, physical button inputs and read-only telemetry; no gameplay memory writes.'}
(WORK/'native_test_report.json').write_text(json.dumps(report,indent=2));(WORK/'native_commands.log').write_text('\n'.join(log));print(json.dumps({k:v for k,v in report.items() if k not in ['native_rounds','native_roster']},indent=2))
