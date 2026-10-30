"""Native feedback/audio checks using real button inputs and read-only telemetry."""
from pathlib import Path
import array, json, math, os, re, subprocess, time, wave
from PIL import Image

r=Path(__file__).resolve().parents[1]
out=r/'tests/native_artifacts/feel';out.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['readelf','-sW',str(r/'.gbastudio/PokemonBattleArena.elf')],text=True)
def symbol(name,size):
    return int(re.search(r'\b([0-9a-f]{8})\s+'+str(size)+r'\s+OBJECT\s+GLOBAL[^\n]+\b'+name+r'\b',symbols).group(1),16)
main=symbol('pkmn_debug',128);feel=symbol('pkmn_feel_debug',64)
p=subprocess.Popen([os.environ.get('PKMN_MGBA_DRIVER',str(r/'tests/mgba_driver')),str(r/'out/PokemonBattleArena.gba')],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(out/'errors.log').open('w'),text=True,bufsize=1)
checks=0;reports=[];started=time.time()
def cmd(s):
    p.stdin.write(s+'\n');p.stdin.flush();line=p.stdout.readline().strip()
    if not line:raise RuntimeError('Emulator exited')
    return line
def frames(n,k=0):cmd(f'f {n} {k}')
def press(k):frames(2,k);frames(2)
def state():return list(map(int,cmd(f's {main:08x}').split()[1:]))
def feedback():return list(map(int,cmd(f'v {feel:08x}').split()[1:]))
def dump(name):cmd('d '+str(out/(name+'.ppm')))
def world_translation_match(a,b):
    # Frozen fighters/effects and scenery must share one translation. An
    # opposite BG scroll would move most pixels but shear actors off the world.
    best=0
    for dy in range(-2,3):
        for dx in range(-6,7):
            if dx==dy==0:continue
            left=a.crop((8,67,232,135)).tobytes()
            right=b.crop((8+dx,67+dy,232+dx,135+dy)).tobytes()
            same=sum(left[i:i+3]==right[i:i+3] for i in range(0,len(left),3))
            best=max(best,same/(len(left)//3))
    return best
def check(value,message):
    global checks
    checks+=1
    if not value:
        dump('failure')
        raise AssertionError(message+' '+str(state())+' '+str(feedback()))
def audio_stats(path):
    with wave.open(str(path)) as w:
        check(w.getnchannels()==2 and w.getsampwidth()==2,'Native stereo PCM')
        data=array.array('h',w.readframes(w.getnframes()));rate=w.getframerate()
    check(bool(data) and rate==32768,'Native capture has samples at selected rate')
    rms=math.sqrt(sum(v*v for v in data)/len(data));peak=max(abs(v) for v in data)
    check(rms>10,'Native audio is not silent')
    check(sum(abs(v)>=32760 for v in data)/len(data)<.001,'No sustained mixer clipping')
    return dict(samples=len(data)//2,rate=rate,rms=round(rms,2),peak=peak)

try:
    frames(90);press(8)
    # Audio-bank coverage needs a repeatable opportunity to earn and release
    # every super. CPU tactics/difficulty are stressed separately by native_cpu_flow.
    # Choose Easy through the real selector, without altering combat resources.
    press(128);press(128);press(32);press(64)
    check(state()[4]==0,'Audio coverage uses the actual Easy setting')
    all_events=all_audio=all_charge=all_release=0
    for ch,name in enumerate(['pikachu','charmander','squirtle','bulbasaur','eevee','meowth']):
        while state()[2]!=ch:press(16)
        own_release=False
        for attempt in range(5):
            press(1);frames(185)
            recording=out/f'{name}_{attempt+1}.wav';cmd('o '+str(recording))
            frames(1,2)
            for _ in range(24):
                if feedback()[4]:break
                frames(1)
            press(8);before=state();quiet=feedback()
            check(before[1]==4 and quiet[4]==0,'Pause stops combat voices')
            frames(60)
            check(state()[5:21]==before[5:21],'Pause freezes simulation')
            after_feedback=feedback()
            check(after_feedback[:13]+after_feedback[14:]==quiet[:13]+quiet[14:],'Pause freezes feedback state')
            press(8);frames(65)
            line=cmd(f'b 8000 {main:08x} {feel:08x}')
            metrics={key:int(v) for key,v in re.findall(r'(\w+)=(\d+)',line)}
            cmd('c')
            check(state()[1]==6,'Feedback stress round completes')
            check(metrics['voices']<=2,'Two-voice combat mixer bound')
            check(metrics['shake']<=3,'Camera amplitude bounded')
            check(metrics['audio_themes']&(1<<ch),'Player elemental sound bank used')
            check(state()[22]==0,'No missed native display frames')
            all_events|=metrics['events'];all_audio|=metrics['audio_themes'];all_charge|=metrics['charge_themes'];all_release|=metrics['release_themes']
            reports.append(dict(character=name,attempt=attempt+1,**metrics,audio=audio_stats(recording)))
            own_release=bool(metrics['release_themes']&(1<<ch))
            press(16);press(1)
            if own_release:break
        check(own_release,'Player earned and released their own themed super')
    check(all_audio==all_charge==all_release==63,'All six shot, charge and release banks played natively')

    # Read-only visual proof: during hitstop the world moves but the fixed HUD does not.
    hud_checked=False;charge_paused=False
    # Reuse the driver's stamina-aware policy one frame at a time so we can
    # intercept a naturally earned super before its 30-tick startup ends.
    # A single round need not award a super (a fighter can lose first).
    for attempt in range(5):
        press(1);frames(185)
        for n in range(6000):
            v=state();f=feedback()
            if v[1]!=3:break
            if not hud_checked and v[19]>=3 and (f[2] or f[3]):
                # The displayed framebuffer trails telemetry by a frame. Settle
                # the hit's resource update before comparing two frozen HUDs.
                frames(1);tick=state()[5];timer=state()[18];dump('hud_a');frames(1);after=state();dump('hud_b')
                check(after[5]==tick and after[18]==timer,'Hitstop retains simulation/timer freeze')
                a=Image.open(out/'hud_a.ppm');b=Image.open(out/'hud_b.ppm')
                check(a.crop((0,0,240,41)).tobytes()==b.crop((0,0,240,41)).tobytes(),'HUD pixels fixed during world shake')
                check(a.crop((0,65,240,140)).tobytes()!=b.crop((0,65,240,140)).tobytes(),'World pixels actually move')
                check(world_translation_match(a,b)>.999,'Background and frozen fighters shake together')
                hud_checked=True;v=state();f=feedback()
            if not charge_paused and (v[16]==5 or v[17]==5) and f[12]>0:
                dump('super_charge');press(8);frozen=state();check(feedback()[12]==0,'Pause stops charging audio');frames(45);check(state()[5:21]==frozen[5:21],'Charging pause retains combat state');press(8);frames(65);charge_paused=True
                v=state()
            if hud_checked and charge_paused:break
            keys=0;distance=abs(v[13]-v[12])//256
            if n%6==0:
                if v[10]==100 and v[8]<1500:keys=32 if v[13]>v[12] else 16
                elif v[10]==100 and v[8]>=1500 and v[14]>=120*256:keys=256
                elif distance<27 and v[8]>=480:keys=1
                elif distance>48 and v[8]>=1800 and n%18==0:keys=2
                elif distance>24:keys=16 if v[13]>v[12] else 32
            elif distance>24 and v[16]==0:keys=16 if v[13]>v[12] else 32
            frames(1,keys)
        if hud_checked and charge_paused:break
        check(state()[1]==6,'Charge-search round completes')
        press(16);press(1)
    check(hud_checked,'Native fixed-HUD shake case exercised')
    check(charge_paused,'Native charging pause case exercised')

    # Blocking until stamina depletes exercises guard break without memory writes.
    if state()[1]==3:press(8);press(128);press(128);press(1)
    elif state()[1]==6:press(16);press(1)
    while state()[2]!=2:press(16)
    press(1);frames(185);guard=False;tired=False
    for n in range(700):
        if state()[1]!=3:break
        frames(1,512)
        if feedback()[8]:guard=True;dump('guard_break');break
    check(guard,'Guard naturally breaks under pressure')
    for n in range(90):
        if state()[1]!=3:break
        frames(1,2 if n%3==0 else 0)
        if feedback()[9]:tired=True;dump('low_stamina');break
    check(tired,'Insufficient stamina produces feedback')
    check(state()[22]==0,'No missed frames through final guard/exhaustion checks')
    result=dict(status='passed',checks=checks,difficulty='Easy (audio opportunity coverage); all levels covered by native_cpu_flow',characters=reports,events=all_events,audio_theme_mask=all_audio,charge_theme_mask=all_charge,release_theme_mask=all_release,hud_fixed=True,charge_pause=True,guard_break=True,exhaustion=True,peak_cpu_fraction=state()[21]/4096,missed_display_frames=state()[22],wall_seconds=round(time.time()-started,2),method='mGBA 0.10.2, physical inputs, read-only telemetry and actual native audio capture; no gameplay-memory writes')
    (out/'report.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='characters'},indent=2))
finally:
    if p.poll() is None:
        cmd('c');p.stdin.write('q\n');p.stdin.flush();p.wait(timeout=10)
