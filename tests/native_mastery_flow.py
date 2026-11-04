"""v0.9 arenas/mastery: real GBA buttons, read-only telemetry and SRAM restarts."""
from pathlib import Path
import hashlib,json,os,re,struct,subprocess,time
from PIL import Image

r=Path(__file__).resolve().parents[1]
out=r/'tests/native_artifacts/mastery';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name,size=128):
    return int(re.search(r'\b([0-9a-f]{8})\s+'+str(size)+r'\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug');detail=symbol('pkmn_mastery_debug');arcade=symbol('pkmn_arcade_debug')
depth=symbol('pkmn_depth_debug');feel=symbol('pkmn_feel_debug',64);audio=symbol('pkmn_presentation_debug')
save=out/'mastery.sav';save.unlink(missing_ok=True)
p=None;checks=0;peak=0;missed=0;menu_keys=0;started=time.time();arena_checks=[];match_count=0
def cmd(s):
    p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
    if not line:raise RuntimeError('Emulator stopped: '+str(p.poll())+'; see '+str(out/'errors.log'))
    return line
def read(addr,op='s'):return list(map(int,cmd(f'{op} {addr:08x}').split()[1:]))
def state():return read(main)
def info():return read(detail)
profile_missed=0
def frames(n,k=0):
    global profile_missed
    if os.environ.get('PKMN_PROFILE_FRAMES')!='1':cmd(f'f {n} {k}');return
    for _ in range(n):
        cmd(f'f 1 {k}');v=state();d=read(depth)
        if v[22]>profile_missed:
            print('FRAME_PROFILE',dict(keys=k,screen=v[1],fighter=v[2],frame=v[23],cpu=d[20]/4096,missed=v[22],mastery=info()[:10]),flush=True)
        profile_missed=v[22]
def press(k):
    global menu_keys
    if state()[1] in (0,1,7,9,10,11,14,15,16):menu_keys|=k
    frames(2,k);frames(2)
def check(value,message):
    global checks,peak,missed
    checks+=1
    if not value:raise AssertionError(message+(' state='+str(state())+' mastery='+str(info()) if p is not None else ''))
    if p is not None:
        v=state();peak=max(peak,v[21]);missed=max(missed,v[22])
def capture(name):
    frames(6);path=out/(name+'.ppm');cmd('d '+str(path))
    im=Image.open(path);im.save(out/(name+'.png'))
def boot():
    global p
    p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba'),str(save)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('a'),text=True,bufsize=1)
    frames(90);check(state()[1]==0,'Boot title')
def close():
    global p
    if p is not None and p.poll() is None:
        p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10);assert p.returncode==0
    p=None
def menu():
    if state()[1]==0:press(8)
    elif state()[1] in (1,9,14,16):press(2)
    check(state()[1]==15,'Visible eight-button menu')
def choose(row):
    menu()
    for _ in range(9):
        if read(arcade)[17]==row:break
        press(128)
    press(1)
    check(state()[1]==(1 if row<2 else 9 if row==2 else 14 if row==3 else 11 if row==4 else 7 if row==5 else 16),'Correct visible mode entry')
def fighter(ch):
    if state()[1]==1:
        while info()[3]:press(64)
    elif state()[1]==9:
        while read(depth)[2]:press(4)
    for _ in range(13):
        if state()[2]==ch:break
        press(16)
    check(state()[2]==ch,'Select requested fighter')
def setup_quick(ch=0,stage=0,level=0):
    choose(0)
    while info()[3]!=1:press(128)
    while state()[4]!=level:press(32)
    press(128);check(info()[3]==2,'Arena row reachable with D-pad')
    while info()[1]!=stage:press(16)
    fighter(ch)
def play():
    press(1);check(state()[1]==2,'Battle countdown');frames(185);check(state()[1]==3,'Active native fight')
def change():
    if state()[1]==3:press(8)
    check(state()[1]==4,'Pause opens');press(128);press(128);press(1)
    check(state()[1] in (1,9),'Change returns to correct setup')
def approach(gap=22):
    for _ in range(240):
        v=state()
        if abs(v[13]-v[12])<=gap*256:break
        frames(1,16 if v[13]>v[12] else 32)
    frames(5)
def configure_practice(ch,stage,mode=0,rival=1):
    choose(2);fighter(ch)
    press(4)
    while read(depth)[3]!=mode:press(16)
    press(128)
    while read(depth)[4]!=rival:press(16)
    press(128);check(read(depth)[2]==3,'Practice arena row reachable')
    while info()[1]!=stage:press(16)
    fighter(ch)
def finish_or_retry():
    global match_count
    if state()[1]==5:frames(180)
    check(state()[1]==6,'Completed real regular round')
    match_count+=1;press(1);frames(185)
def followup_policy(target=10):
    for n in range(42000):
        v=state();m=info()
        if m[4]>=target:return
        if v[1] in (5,6):finish_or_retry();continue
        check(v[1]==3,'Follow-up replay remains playable') if n%2000==0 else None
        status=read(feel,'v')[14];keys=0;distance=abs(v[13]-v[12])//256
        if status in (7,8):keys=2
        elif distance>20 and v[16] in (0,1):keys=16 if v[13]>v[12] else 32
        elif v[16] in (0,1) and v[8]>=1800 and n%8==0:keys=1
        frames(1,keys)
    check(False,'Regular battles earn ten real follow-up hits')
