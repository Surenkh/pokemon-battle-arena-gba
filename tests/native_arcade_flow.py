"""Visible mode menu, new fighters, Arcade and schema-1/2 SRAM integration."""
from pathlib import Path
import hashlib,json,os,re,subprocess,time,struct
from PIL import Image
r=Path(__file__).resolve().parents[1];out=r/'tests/native_artifacts/arcade';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name,size=128):return int(re.search(r'\b([0-9a-f]{8})\s+'+str(size)+r'\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug');detail=symbol('pkmn_arcade_debug');audio=symbol('pkmn_presentation_debug');depth=symbol('pkmn_depth_debug');feel=symbol('pkmn_feel_debug',64);mastery=symbol('pkmn_mastery_debug')
chars=json.loads((r/'config/balance.json').read_text())['characters'];save=out/'arcade.sav';save.unlink(missing_ok=True)
checks=0;started=time.time();p=None;rounds=[];peak=0;missed=0;menu_keys=0;native_new=[];pictures=[]
def cmd(s):
 p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Emulator stopped '+str(p.poll()))
 return line
def read(addr,op='s'):return list(map(int,cmd(f'{op} {addr:08x}').split()[1:]))
def state():return read(main)
def info():return read(detail)
def frames(n,k=0):cmd(f'f {n} {k}')
def press(k):
 global menu_keys
 if state()[1] in (0,1,7,9,10,11,14,15):menu_keys|=k
 frames(2,k);frames(2)
def check(value,message):
 global checks,peak,missed
 checks+=1
 if not value:raise AssertionError(message+(' state='+str(state())+' detail='+str(info()) if p is not None else ''))
 if p is not None:
  v=state();peak=max(peak,v[21]);missed=max(missed,v[22])
def capture(name):
 frames(6);path=out/(name+'.ppm');cmd('d '+str(path));Image.open(path).resize((960,640),Image.Resampling.NEAREST).save(out/(name+'.png'))
def record(n,k,name):
 for t in range(n):
  frames(1,k)
  if t%4==0:
   path=out/(name+f'_{len(pictures):03}.ppm');cmd('d '+str(path));pictures.append(path)
def boot():
 global p
 p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba'),str(save)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('a'),text=True,bufsize=1)
 frames(90);check(state()[1]==0,'Boot title')
def close():
 global p
 if p is not None and p.poll() is None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10);assert p.returncode==0
 p=None
def menu():
 if state()[1]==0:press(8)
 elif state()[1] in (1,9,14):press(2)
 check(state()[1]==15,'Visible mode menu')
def choose(row):
 menu()
 while info()[17]!=row:press(128)
 press(8)
 check(state()[1]==(1 if row<2 else 9 if row==2 else 14 if row==3 else 11 if row==4 else 7),'Menu button enters intended mode')
def character(ch):
 while state()[2]!=ch:press(16)
def level(value):
 while state()[2]<6:press(128)
 press(128)
 while state()[4]!=value:press(32)
 press(64)
def leave_practice():
 press(8);check(state()[1]==4,'Pause practice');press(128);press(128);press(1);check(state()[1]==9,'Return to practice setup');press(2);check(state()[1]==15,'Practice back returns to menu')
def setup_arcade():
 choose(1);level(0);character(3);press(8)
 check(state()[1]==2,'Arcade countdown');roster=info()[12:17]
 check(len(set(roster))==5 and 3 not in roster and all(0<=x<9 for x in roster),'Five unique non-mirror rivals from all nine fighters')
 check(info()[4]==2,'Two starting continues');frames(185)
def finish(auto=True):
 if auto:cmd(f'b 6500 {main:08x} {feel:08x}')
 else:frames(5000)
 if state()[1]==5:frames(180)
 check(state()[1] in (12,13),'Arcade result/champion reached');v=state();rounds.append(dict(outcome=v[20],stage=info()[3],continues=info()[4],arena=read(mastery)[2]));return v[20]
