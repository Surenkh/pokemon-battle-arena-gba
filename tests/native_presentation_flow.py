"""Audio mix/mutes and victory recall, using physical buttons and native capture."""
from pathlib import Path
import array,hashlib,json,math,os,re,subprocess,time,wave
from PIL import Image
r=Path(__file__).resolve().parents[1];out=r/'tests/native_artifacts/presentation';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name,size=128):return int(re.search(r'\b([0-9a-f]{8})\s+'+str(size)+r'\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug');detail=symbol('pkmn_presentation_debug');feel=symbol('pkmn_feel_debug',64)
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('w'),text=True,bufsize=1)
checks=0;started=time.time();captures=[];mix={};rounds=[];positions=set();worst=0

def cmd(value):
 p.stdin.write(value+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Emulator stopped: '+str(p.poll()))
 return line
def frames(n,k=0):cmd(f'f {n} {k}')
def press(k):frames(2,k);frames(2)
def read(addr,op='s'):return list(map(int,cmd(f'{op} {addr:08x}').split()[1:]))
def state():return read(main)
def info():return read(detail)
def check(value,message):
 global checks
 checks+=1
 if not value:raise AssertionError(message+' state='+str(state())+' presentation='+str(info()))
def capture(name):cmd('d '+str(out/(name+'.ppm')));captures.append(name)
def audio_stats(name,silent=False):
 with wave.open(str(out/(name+'.wav'))) as w:
  check(w.getframerate()==32768 and w.getnchannels()==2,'Actual stereo audio format');samples=array.array('h',w.readframes(w.getnframes()))
 check(len(samples)>1000,'Recorded audio samples')
 peak=max(abs(x) for x in samples);rms=math.sqrt(sum(x*x for x in samples)/len(samples));clipped=sum(abs(x)>=32760 for x in samples)/len(samples)
 if name=='title_music':
  span=32768//5*2
  windows=[math.sqrt(sum(x*x for x in samples[i:i+span])/span) for i in range(0,len(samples)-span,span)]
  check(min(windows)>30,'Title loop has no silent gaps')
 if silent:check(peak<=2,'Disabled buses produce actual digital silence')
 else:check(rms>10 and clipped==0,'Audible cue without clipping')
 return dict(peak=peak,rms=round(rms,2),rms_dbfs=round(20*math.log10(max(rms,1)/32768),2),clipped_fraction=clipped,frames=len(samples)//2)
def change():
 v=state()
 if v[1]==5:frames(170)
 if state()[1]==6:press(16);press(1)
 elif state()[1]==3:press(8);press(128);press(128);press(1)
 check(state()[1]==1,'Return to selector')
def level(value):
 if state()[2]<3:press(128)
 press(128)
 while state()[4]!=value:press(16)
 press(64)
 while state()[2]!=0:press(16)
def fight_until_recall(idle=False):
 # Ordinary external play; no state writes or privileged game inputs.
 for n in range(6500):
  v=state()
  if v[1]!=3:break
  keys=0;gap=abs(v[13]-v[12])//256
  if not idle:
   toward=16 if v[13]>v[12] else 32;away=32 if v[13]>v[12] else 16
   if n%6==0:
    if v[10]==100 and (v[8]<1500 or (gap<80 and ((v[13]>v[12] and v[12]>34*256) or (v[13]<v[12] and v[12]<206*256)))):keys=away
    elif v[10]==100 and v[8]>=1500 and v[14]>=120*256:keys=256
    elif gap<27 and v[8]>=480:keys=1
    elif gap>48 and v[8]>=1800 and n%18==0:keys=2
    elif gap>24:keys=toward
   elif v[16]==0:
    if v[10]==100 and gap<80:keys=away
    elif gap>24:keys=toward
  frames(1,keys)
 check(state()[1]==5,'Real fight reaches recall')
def observe_recall(name,skip=False):
 v=state();outcome=v[20];frozen=v[5:21];loser=2-outcome;mask=0;last_scale=256
 cmd('o '+str(out/(name+'_recall.wav')))
 check(outcome in (1,2),'A real winner selects one defeated fighter')
 check(info()[12]==loser and info()[13]==1,'Recall selects actual loser')
 for _ in range(180):
  v=state();d=info()
  if v[1]!=5:break
  check(v[5:21]==frozen,'Recall preserves complete resolved combat state')
  check(d[16]<=last_scale,'Recall scale shrinks monotonically');last_scale=d[16];mask|=1<<d[11]
  check(d[26+1-loser]==1,'Winner remains visible')
  check(d[26+loser]==(d[10]<92),'Rendered loser disappears inside the ball')
  check(d[28]==d[18] and d[29]==d[19],'Native ball/beam visibility follows sequence')
  check(d[23]<=3,'Three total effect voices maximum')
  if d[10] in (24,44,56,68,80,91,98,112,128,144,152):capture(name+'_'+str(d[10]))
  if d[10]%3==0:cmd('d '+str(out/(name+f'_frame_{d[10]:03}.ppm')))
  if skip and d[10]>=112:press(1);break
  frames(1)
 if not skip:
  for n in range(20):
   frames(3);cmd('d '+str(out/(name+f'_frame_{156+n*3:03}.ppm')))
 cmd('c')
 check(state()[1]==6,'Recall leads to actual result screen')
 mix[name+'_recall']=audio_stats(name+'_recall')
 check(info()[26]==(outcome!=2),'Defeat result keeps the recalled player inside the ball')
 if not skip:check(mask==127,'All seven recall phases rendered')
 capture(name+'_result');rounds.append(dict(outcome=outcome,loser=loser,phase_mask=mask,skipped=skip))

try:
 frames(90);check(state()[1]==0,'Title boot');check(info()[3]==1 and info()[4]==84,'Title tune at moderate target gain');capture('title')
 # Longer than one original loop: verify all four order positions and continued playback.
 cmd('o '+str(out/'title_music.wav'))
 for _ in range(22):frames(120);positions.add(info()[22]);check(info()[3]==1,'Title music continues and loops')
 cmd('c');mix['title_music']=audio_stats('title_music');check(positions=={0,1,2,3},'All four music patterns play')
 check(-36<mix['title_music']['rms_dbfs']<-17,'Moderate measured title-music level')
 press(512);check(state()[1]==11,'L opens title audio settings');capture('audio_on')
 press(1);frames(12);check(info()[1]==0 and info()[3]==0,'Music off stops the native music bus')
 press(128);press(1);frames(12);check(info()[1:3]==[0,0],'Independent sound mute');capture('audio_off')
 cmd('o '+str(out/'muted_menu.wav'));frames(90);cmd('c');mix['muted_menu']=audio_stats('muted_menu',True)
 press(2);press(8);press(1);frames(185);check(state()[1]==3 and info()[1:3]==[0,0],'Settings persist into a battle')
 cmd('o '+str(out/'muted_battle.wav'))
 for n in range(120):frames(1,2 if n%24==0 else 512)
 cmd('c');mix['muted_battle']=audio_stats('muted_battle',True);check(read(feel,'v')[4]==0 and info()[5]==0,'Muted battle has no effect voices')
 press(8);check(state()[1]==4,'Pause');frozen=state()[5:21];press(512);check(state()[1]==11,'L opens paused audio settings');frames(45);check(state()[5:21]==frozen,'Options preserve paused combat')
 press(1);frames(40);check(info()[1:3]==[1,0] and info()[3]==1,'Music can be enabled independently for a menu preview')
 press(128);press(1);frames(15);check(info()[1:3]==[1,1],'Sound enabled independently')
 press(64);press(1);frames(16);check(info()[1:3]==[0,1] and info()[3]==0,'Sound effects work independently with music off')
 cmd('o '+str(out/'menu_cues.wav'))
 for n in range(6):press(128);frames(8)
 cmd('c');mix['menu_cues']=audio_stats('menu_cues');check(mix['menu_cues']['peak']>1500,'Navigation cues have a useful foreground level');press(1);frames(35);check(info()[1:3]==[1,1],'Restore both options for normal play')
 press(2);frames(30);check(state()[1]==4 and info()[3]==0,'Return to paused battle stops menu music')
 press(8);frames(65);check(state()[1]==3,'Resume works after options');change()
 # Actual idle loss provides defeat recall, then successful external play gives victory recall.
 level(1);press(1);frames(185);fight_until_recall(True);check(state()[20]==2,'Idle player actually loses');observe_recall('defeat')
 change();level(0);won=False
 for attempt in range(6):
  press(1);frames(185);fight_until_recall();outcome=state()[20];observe_recall('victory' if outcome==1 else 'retry_'+str(attempt))
  if outcome==1:won=True;break
  change()
 check(won,'Actual player victory recall exercised')
 change();press(1);frames(185);fight_until_recall();observe_recall('skip',True)
 # Both outcome cues and all recall cues have gone through the real dispatcher.
 check(info()[6]&0x1fff==0x1fff,'Navigation, countdown, pause, recall and win/lose cue coverage')
 check(state()[22]==0,'No missed native display frames')
 result=dict(status='passed',checks=checks,mix=mix,music_positions=sorted(positions),recall_rounds=rounds,cue_mask=info()[6],peak_cpu_fraction=state()[21]/4096,missed_display_frames=state()[22],wall_seconds=round(time.time()-started,2),rom_sha256=hashlib.sha256((r/'out/PokemonBattleArena.gba').read_bytes()).hexdigest(),method='mGBA 0.10.2; physical inputs, read-only telemetry, native framebuffer/audio capture; no game-memory writes')
 (out/'report.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
finally:
 if p.poll() is None:cmd('c');p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
