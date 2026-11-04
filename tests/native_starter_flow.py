"""Play the starter lessons and both Versus sides through actual mGBA/Lua inputs."""
from pathlib import Path
from PIL import Image
import hashlib,json,os,re,shutil,subprocess,time
R=Path(__file__).resolve().parents[1];OUT=R/'tests/native_artifacts/starter';OUT.mkdir(parents=True,exist_ok=True)
ROM=R/'out/PokemonBattleArena.gba';ADAPTER=R/'tools/local_versus.lua'
raw=subprocess.check_output(['readelf','-sW',str(R/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name):return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',raw)[1],16)
MAIN,STARTER,PRACTICE,DEPTH,VERSUS,ARCADE=map(symbol,['pkmn_debug','pkmn_starter_debug','pkmn_practice_debug','pkmn_depth_debug','pkmn_versus_debug','pkmn_arcade_debug'])
SAVE=OUT/'progress.sav';shutil.copyfile(R/'tests/fixtures/nine_fighter_v1.sav',SAVE);original=SAVE.read_bytes()
errors=(OUT/'errors.log').open('w');p=subprocess.Popen([os.environ.get('PKMN_VERSUS_DRIVER',str(R/'.tooling/mgba_versus_driver')),str(ROM),str(ADAPTER),str(SAVE)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=errors,text=True,bufsize=1)
checks=peak=missed=0;started=time.time();movie=[];cases=[]
def command(v):
 p.stdin.write(v+'\n');p.stdin.flush();result=p.stdout.readline().strip()
 if not result:raise RuntimeError('Emulator exited: '+(OUT/'errors.log').read_text()[-1000:])
 return result
def read(a=MAIN):return list(map(int,command(f's {a:08x}').split()[1:]))
def frames(n=1,p1=0,p2=0):
 global peak,missed
 command(f'f {n} {p1} {p2}');s=read();peak=max(peak,s[21]);missed=max(missed,s[22])
def controls(who,key,n=1):frames(n,key if who==0 else 0,key if who==1 else 0)
def tap(key=0,p2=0):frames(1,key,p2);frames(3)
def capture(name=None,record=False):
 path=OUT/((name or 'current')+'.ppm');command('d '+str(path));im=Image.open(path).convert('RGB')
 if name:im.save(OUT/(name+'.png'))
 if record:movie.append(im)
 return im
def check(v,message):
 global checks
 checks+=1
 if not v:
  capture('failure');raise AssertionError(message+' state='+str(read())+' starter='+str(read(STARTER))+' coach='+str(read(PRACTICE)))
def retry():
 frames(1,4);check(read()[1]==8,'Select retries through short Ready');frames(7);check(read()[1]==3,'Fresh play returns on frame eight')
def play_practice(character):
 for _ in range(13):
  if read()[2]==character:break
  tap(16)
 tap(256);check(read()[1]==10,'Selected starter guide opens');capture('guide-'+str(character));tap(2);tap(4)
 for _ in range(6):
  if read(DEPTH)[3]==5:break
  tap(16)
 check(read(DEPTH)[3]==5,'Starter technique lesson is selectable');tap(1);frames(185);check(read()[1]==3,'Lesson starts in the native arena')
def gesture(who,direction):controls(who,direction);frames(1);controls(who,direction)
def back_setup():tap(8);tap(128);tap(128);tap(1);check(read()[1]==9,'Pause Change returns to Practice setup')
def shared_chooser():
 if read()[1]==3:tap(8);tap(128);tap(128);tap(1)
 check(read()[1]==17,'Change restores independent Versus selection')
def start_versus(left,right):
 for who,desired in enumerate([left,right]):
  for _ in range(13):
   if read(VERSUS)[3+who]==desired:break
   controls(who,16);frames(3)
  check(read(VERSUS)[3+who]==desired,'Each player selects the requested starter')
 tap(1,1);frames(65);check(read()[1]==3,'Shared native fight starts')
try:
 info=command('i').split();check(info[1]=='a67176cd3e6bf60a5016d7adc94357142dd12617','Actual pinned upstream core')
 frames(90);tap(2);check(read()[1]==9,'Title shortcut opens Practice');play_practice(0)
 controls(0,16,18);check(read(STARTER)[3]==0,'Held direction cannot trigger Volt Step');retry()
 stamina=read()[8];gesture(0,16);check(read()[16]==9 and read(STARTER)[3]==1,'Second fresh tap activates one Volt Step on F1')
 check(read()[8]==stamina-12*60,'Native Volt Step spends exactly twelve stamina');start=read()[12]
 for _ in range(18):frames(1);capture(record=True)
 check(read()[12]-start==24*256,'Native dash travels exactly twenty-four pixels');check(read(PRACTICE)[2] and read(PRACTICE)[1]==26,'Actual completed dash clears its lesson');capture('pikachu-complete')
 retry();gesture(0,16);frames(3);capture('pikachu-volt-step',True);check(read(STARTER)[21]&1,'Renderer presents the electric cue')
 tap(8);frozen=read()[5:21];mobility=read(STARTER)[3:21];frames(35,16)
 check(read()[5:21]==frozen and read(STARTER)[3:21]==mobility,'Pause freezes dash movement, stamina, expiry and counts')
 frames(1);tap(8);frames(80);check(read(STARTER)[3]==1,'Pause inputs cannot retrigger the dash')
 retry();controls(0,16);frames(1);tap(8);tap(8);frames(57);controls(0,16);check(read(STARTER)[3]==0,'A direction tap before Pause cannot complete a gesture after Resume')
 back_setup();play_practice(2)
 while read()[5]<85:frames(1)
 controls(0,128);frames(45);check(not read(PRACTICE)[2],'A stationary dodge does not complete the rolling lesson');retry()
 while read()[5]<85:frames(1)
 stamina=read()[8];life=read()[6];start=read()[12];controls(0,128|16)
 check(read()[16]==4 and read(STARTER)[5]==1,'Down plus direction activates Shell Roll on F1');check(read()[8]==stamina-20*60,'Roll spends twelve plus eight stamina')
 for _ in range(40):
  frames(1);capture(record=True)
  if read(STARTER)[13]:capture('squirtle-shell-roll')
 check(read()[12]-start==20*256,'Native roll moves twenty pixels');check(read()[6]==life and read(PRACTICE)[2] and read(PRACTICE)[1]==30,'A real rolling super evasion clears the lesson');capture('squirtle-complete')
 retry();check(read(STARTER)[5]==0 and not read(STARTER)[11] and not read(PRACTICE)[2],'Fast retry clears roll state, counts and completion')
 back_setup();tap(2);check(read()[1]==15,'Back returns to visible modes');tap(128);check(read(ARCADE)[17]==7,'Local Versus is next to Practice');tap(1)
 for who in range(2):
  for character in [0,2]:
   start_versus(character if who==0 else 1,character if who==1 else 1);direction=16 if who==0 else 32;stamina=read()[8+who];position=read()[12+who]
   if character==0:gesture(who,direction);check(read()[16+who]==9 and read(STARTER)[3+who]==1,'Both Versus players can activate Volt Step independently');cost=12
   else:controls(who,128|direction);check(read(STARTER)[5+who]==1,'Both Versus players can activate Shell Roll independently');cost=20
   check(read()[8+who]==stamina-cost*60,'Each side pays the same real stamina cost')
   frames(3);check(read(STARTER)[21]&(1<<who if character==0 else 4<<who),'Both sides receive their actual visual cue')
   capture('versus-'+str(character)+'-'+str(who));frames(40);check(read()[12+who]!=position,'Each side moves with ordinary gameplay physics');cases.append(dict(player=who,fighter=character,cost=cost));shared_chooser()
 check(missed==0,'Zero missed native display frames')
 p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
finally:
 if p.poll() is None:p.kill();p.wait()
 errors.close()
check(p.returncode==0,'Upstream test exits cleanly');check(SAVE.read_bytes()==original,'Practice and Versus preserve solo SRAM byte for byte');check(not (OUT/'errors.log').read_text().strip(),'No actual emulator or Lua errors')
if movie:
 images=[im.resize((720,480),Image.Resampling.NEAREST) for im in movie];images[0].save(OUT/'starter-native.gif',save_all=True,append_images=images[1:],duration=70,loop=0)
report=dict(status='passed',checks=checks,rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(),mgba_revision=info[1],emulated_gba_instances=1,raw_host_key_events=True,combat_state_writes=False,signature_fighters=['pikachu','squirtle'],drills_completed=True,first_frame_actions=True,both_player_sides=True,pause_gesture_cleared=True,pause_freezes_mobility=True,fast_retry=True,save_byte_identical=True,versus_cases=cases,peak_cpu_fraction=peak/4096,missed_display_frames=missed,wall_seconds=round(time.time()-started,2),method='One actual pinned upstream mGBA 0.11 core, raw-key events and shipping Lua adapter; actual lessons, super evasion, both player sides, framebuffer captures and unchanged SRAM. No live combat-state writes; physical keyboard and human balance remain unverified.')
(OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
