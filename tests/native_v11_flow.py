"""v1.1: all thirteen visible cards, exact choices and real super/punch frames."""
from pathlib import Path
from PIL import Image
import hashlib,json,os,re,subprocess,time
R=Path(__file__).resolve().parents[1];OUT=R/'tests/native_artifacts/v11';OUT.mkdir(exist_ok=True);ROM=R/'out/PokemonBattleArena.gba'
CHARS=json.loads((R/'config/balance.json').read_text())['characters'];LAYOUT=json.loads((R/'config/selection.json').read_text());FONT=Image.open(R/'docs/pkfont.png').convert('RGB')
GLYPHS=json.loads(subprocess.check_output(['node','-e',"process.stdout.write(JSON.stringify(require('./tools/selection_art.cjs').glyphs))"],cwd=R,text=True))
symbols=subprocess.check_output(['readelf','-sW',str(R/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name):return int(re.search(r'\b([0-9a-f]{8})\s+\d+\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols)[1],16)
MAIN,POWER,DETAIL,DEPTH,ARCADE=map(symbol,['pkmn_debug','pkmn_power_debug','pkmn_mastery_debug','pkmn_depth_debug','pkmn_arcade_debug'])
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(R/'tests/mgba_driver')),str(ROM)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(OUT/'errors.log').open('w'),text=True,bufsize=1)
checks=peak=missed=0;started=time.time();movie=[];cases=[];choices=[]
def cmd(v):
 p.stdin.write(v+'\n');p.stdin.flush();s=p.stdout.readline().strip()
 if not s:raise RuntimeError('Emulator stopped')
 return s
def read(a=MAIN):return list(map(int,cmd(f's {a:08x}').split()[1:]))
def frames(n=1,k=0):
 global peak,missed
 cmd(f'f {n} {k}');s=read();peak=max(peak,s[21]);missed=max(missed,s[22])
def tap(k):frames(1,k);frames(3)
def capture(name=None):
 path=OUT/((name or 'current')+'.ppm');cmd('d '+str(path));im=Image.open(path).convert('RGB')
 if name:im.save(OUT/(name+'.png'))
 return im
def check(value,msg):
 global checks
 checks+=1
 if not value:capture('failure');raise AssertionError(msg+' state='+str(read())+' power='+str(read(POWER)))
def white(im):
 pixels=im.tobytes();return bytes(int(min(pixels[i:i+3])>=180)for i in range(0,len(pixels),3))
def text(im,value,x,y,right=False):
 expected=Image.new('RGB',(len(value)*8,8))
 for i,c in enumerate(value):
  if c!=' ':g=ord(c)-33;expected.paste(FONT.crop((0,g*8,8,g*8+8)),(i*8,0))
 left=x-expected.width if right else x-expected.width//2;target=white(expected)
 check(any(white(im.crop((left+dx,top,left+dx+expected.width,top+8)))==target for dx in [-1,0,1]for top in range(y-5,y+1)),'Exact visible label '+value)
def all_names(im):
 for who,c in enumerate(CHARS):
  label=c['name'].upper();width=len(label)*4-1;left=LAYOUT['grid_x']+(who%5)*LAYOUT['step_x']+22-width//2;top=LAYOUT['grid_y']+(who//5)*LAYOUT['step_y']+19
  expected=[]
  for y in range(5):
   for i,ch in enumerate(label):
    for x in range(4):
     if i*4+x<width:expected.append(int(x<3 and GLYPHS[ch][y]&(4>>x)!=0))
  check(white(im.crop((left,top,left+width,top+5)))==bytes(expected),'Visible full card name '+label)
def row():return read(DEPTH)[2]if read()[1]==9 else read(DETAIL)[3]
def menu():
 screen=read()[1]
 if screen==3:tap(8);tap(128);tap(128);tap(1);screen=read()[1]
 if screen in [1,9]:
  if row():tap(2)
  tap(2)
 elif screen==0:tap(8)
 check(read()[1]==15,'Visible mode menu')
def choose(mode):
 menu()
 for _ in range(9):
  if read(ARCADE)[17]==mode:break
  tap(128)
 tap(1)
def fighter(who):
 if row():tap(2)
 for _ in range(13):
  if read()[2]==who:break
  tap(16)
 check(read()[2]==who,'Requested fighter')
def setup(ch,stage,training=False):
 choose(2 if training else 0);fighter(ch);tap(4)
 if training:
  while read(DEPTH)[3]!=0:tap(16)
  tap(128);tap(128)
 else:tap(128)
 while read(DETAIL)[1]!=stage:tap(16)
 check(read()[2]==ch,'Location changes retain fighter')
def play(ch,stage,training):
 tap(1);check(read()[1]==2 and read()[2]==ch,'One confirm starts intended fighter');frames(185)
 check(read()[1]==3 and read(DETAIL)[2]==stage and bool(read(DEPTH)[19])==training,'Actual battle uses selected location and mode')
try:
 frames(90)
 # Every fighter stays visible in all three setups, including its full card name.
 for mode in [0,1,2]:
  choose(mode)
  for ch in range(13):
   fighter(ch);frames(6);q=read(POWER);check(q[8]==8191 and q[9]==1<<ch and read()[2]==ch,'All cards visible with exactly one animated portrait')
   im=capture();all_names(im);text(im,CHARS[ch]['name'].upper(),230,9,True)
   if ch in [0,7,12]:capture('setup-'+str(mode)+'-'+str(ch))
   before=(read()[4],read(DETAIL)[1]);tap(4);check(row()==1 and read()[2]==ch,'Select focuses settings for the chosen fighter');tap(2);check(row()==0 and before==(read()[4],read(DETAIL)[1]),'Back restores exact choice')
  tap(16);check(read()[2]==0,'Right wraps to first fighter');tap(32);check(read()[2]==12,'Left wraps to last fighter')
  fighter(0);tap(64);check(read()[2]==10,'Up wraps within first column');tap(128);check(row()==1 and read()[2]==10,'Down from last card opens options without changing it');tap(2);tap(2)
 # All five locations are named correctly and the preview follows immediately.
 for training in [False,True]:
  for stage in range(5):
   ch=(stage+9)%13;setup(ch,stage,training);frames(6);check(read(POWER)[7]==stage,'Preview matches selected location');im=capture(('practice'if training else 'quick')+'-location-'+str(stage));text(im,['WOODLAND','VOLCANIC','SHORELINE','FROSTPEAK','ANCIENT RUINS'][stage],220,132 if training else 128,True)
   play(ch,stage,training);choices.append(dict(fighter=ch,arena=stage,training=training));menu()
 choose(0);fighter(7);tap(4)
 for level in [0,1,2]:
  while read()[4]!=level:tap(16)
  frames(6);im=capture('difficulty-'+str(level));text(im,['EASY','NORMAL','HARD'][level],180,114);check(read()[2]==7,'Difficulty retains selected fighter')
 old=read()[4];frames(1,16);check(read()[4]==(old+1)%3,'One-frame difficulty tap changes one value');frames(18,16);check(read()[4]==(old+1)%3,'Held direction never repeats a setting');frames();tap(128);tap(128);tap(1);check(read()[1]==10,'Visible Fighter Info opens');tap(2);check(row()==3 and read()[2]==7,'Guide restores exact fighter and row');tap(2);tap(2)
 choose(1);fighter(12);tap(4);tap(128);check(row()==3 and read(POWER)[7]==0xffffffff,'Arcade skips manual location and shows rotation');tap(2);tap(2)
 # Each fighter renders every actual aura, super, burst and punch phase.
 for ch,c in enumerate(CHARS):
  setup(ch,ch%5,True);play(ch,ch%5,True);frames(12,32);frames(2)
  frames(1,256);check(read()[16]==5,'A real full-meter super starts');aura=wave=burst=0
  for t in range(130):
   q=read(POWER)
   if q[20]:aura|=1<<(q[20]-1)
   if q[16]:wave|=1<<(q[16]-1)
   burst|=q[22]
   if ch in [0,9]and t%2==0 and t<90:movie.append(capture())
   if ch==0 and t in [10,35,45,65]:capture('super-'+str(t))
   frames()
  check(aura==wave==burst==255,'All eight real aura, super and impact frames for '+c['name']);check(read()[7]==CHARS[read()[3]]['max_hp']-c['super']['damage'],'Super retains its actual damage')
  for _ in range(500):
   v=read()
   if abs(v[13]-v[12])<=20*256:break
   frames(1,16 if v[13]>v[12] else 32)
  frames(3);hp=read()[7];frames(1,1);check(read()[16]==2,'Actual melee starts');punch=0
  for t in range(60):
   q=read(POWER)
   if q[18]:punch|=1<<(q[18]-1)
   if ch in [0,9]and t%2==0 and t<36:movie.append(capture())
   if ch==0 and t in [2,5,10]:capture('punch-'+str(t))
   frames()
  check(punch==63,'All six real punch frames for '+c['name']);check(hp-read()[7]==c['melee']['damage'],'Punch animation retains melee damage');cases.append(dict(fighter=c['id'],aura_frame_mask=aura,super_frame_mask=wave,impact_frame_mask=burst,punch_frame_mask=punch));menu()
 check(missed==0,'Zero missed native display frames')
 images=[im.resize((480,320),Image.Resampling.NEAREST)for im in movie]
 images[0].save(OUT/'v1-1-power-native.gif',save_all=True,append_images=images[1:],duration=34,loop=0)
 report=dict(status='passed',checks=checks,roster_pages=1,visible_fighters=13,max_animated_portraits=1,full_card_glyph_masks=True,selected_header_glyph_masks=True,correct_location_glyph_masks=True,configured_battles=choices,held_setting_no_repeat=True,arcade_location_rotation=True,characters=cases,punch_damage_unchanged=True,super_damage_unchanged=True,peak_cpu_fraction=peak/4096,missed_display_frames=missed,rom_sha256=hashlib.sha256(ROM.read_bytes()).hexdigest(),wall_seconds=round(time.time()-started,2),method='mGBA 0.10.2 physical GBA keys, exact native glyph masks, real HP changes and read-only per-owner frame telemetry; no game-memory writes')
 (OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in ['characters','configured_battles']},indent=2))
finally:
 if p.poll()is None:p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