try:
 boot();capture('title');menu();capture('mode_menu')
 # Every mode is reachable using only directions and Start/A/B.
 choose(0);capture('nine_fighter_selector');press(2);choose(1);check(info()[1]==1,'Arcade mode distinct from Quick Battle');press(2)
 choose(2);capture('practice_selector');press(2);choose(3);capture('records_empty');press(2)
 choose(4);press(1);press(2);check(read(audio)[1:3]==[0,1],'Audio menu toggles music');choose(5);capture('controls');press(2);close()
 boot();check(read(audio)[1:3]==[0,1],'Settings survive power cycle');close()
 # A fixed old-format save retains old records and audio preferences.
 save.write_bytes((r/'tests/fixtures/six_fighter_v08.sav').read_bytes());boot();check(read(audio)[1:3]==[0,1],'Schema-1 audio preferences loaded')
 choose(3);press(64);check(state()[4]==0 and info()[8:11]==[2,1,3600],'Schema-1 Pikachu record retained')
 character(5);press(64);check(state()[4]==2 and info()[8:11]==[7,3,14400],'Schema-1 Meowth record retained');character(6);check(info()[8:11]==[0,0,0],'New fighter starts with empty records');press(2)
 choose(4);press(1);press(2);check(not info()[11],'Migrated save write verified');close();raw=bytearray(save.read_bytes());check(struct.unpack_from('<I',raw,4)[0]==3,'Next save writes schema 3')
 raw[40]^=1;save.write_bytes(raw);boot();check(read(audio)[1:3]==[0,1],'Corrupt newest copy falls back to schema-1 copy');choose(4);press(1);press(2)
 # Each new fighter has a guide and actual native movement/attack/charge VFX.
 for ch in (6,7,8):
  choose(0);character(ch);press(4);press(128);press(128);press(1);check(state()[1]==10,'Visible Info opens without R');capture('guide_'+chars[ch]['id']);press(2);press(2);press(2)
  choose(2);character(ch);capture('select_'+chars[ch]['id']);press(8);frames(185)
  check(state()[1]==3 and state()[6]==chars[ch]['max_hp'] and state()[8]==chars[ch]['max_stamina']*60 and state()[10]==100,'New fighter practice resources')
  pictures=[];record(55,0,chars[ch]['id']);record(100,16,chars[ch]['id']);press(64);record(60,0,chars[ch]['id']);press(1);record(36,0,chars[ch]['id']);press(2);record(40,0,chars[ch]['id']);press(512);record(12,512,chars[ch]['id']);frames(20);press(256);record(85,0,chars[ch]['id'])
  check(state()[1]==3,'New fighter remains in playable practice');f=read(feel,'v');check(f[5]&(1<<ch) and f[6]&(1<<ch) and f[7]&(1<<ch),'New themed shot, charge and release played');check(read(depth)[19]==1,'Practice uses shared combat rules');capture('battle_'+chars[ch]['id'])
  images=[Image.open(path).resize((480,320),Image.Resampling.NEAREST) for path in pictures];images[0].save(out/(chars[ch]['id']+'-native.gif'),save_all=True,append_images=images[1:],duration=67,loop=0)
  native_new.append(dict(character=chars[ch]['id'],audio_mask=f[5],charge_mask=f[6],release_mask=f[7],captured_frames=len(images)));leave_practice()
 # Failed Arcade run has no clear, and cannot gain a third continue.
 setup_arcade();capture('arcade_battle');press(8);clock=info()[6];frames(90);check(info()[6]==clock,'Pause freezes Arcade clock');press(8);frames(65)
 for remaining in (2,1,0):
  check(finish(False)==2,'Idle player loses naturally');check(info()[4]==remaining and info()[3]==0,'Loss preserves opponent and waits for continue choice');capture('loss_'+str(remaining));clock=info()[6];frames(60);check(info()[6]==clock,'Result screen freezes run clock')
  if remaining:
   before_arena=read(mastery)[2];press(8);check(state()[1]==2 and info()[4]==remaining-1,'Continue consumes exactly one');check(read(mastery)[2]==before_arena,'Continue preserves current arena');frames(185)
  else:press(8);check(state()[1]==15,'Exhausted run finishes with A/Start')
 check(state()[1]==15 and not info()[2],'End run returns to mode menu')
 # Actual external play, never gameplay memory writes, earns a five-win clear.
 champion=False
 for attempt in range(18):
  setup_arcade()
  while True:
   result=finish()
   if state()[1]==13:
    champion=True;check(info()[3]==5 and info()[8]==1 and info()[10]==info()[6],'Five wins grant one clear and exact run time');capture('champion');break
   if result==2 and info()[4]==0:press(2);break
   stage=info()[3];before_arena=read(mastery)[2];press(8);check(state()[1]==2 and state()[4]==0 and state()[6]==120 and state()[8]==6000 and state()[10]==0,'Next rival restores resources and chosen difficulty');check(state()[3]==info()[12+stage],'Correct next rival');check(read(mastery)[2]==((before_arena+1)%3 if result==1 else before_arena),'Winning rotates the arena; losing preserves it');frames(185)
  if champion:break
 check(champion,'Player clears five-rival Arcade');ticks=info()[10];perfect=info()[9];frames(90);check(info()[8]==1,'Champion screen does not double count');press(1);capture('records_clear');close()
 boot();choose(3);character(3)
 while state()[4]!=0:press(64)
 check(info()[8:11]==[1,perfect,ticks],'Completed record survives power cycle');check(not menu_keys&0x300,'All new menu paths use no L/R shoulder shortcuts');check(missed==0,'No missed display frames')
 report=dict(status='passed',checks=checks,mode_menu=True,menu_keys=menu_keys,no_shoulder_required_in_menus=True,roster_count=9,new_fighters=native_new,legacy_save_migration=True,corrupt_save_recovery=True,rounds=rounds,cleared_after_runs=attempt+1,best_ticks=ticks,no_continue_clears=perfect,peak_cpu_fraction=peak/4096,missed_display_frames=missed,rom_sha256=hashlib.sha256((r/'out/PokemonBattleArena.gba').read_bytes()).hexdigest(),wall_seconds=round(time.time()-started,2),method='mGBA 0.10.2 real buttons and read-only telemetry; native frame/audio dispatch; synthetic schema-1 SRAM fixture, restarts and offline save corruption; no gameplay-memory writes')
 (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
finally:close()