def evade_policy():
    for n in range(36000):
        v=state();m=info()
        if m[5]>=1:return
        if v[1] in (5,6):finish_or_retry();continue
        if v[1]!=3:frames(1);continue
        keys=0;distance=abs(v[13]-v[12])//256
        if v[17]==5:
            began=read(depth)[28];age=v[5]-began
            arrival=30-age+max(0,distance-22)/5
            if 7<=arrival<=13 and v[14]>=120*256 and v[16] in (0,1):keys=64
        elif v[11]==100:
            if distance<76 and v[16] in (0,1):keys=32 if v[13]>v[12] else 16
        # Let the rival earn SP through ordinary clean hits, then evade its
        # real visible charge. No resource or gameplay-memory writes are used.
        frames(1,keys)
    check(False,'Evade a real regular-mode CPU super')

try:
    boot();capture('title-v09');choose(6);check(info()[4:8]==[0,0,0,0],'New mastery starts empty');capture('mastery-empty')
    press(1);check(state()[1]==14,'Mastery links to Records');press(1);check(state()[1]==16,'Records links to Mastery');press(2);capture('mode-menu-v09')
    # Three regular stages, live effects and frozen pause behavior.
    for stage in range(5):
        setup_quick(0,stage);capture('arena-selector-'+str(stage));play();check(info()[2]==stage,'Quick Battle uses selected arena')
        frames(30);press(2);frames(20);capture('arena-'+str(stage))
        press(8);frozen=state()[5:21];frames(60);check(state()[5:21]==frozen,'Pause freezes combat on each stage');change();press(2)
        arena_checks.append(dict(arena=stage,quick=True,pause=True))
    # Practice supports the same stage set and cannot farm mastery.
    for stage in range(5):
        configure_practice(0,stage);play();approach();frames(1,1);frames(1)
        for _ in range(50):
            if read(feel,'v')[14] in (7,8):break
            frames(1)
        check(read(feel,'v')[14] in (7,8),'Real practice follow-up available');frames(1,2);frames(80)
        check(read(depth)[14]>=1,'Real damaging practice follow-up');check(info()[4:6]==[0,0],'Practice does not award counters')
        capture('practice-'+str(stage));change();press(2);arena_checks[stage]['practice']=True
    configure_practice(0,2,2);play()
    while state()[5]<39:frames(1)
    frames(1,64);frames(65);check(state()[6]==95,'Actual practice super evaded');check(info()[4:6]==[0,0],'Practice super evasion earns no mastery');change();press(2);close()
    # Frozen v0.8 schema-2 bytes preserve all nine fighters and old medals.
    save.write_bytes((r/'tests/fixtures/nine_fighter_v08.sav').read_bytes());boot();check(read(audio)[1:3]==[0,0],'Legacy nine-fighter audio retained')
    choose(6);fighter(8);check(info()[4:8]==[0,0,1,1],'Legacy no-continue clear earns its existing medal');capture('mastery-migrated')
    press(1)
    while state()[4]!=2:press(128)
    check(read(arcade)[8:11]==[27,8,16200],'Last legacy fighter record retained');press(2)
    choose(4);press(1);press(2);close()
    raw=bytearray(save.read_bytes());check(struct.unpack_from('<I',raw,1028)[0]==4,'Migration writes schema 4')
    raw[1024+40]^=1;save.write_bytes(raw);boot();check(read(audio)[1:3]==[0,0],'Corrupt schema-4 falls back to valid schema-2 copy');close()
    # Earn a complete new medal through actual regular fights from zero.
    save.unlink();boot();setup_quick(0,1);play();followup_policy();check(info()[4]==10 and info()[7]>=1,'Ten resolved follow-ups earn a mastery medal');capture('followup-medal-earned')
    change();followups=info()[4];check(not info()[8],'Leaving fight saves earned mastery');press(2);choose(6);fighter(0);capture('mastery-followup-medal');close()
    boot();choose(6);check(info()[4]==followups and info()[7]>=1,'New mastery medal survives power cycle');press(2)
    setup_quick(3,2,1);play();evade_policy();evades=info()[5];check(evades>=1,'Actual CPU super evasion advances mastery');capture('real-super-evaded');change();close()
    boot();choose(6);fighter(3);check(info()[5]==evades,'Super-evasion counter survives power cycle');capture('mastery-evade-progress')
    check(not menu_keys&0x300,'All new menu paths use no shoulder shortcuts');check(missed==0,'Zero missed native display frames')
    report=dict(status='passed',checks=checks,arenas=arena_checks,mastery_menu=True,regular_followup_hits=followups,regular_super_evades=evades,completed_medal=True,practice_excluded=True,schema2_migration=True,mixed_schema_corruption_recovery=True,mastery_power_cycle=True,save_schema=4,save_copy_bytes=596,menu_keys=menu_keys,no_shoulder_required_in_menus=True,regular_matches_completed=match_count,peak_cpu_fraction=peak/4096,missed_display_frames=missed,rom_sha256=hashlib.sha256((r/'out/PokemonBattleArena.gba').read_bytes()).hexdigest(),wall_seconds=round(time.time()-started,2),method='mGBA 0.10.2 real button inputs and read-only telemetry; native captures and SRAM power cycles; frozen legacy fixture for migration, new mastery earned from zero in regular matches; no gameplay-memory writes')
    (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
finally:close()
