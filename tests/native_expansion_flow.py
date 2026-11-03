"""Paged roster and independent RC1 save migration, through real GBA keys."""
from pathlib import Path
from PIL import Image
import hashlib,json,os,re,struct,subprocess,time
r=Path(__file__).resolve().parents[1];out=r/'tests/native_artifacts/expansion';out.mkdir(exist_ok=True);save=out/'expansion.sav';rom=r/'out/PokemonBattleArena.gba'
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name):return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols)[1],16)
MAIN,EXP,MASTERY,ARCADE,FEEL,AUDIO,DEPTH=map(symbol,['pkmn_debug','pkmn_expansion_debug','pkmn_mastery_debug','pkmn_arcade_debug','pkmn_feel_debug','pkmn_presentation_debug','pkmn_depth_debug'])
p=None;checks=peak=missed=0;started=time.time()
def cmd(value):
 p.stdin.write(value+'\n');p.stdin.flush();line=p.stdout.readline().strip()
 if not line:raise RuntimeError('Emulator stopped')
 return line
def read(addr=MAIN):return list(map(int,cmd(('v' if addr==FEEL else 's')+f' {addr:08x}').split()[1:]))
def frames(n=1,k=0):
 global peak,missed
 cmd(f'f {n} {k}');v=read();peak=max(peak,v[21]);missed=max(missed,v[22])
def tap(k):frames(1,k);frames(3)
def capture(name):
 frames(6);path=out/(name+'.ppm');cmd('d '+str(path));Image.open(path).save(out/(name+'.png'))
def check(ok,msg):
 global checks
 checks+=1
 if not ok:raise AssertionError(msg+(' state='+str(read()) if p is not None else ''))
def boot():
 global p
 p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(rom),str(save)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('a'),text=True,bufsize=1)
 frames(90);check(read()[1]==0,'Boot title');check(read(EXP)[:9]==[0x45585032,13,5,0,13,4,596,1024,2048],'Counts and save layout')
def close():
 global p
 if p is not None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10);assert p.returncode==0;p=None
def menu():
 screen=read()[1]
 if screen==0:tap(8)
 elif screen in (1,9):
  row=read(MASTERY)[3] if screen==1 else read(DEPTH)[2]
  if row:tap(2)
  tap(2)
 elif screen in (11,14,16):tap(2)
 check(read()[1]==15,'Mode menu')
def choose(mode):
 menu()
 for _ in range(7):
  if read(ARCADE)[17]==mode:break
  tap(128)
 tap(1)
def fighter(who):
 for _ in range(13):
  if read()[2]==who:break
  tap(16)
 check(read()[2]==who,'Correct fighter')
def records_check(expected):
 choose(3)
 for who in range(13):
  fighter(who)
  for level in range(3):
   while read()[4]!=level:tap(128)
   check(read(ARCADE)[8:11]==expected[who*3+level],'Exact saved record')
 tap(2)
def mastery_check(expected):
 choose(6)
 for who in range(13):fighter(who);check(read(MASTERY)[4:6]==expected[who],'Exact mastery counters')
 tap(2)
def change():
 tap(8);check(read()[1]==4,'Pause');tap(128);tap(128);tap(1);check(read()[1]==1,'Change returns selector')
