"""Real one-core mGBA inputs: signature drills, counterplay and Local Versus."""
from pathlib import Path
from PIL import Image
import hashlib,json,os,re,shutil,subprocess,time
R=Path(__file__).resolve().parents[1];OUT=R/'tests/native_artifacts/techniques';OUT.mkdir(parents=True,exist_ok=True)
ROM=R/'out/PokemonBattleArena.gba';ADAPTER=R/'tools/local_versus.lua'
config=json.loads((R/'config/balance.json').read_text());startup=config['techniques']['shadow_burst']['startup_ticks']
raw=subprocess.check_output(['readelf','-sW',str(R/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name):return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',raw)[1],16)
MAIN,TECH,PRACTICE,DEPTH,VERSUS,ARCADE=map(symbol,['pkmn_debug','pkmn_technique_debug','pkmn_practice_debug','pkmn_depth_debug','pkmn_versus_debug','pkmn_arcade_debug'])
SAVE=OUT/'progress.sav';shutil.copyfile(R/'tests/fixtures/nine_fighter_v1.sav',SAVE);original=SAVE.read_bytes()
errors=(OUT/'errors.log').open('w');p=subprocess.Popen([os.environ.get('PKMN_VERSUS_DRIVER',str(R/'.tooling/mgba_versus_driver')),str(ROM),str(ADAPTER),str(SAVE)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=errors,text=True,bufsize=1)
checks=peak=missed=0;started=time.time();movie=[];counter_cases=[]
sheet=Image.open(R/'graphics/shadow_burst.bmp');palette=sheet.getpalette();art=[]
for phase in range(8):
 points=[]
 for y in range(32):
  for x in range(32):
   index=sheet.getpixel((x,phase*32+y))
   if index:points.append((x,y,tuple(((v//8)<<3)|((v//8)>>2) for v in palette[index*3:index*3+3])))
 art.append(points)
def command(v):
 p.stdin.write(v+'\n');p.stdin.flush();result=p.stdout.readline().strip()
 if not result:raise RuntimeError('Emulator exited: '+(OUT/'errors.log').read_text()[-1000:])
 return result
def read(a=MAIN):return list(map(int,command(f's {a:08x}').split()[1:]))
def signed(v):return v-2**32 if v>=2**31 else v
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
  capture('failure');raise AssertionError(message+' state='+str(read())+' technique='+str(read(TECH)))
def visible_burst_mask(image,x,y):
 mask=0
 for phase,points in enumerate(art):
  for dx in [-1,0,1]:
   for dy in [-1,0,1]:
    if all(image.getpixel((x-16+px+dx,y-16+py+dy))==color for px,py,color in points):mask|=1<<phase
 return mask
def choose_practice(character):
 for _ in range(13):
  if read()[2]==character:return
  tap(16)
 check(False,'Requested Practice fighter reachable')
def play_practice(character):
 choose_practice(character);tap(256);check(read()[1]==10,'Fighter Info opens for the selected technique')
 capture('guide-'+str(character));tap(2);tap(4)
 check(read(DEPTH)[2]==1,'Adapter O opens options without changing fighter')
 for _ in range(6):
  if read(DEPTH)[3]==5:break
  tap(16)
 check(read(DEPTH)[3]==5 and read()[2]==character,'Technique mode belongs to the exact chosen fighter')
 capture('practice-setup-'+str(character));tap(1);frames(185)
 check(read()[1]==3 and read(PRACTICE)[9],'Technique drill enters the native arena')
def retry():
 frames(1,4);check(read()[1]==8,'O retries through the existing short Ready state')
 frames(7);check(read()[1]==3,'Technique retry accepts play within eight display frames')
def shared_chooser():
 if read()[1]==3:
  tap(8);tap(128);tap(128);tap(1)
 check(read()[1]==17,'Change Pokemon restores the shared chooser')
def start_versus(gastly):
 desired=[10,7] if gastly==0 else [7,10]
 for who in range(2):
  for _ in range(13):
   if read(VERSUS)[3+who]==desired[who]:break
   controls(who,16);frames(3)
  check(read(VERSUS)[3+who]==desired[who],'Both players independently select the requested technique fighter')
 tap(1,1);frames(65);check(read()[1]==3 and read()[2:4]==desired,'Both techniques run in the same shared fight')
def ready_orb(who):
 controls(who,2);frames(1)
 for _ in range(100):
  q=read(TECH)
  if q[13+who] and abs(q[17+who]-read()[13-who])<=17*256:return
  frames(1)
 check(False,'Live orb reaches a usable burst position')
def arm(who):
 before=read()[8+who];controls(who,2)
 q=read(TECH);check(signed(q[15+who])>=0 and q[26+who]==1,'Fresh B arms exactly one owned orb on F1')
 check(read()[8+who]==before-8*60,'Native burst spends exactly eight extra stamina')
 return q[15+who]
try:
 info=command('i').split();check(info[1]=='a67176cd3e6bf60a5016d7adc94357142dd12617','Pinned actual upstream core')
 frames(90);tap(2);check(read()[1]==9,'Title Back opens ordinary Practice through the adapter')
 play_practice(7)
 controls(0,64);baseline_air=1
 while read()[14]<120*256 and baseline_air<120:frames(1);baseline_air+=1
 check(baseline_air<120 and not read(PRACTICE)[2],'An ordinary jump does not clear the technique drill')
 retry();controls(0,64);braked_air=1
 for _ in range(100):
  if read(TECH)[9]:break
  frames(1);braked_air+=1
 check(read(TECH)[9] and signed(read(TECH)[7])>0,'Brake opportunity appears only during real descent')
 before=read()[8];controls(0,64);braked_air+=1
 check(read(TECH)[24]==1 and read()[8]==before-8*60,'Fresh descending Up accepts Air Brake on F1 with its real cost')
 check(0<=signed(read(TECH)[7])<=128 and read(TECH)[30]&1,'Native renderer presents the active float cue')
 frames(2);braked_air+=2 # Allow the normal GBA sprite upload before the capture.
 capture('jigglypuff-air-brake',True)
 tap(8);frozen=read()[5:21];state=read(TECH)[3:23];frames(35,64)
 check(read()[5:21]==frozen and read(TECH)[3:23]==state,'Pause freezes brake motion and expiry')
 frames(1);tap(8);frames(65)
 while read()[14]<120*256 and braked_air<120:
  frames(1);braked_air+=1
  if braked_air%2==0:capture(record=True)
 check(read(PRACTICE)[2] and read(PRACTICE)[1]==17,'Real brake plus landing completes its drill')
 check(read(TECH)[24]==1 and not read(TECH)[3],'Pause inputs do not retrigger; landing restores one use')
 # Compare simulation ticks rather than paused/countdown display frames.
 check(read()[5]>baseline_air,'Air Brake changes actual landing timing')
 capture('jigglypuff-drill-complete')
 tap(8);tap(128);tap(128);tap(1);check(read()[1]==9,'Return to Practice setup')
 play_practice(10);ready_orb(0);armed=arm(0);life=read()[7];mask=0
 frames(1);capture('gastly-warning',True)
 while read()[5]<=armed+startup:
  q=read(TECH);check(read()[7]==life,'Entire native warning is harmless')
  if q[31]&15:mask|=1<<((q[31]&15)-1)
  frames(1);capture(record=True)
 check(read()[7]==life-10 and read(TECH)[28]==1,'Native burst inflicts exactly one ten-damage hit')
 check(read(PRACTICE)[2] and read(PRACTICE)[1]==20,'Only the actual burst hit clears Gastly training')
 check(mask==15,'All four outlined warning phases are rendered before damage')
 for _ in range(8):frames(1);capture(record=True)
 capture('gastly-burst-hit',True)
 retry();controls(0,2);frames(1)
 while not read(TECH)[13]:frames(1)
 arm(0);frames(2);retry()
 check(not read(TECH)[13] and signed(read(TECH)[15])==-1 and not read(PRACTICE)[2],'Fast retry clears an armed orb and drill completion')
 controls(0,2);frames(1)
 while not read(TECH)[13]:frames(1)
 arm(0);mask=0
 for _ in range(startup+6):
  q=read(TECH);phase=q[31]&15
  if phase:mask|=1<<(phase-1)
  if phase>=5:capture('gastly-active-blast',True)
  frames(1)
 check(mask==255 and not read(TECH)[13],'A native miss renders all eight warning/blast phases and expires')
 check(read(PRACTICE)[1]==21 and not read(PRACTICE)[2],'An out-of-range burst reports a retry instead of success')
 tap(8);tap(128);tap(128);tap(1);tap(16)
 check(read()[2]==11 and read(DEPTH)[3]==0,'Changing to an unsupported fighter restores Free Practice visibly')
 tap(2);check(read()[1]==15,'Back returns to the mode menu');tap(128);check(read(ARCADE)[17]==7,'Local Versus is the next visible play mode');tap(1)
 for who in range(2):
  start_versus(who);jiggly=1-who;controls(jiggly,64);frames(1)
  for _ in range(100):
   if read(TECH)[9+jiggly]:break
   frames(1)
  controls(jiggly,64);check(read(TECH)[24+jiggly]==1 and read(TECH)[30]&(1<<jiggly),'Each player can independently use Air Brake in Versus')
  frames(65);shared_chooser()
  for defense in ['none','block','jump','leave','shoot']:
   start_versus(who);ready_orb(who);arm(who);target=1-who;life=read()[6+target]
   button={'none':0,'block':512,'jump':64,'leave':16 if who==0 else 32,'shoot':2}[defense]
   visible_mask=0;orb_x,orb_y=read(TECH)[17+who]//256,read(TECH)[19+who]//256
   for _ in range(startup+8):
    controls(target,button)
    if who==0 and defense=='leave':visible_mask|=visible_burst_mask(capture(),orb_x,orb_y)
   if who==0 and defense=='leave':check(visible_mask==255,'Exact framebuffer/palette masks show all eight actual warning/blast phases')
   damage=life-read()[6+target]
   check(damage==(10 if defense=='none' else 0),'Actual '+defense+' counter outcome on side '+str(who))
   counter_cases.append(dict(caster=who,defense=defense,damage=damage))
   if who==0 and defense=='block':capture('versus-burst-block')
   shared_chooser()
 start_versus(0);ready_orb(0);arm(0);frames(3);tap(0,8)
 frozen=read(VERSUS)[15];phase=read(TECH)[15:23];frames(30,2,64)
 check(read(VERSUS)[15]==frozen and read(TECH)[15:23]==phase,'Either-player Pause freezes an armed orb and discards new attacks')
 command('x');frames(5);check(read()[1]==4 and not read(VERSUS)[2],'Disconnected adapter keeps the signature fight paused')
 command('c');frames(5);check(read()[1]==4 and read(VERSUS)[2],'Reconnect does not automatically resume a pending blast')
 tap(1);frames(80);check(read()[1]==3 and read(TECH)[26]==1,'Explicit resume neither rearms nor duplicates the orb')
 check(missed==0,'Zero missed display frames in native technique scenarios')
 if movie:
  images=[im.resize((480,320),Image.Resampling.NEAREST) for im in movie]
  images[0].save(OUT/'techniques-native.gif',save_all=True,append_images=images[1:],duration=70,loop=0)
finally:
 if p.poll() is None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
 errors.close()
check(p.returncode==0,'One-core test exits cleanly');check(SAVE.read_bytes()==original,'Practice and Versus leave solo SRAM byte-identical');check(not (OUT/'errors.log').read_text().strip(),'No actual Lua or mGBA errors')
report=dict(status='passed',checks=checks,rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),adapter_sha256=hashlib.sha256(ADAPTER.read_bytes()).hexdigest(),mgba_revision=info[1],emulated_gba_instances=1,raw_host_key_events=True,combat_state_writes=False,signature_fighters=['jigglypuff','gastly'],counter_cases=counter_cases,first_frame_actions=True,drills_completed=True,pause_reconnect=True,save_byte_identical=True,warning_frame_mask=15,all_burst_frame_mask=255,exact_framebuffer_phase_mask=255,peak_cpu_fraction=peak/4096,missed_display_frames=missed,wall_seconds=round(time.time()-started,2),method='One actual upstream mGBA 0.11 core, raw keyboard events, shipping Lua input adapter, read-only native telemetry and actual framebuffer captures. Physical keyboard/Qt GUI and human balance remain unverified.')
(OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
