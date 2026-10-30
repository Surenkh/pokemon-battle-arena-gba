"""v0.5 native practice/traits: physical buttons and read-only telemetry."""
from pathlib import Path
import hashlib,json,os,re,subprocess,sys,time

r=Path(__file__).resolve().parents[1];out=r/'tests/native_artifacts/depth';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name,size):return int(re.search(r'\b([0-9a-f]{8})\s+'+str(size)+r'\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug',128);depth=symbol('pkmn_depth_debug',128);feel=symbol('pkmn_feel_debug',64)
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('w'),text=True,bufsize=1)
checks=0;started=time.time();roster=[];guard=[]
def cmd(s):
 p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Native emulator stopped')
 return line
profile=os.environ.get('PKMN_PROFILE_FRAMES')=='1';profile_missed=0
def frames(n,k=0):
 global profile_missed
 if not profile:cmd(f'f {n} {k}');return
 caller=sys._getframe(1).f_lineno
 for _ in range(n):
  cmd(f'f 1 {k}');v=state();d=info()
  if d[20]>4096 or v[22]>profile_missed:
   print('FRAME_PROFILE',dict(caller=caller,keys=k,screen=v[1],character=v[2],tick=v[5],cpu=d[20]/4096,missed=v[22],display=v[23]),flush=True)
  profile_missed=v[22]
def press(k):frames(2,k);frames(2)
def state():return list(map(int,cmd(f's {main:08x}').split()[1:]))
def info():return list(map(int,cmd(f's {depth:08x}').split()[1:]))
def feedback():return list(map(int,cmd(f'v {feel:08x}').split()[1:]))
def signed(n):return n if n<2**31 else n-2**32
def capture(name):cmd('d '+str(out/(name+'.ppm')))
def check(value,message):
 global checks
 checks+=1
 if not value:
  capture('failure');raise AssertionError(message+' state='+str(state())+' depth='+str(info()))
def configure(ch,mode=0,opponent=None):
 check(state()[1]==9,'Practice setup reachable')
 while info()[2]!=0:press(4)
 while state()[2]!=ch:press(16)
 press(4);check(info()[2]==1 and state()[2]==ch,'Options retain selected fighter')
 while info()[3]!=mode:press(16)
 if opponent is not None:
  check(opponent!=ch,'Test does not select a mirror')
  press(4)
  for _ in range(6):
   if info()[4]==opponent:break
   press(16)
  check(info()[4]==opponent,'Choose dummy character')
 while info()[2]!=0:press(4)
def start():
 press(1);frames(185);check(state()[1]==3 and info()[19]==1,'Practice uses active combat core')
def change():
 press(8);check(state()[1]==4,'Practice pause opens');press(128);press(128);press(1);check(state()[1]==9,'Change returns to practice setup')
def approach(gap):
 for _ in range(240):
  v=state()
  if abs(v[13]-v[12])<=gap*256:break
  frames(1,16 if v[13]>v[12] else 32)
 frames(5)
 check(state()[1]==3,'Approach retains active practice')