try:
 save.unlink(missing_ok=True);boot();crossings=0
 for mode in (0,1,2):
  choose(mode)
  for who in range(13):
   fighter(who);frames(8);e=read(EXP);check(e[3:5]==[0,13],'Correct page and card count');check(e[9]==8191,'All thirteen cards stay visible')
   if who in (8,9,12):capture(f'setup-{mode}-fighter-{who}')
  tap(16);check(read()[2]==0,'Right wraps last fighter');tap(32);check(read()[2]==12,'Left wraps first fighter')
  fighter(0);tap(64);check(read()[2]==10,'Up wraps within a five-column grid');tap(64);check(read()[2]==5,'Up traverses a valid card');fighter(10);tap(128);check(read()[2]==10,'Down enters settings without changing fighter');tap(2)
  for _ in range(20):
   fighter(8);tap(16);check(read()[2]==9 and read(EXP)[9]==8191,'Rapid next fighter retains the full roster');tap(32);check(read()[2]==8 and read(EXP)[9]==8191,'Rapid previous fighter retains the full roster');crossings+=2
  tap(2)
 close()
 original=(r/'tests/fixtures/nine_fighter_v1.sav').read_bytes();fixture=json.loads((r/'tests/fixtures/nine_fighter_v1.json').read_text());check(hashlib.sha256(original).hexdigest()==fixture['sha256'],'Independent frozen RC1 fixture hash')
 records=fixture['records']+[[0,0,0]for _ in range(12)];mastery=fixture['mastery']+[[0,0]for _ in range(4)];save.write_bytes(original);boot();check(read(AUDIO)[1:3]==[0,0],'Legacy audio retained');records_check(records);mastery_check(mastery)
 choose(4);tap(1);tap(2);close();raw=save.read_bytes();check(raw[:1024]==original[:1024],'Migration leaves legacy copies untouched');check(struct.unpack_from('<I',raw,1028)[0]==4,'New independent schema-4 slot')
 boot();records_check(records);mastery_check(mastery);choose(0);fighter(9);tap(4)
 while read()[4]!=0:tap(32)
 tap(2);tap(1);frames(185)
 for tick in range(24000):
  v=read()
  if read(MASTERY)[4]>=1:break
  if v[1]==5:frames(180);continue
  if v[1]==6:tap(1);frames(185);continue
  status=read(FEEL)[14];gap=abs(v[13]-v[12])//256
  keys=2 if status in (7,8) else (16 if v[13]>v[12] else 32) if gap>20 and v[16] in (0,1) else 1 if v[16] in (0,1) and v[8]>=1800 and tick%8==0 else 0
  frames(1,keys)
 check(read(MASTERY)[4]>=1,'Vulpix earns a real follow-up');mastery[9]=read(MASTERY)[4:6];capture('vulpix-followup-earned');change();check(not read(MASTERY)[8],'Mastery save verified');close();boot();records_check(records);mastery_check(mastery)
 champion=False
 for attempt in range(24):
  choose(1);fighter(9);tap(4)
  while read()[4]!=0:tap(32)
  tap(2);tap(1);frames(185);arenas=set()
  while True:
   arenas.add(read(MASTERY)[2]);cmd(f'b 6500 {MAIN:08x} {FEEL:08x}');frames();check(read()[1] in (12,13),'Real Arcade round ends')
   if read()[1]==13:
    champion=True;check(arenas==set(range(5)),'Arcade visits all five arenas');records[27]=read(ARCADE)[8:11];mastery[9]=read(MASTERY)[4:6];check(records[27][0]==1 and records[27][2]>0,'New fighter earns first record');capture('vulpix-champion');tap(2);break
   if read()[20]==2 and not read(ARCADE)[4]:tap(2);break
   tap(1);frames(185)
  if champion:break
 check(champion,'New fighter clears Arcade');close();earned=save.read_bytes();check(earned[:1024]==original[:1024],'Gameplay saves preserve legacy bytes');boot();records_check(records);mastery_check(mastery);close()
 raw=bytearray(earned);offsets=[o for o in (1024,2048)if struct.unpack_from('<I',raw,o+4)[0]==4];check(len(offsets)==2,'Both new copies populated');newest=max(offsets,key=lambda o:struct.unpack_from('<I',raw,o+8)[0]);previous=next(o for o in offsets if o!=newest);prior=[list(struct.unpack_from('<III',raw,previous+16+i*12))for i in range(39)];raw[newest+40]^=1;save.write_bytes(raw);boot();records_check(prior);close()
 raw[1024:1028]=b'\0'*4;raw[2048:2052]=b'\0'*4;save.write_bytes(raw);records[27]=[0,0,0];mastery[9]=[0,0];boot();records_check(records);mastery_check(mastery);check(read(AUDIO)[1:3]==[0,0],'Both new copies invalid recovers original settings');close()
 check(not missed,'Zero missed display frames');check(peak<.9*4096,'Expansion journey below 90 percent')
 report=dict(status='passed',checks=checks,roster_count=13,arena_count=5,rapid_card_changes=crossings,max_visible_portraits=1,legacy_schema=3,save_schema=4,save_copy_bytes=596,save_offsets=[1024,2048],records_verified=39,mastery_fighters_verified=13,new_fighter_mastery_earned=True,new_fighter_arcade_clear=True,arcade_attempts=attempt+1,arcade_arenas=sorted(arenas),power_cycle=True,legacy_copies_unchanged=True,corrupt_newest_recovery=True,both_new_copies_legacy_recovery=True,peak_cpu_fraction=peak/4096,missed_display_frames=missed,rom_sha256=hashlib.sha256(rom.read_bytes()).hexdigest(),wall_seconds=round(time.time()-started,2),method='mGBA 0.10.2, physical GBA keys and read-only telemetry; frozen RC1 SRAM, actual Vulpix mastery and five-rival clear, power cycles and offline corruption; no gameplay-memory writes')
 (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
finally:close()