try:
 frames(90);check(state()[1]==0,'Native title');capture('title');press(2);check(state()[1]==9,'B opens practice')
 for ch,name in enumerate(['pikachu','charmander','squirtle','bulbasaur','eevee','meowth']):
  configure(ch,0,(ch+1)%6);capture('setup_'+name)
  press(256);check(state()[1]==10,'R opens fighter guide');capture('guide_'+name);press(2);check(state()[1]==9 and state()[2]==ch,'Guide returns without changing selection')
  start();v=state();check(v[10]==v[11]==100,'Practice starts with full SP');frames(100);check(state()[18]==3600,'Practice timeout stays frozen');check(state()[6:12]==v[6:12],'Idle dummy does not attack or spend resources')
  approach(34 if ch==3 else 22);before=state();frames(1,1);frames(1);after=state()
  if ch==0:check(after[12]>before[12] and signed(info()[21])>0,'Quick Step visibly advances melee startup')
  confirmed=False
  for _ in range(35):
   if signed(info()[10])>=0:confirmed=True;break
   frames(1)
  check(confirmed,'Real melee hit confirmed: '+name)
  for _ in range(35):
   if feedback()[14] in (7,8):break
   frames(1)
  check(feedback()[14] in (7,8),'Follow-up cue is available')
  old_sta=state()[8];frames(1,2);frames(1);check(state()[16]==3 and info()[8]==1,'A-to-B follow-up entered');check(state()[8]==old_sta-1200,'Follow-up pays full fire stamina cost')
  frames(75);d=info();check(d[6]==2 and d[7]==2 and d[14]>=1,'Only damaging follow-up counts as a two-hit sequence');capture('follow_'+name)
  check(d[5]==[12,14,11,12,10,11][ch],'Feedback displays actual projectile damage')
  press(4);check(state()[1]==2,'Select resets practice into countdown');frames(185);check(info()[5:8]==[0,0,0],'Manual reset clears practice statistics');check(state()[10]==100 and state()[8]==[105,90,110,100,115,100][ch]*60,'Manual reset restores real resources')
  press(8);frozen=state();d=info();frames(60);check(state()[5:21]==frozen[5:21],'Pause freezes practice simulation');check(info()[5:20]==d[5:20],'Pause freezes practice statistics');check(feedback()[4]==0,'Practice pause stops combat audio');press(8);frames(65);check(state()[1]==3,'Practice resumes after countdown')
  check(state()[22]==0,'No missed frames in practice roster flow');roster.append(dict(character=name,confirmed_followup=True,guide=True,reset=True,pause=True,peak_cpu_fraction=state()[21]/4096))
  change()

 # Block-only dummy exposes the real pressure/guard arithmetic in the ROM.
 for player,dummy,expected in [(1,0,23),(0,2,12),(1,2,18)]:
  configure(player,1,dummy);start();check(state()[17]==1,'Blocking dummy holds ordinary guard');frames(1,2);frames(1);observed=None
  for _ in range(90):
   before=state();frames(1);after=state();delta=before[9]-after[9]
   if delta>60:observed=delta;break
  check(observed==expected*60+12,'Native guard pressure and efficiency match the combat rule');check(state()[7]==[95,100,115,120,100,90][dummy],'Affordable block prevents HP damage');capture(f'guard_{player}_{dummy}');guard.append(dict(player=player,dummy=dummy,guard_points=expected));change()

 # Eevee also links A-to-A, with one follow-up and no held-button repetition.
 configure(4,0,1);start();approach(16);frames(1,1);frames(1)
 for _ in range(50):
  if feedback()[14]==8:break
  frames(1)
 check(feedback()[14]==8,'Eevee melee follow-up window');frames(1,1);frames(1);check(info()[8]==1,'Eevee A-to-A enters bounded follow-up');frames(40,1);check(info()[6]==2 and info()[7]==2,'Eevee two damaging melee hits');check(state()[16]==0,'Held A does not repeat attacks');capture('eevee_melee_chain');change()

 # Attacking dummy uses ordinary supers. Jump at the incoming travel window.
 configure(0,2,1);start();check(state()[17]==5,'Attacking dummy starts a real super');capture('attacking_dummy_charge')
 while state()[5]<39:frames(1)
 frames(1,64);frames(25);check(state()[6]==95,'A correctly timed practice jump evades the super');change()

 # Block a normal attack, then punish its recovery with Meowth's bonus.
 configure(5,2,1);start()
 while state()[5]<39:frames(1)
 frames(1,64);frames(35)
 for _ in range(400):
  v=state();d=info();distance=abs(v[13]-v[12])//256
  recovery=v[17]==2 and v[5]-d[28]>=9 and d[26]>v[5]+5
  available=v[16] in (0,1) or (v[16]==7 and d[25]<=v[5]+2)
  frames(1,1 if recovery and available and distance<28 else 512)
  if info()[15]:break
 check(info()[15]>=1 and info()[5]==11,'Meowth recovery punish deals 11 melee damage');frames(4);capture('meowth_punish');change()

 # Repeated actual KOs automatically restart training and retain best sequence.
 configure(0,0,1);start();approach(22);frames(1,1);frames(1)
 for _ in range(50):
  if feedback()[14] in (7,8):break
  frames(1)
 frames(1,2);frames(80);check(info()[7]==2,'Best sequence established before KO replay')
 line=cmd(f'b 12000 {main:08x} {feel:08x}');check(info()[7]==2,'Auto KO reset retains best sequence');check(info()[31]>=3,'KO automatically restarts practice rounds');check(state()[1] in (2,3,5),'Practice never routes to ordinary result screen');check(state()[22]==0,'No missed display frames in long practice replay');capture('long_practice')
 if state()[1]==2:frames(185)
 if state()[1]==5:frames(245)
 change();press(2);check(state()[1]==0,'Leave practice to title');press(8);press(1);frames(185);check(state()[1]==3 and info()[19]==0,'Ordinary mode restores timed rules');check(state()[10]<100 and state()[18]<3600,'Ordinary match has earned SP and a running clock');check(state()[22]==0,'No missed frames in mode transitions')
 result=dict(status='passed',checks=checks,roster=roster,guard=guard,practice_auto_rounds=info()[31],automatic_restarts=max(0,info()[31]-1),practice_stress_display_frames=12000,peak_cpu_fraction=state()[21]/4096,missed_display_frames=state()[22],wall_seconds=round(time.time()-started,2),rom_sha256=hashlib.sha256((r/'out/PokemonBattleArena.gba').read_bytes()).hexdigest(),method='mGBA 0.10.2; physical buttons, read-only telemetry; no gameplay-memory writes')
 (out/'report.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
finally:
 if p.poll() is None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
