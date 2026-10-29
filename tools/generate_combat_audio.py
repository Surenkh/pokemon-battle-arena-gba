"""Deterministic, original short PCM effects for the six combat identities."""
from pathlib import Path
import json, math, random, struct, wave, hashlib

ROOT = Path(__file__).resolve().parents[1]
RATE = 16384
KINDS = {'shot': .115, 'hit': .100, 'charge': .125, 'release': .240}

def synth(theme, kind, duration):
    rng = random.Random(7103 + theme*137 + list(KINDS).index(kind)*29)
    values=[]; low=0.; phase=0.
    for n in range(round(RATE*duration)):
        t=n/RATE; x=t/duration; noise=rng.uniform(-1,1)
        low += .16*(noise-low)
        pitch=(.65 if kind=='hit' else .8 if kind=='release' else 1.)*(1+.7*x if kind=='charge' else 1.-.35*x)
        phase += 2*math.pi*[1350,125,920,1800,740,1300][theme]*pitch/RATE
        if theme==0:
            gate=[1,.3,.8,.5][int(t*180)%4]
            v=(.30*(1 if math.sin(phase)>0 else -1)+.55*(noise-low))*gate
        elif theme==1:
            v=low*2.1+.16*math.sin(phase)
        elif theme==2:
            v=.4*math.sin(phase+1.2*math.sin(2*math.pi*28*t))+.6*low
        elif theme==3:
            v=(noise-low)*(.35+.2*math.sin(2*math.pi*42*t))+.08*math.sin(phase)
        elif theme==4:
            v=.26*math.sin(phase)+.18*math.sin(phase*1.25)+.12*math.sin(phase*1.5)
        else:
            burst=math.exp(-((t*29)%1)*6)
            v=((noise-low)*.7+.22*math.sin(phase))*burst
        if kind=='release':v=.75*v+.2*math.sin(2*math.pi*80*t)*math.exp(-t*12)
        envelope=min(1,t/.004,(duration-t)/.018)
        if kind in ('shot','hit'):envelope*=1-.55*x
        values.append(round(max(-.72,min(.72,v*envelope))*32767))
    return values

def save(name, values):
    target=ROOT/'audio'/f'{name}.wav'
    with wave.open(str(target),'wb') as w:
        w.setparams((1,2,RATE,0,'NONE','not compressed'))
        w.writeframes(struct.pack('<'+'h'*len(values),*values))
    (ROOT/'content'/f'sound_{name}.json').write_text(json.dumps(dict(id=name,type='sound',name=name,path=f'audio/{name}.wav',format='wav'),indent=2)+'\n')
    return dict(name=name,samples=len(values),rate=RATE,peak=max(abs(v) for v in values),sha256=hashlib.sha256(target.read_bytes()).hexdigest())

report=[]
for character in range(6):
    for kind, duration in KINDS.items():
        report.append(save(f'{kind}_fx_{character}',synth(character,kind,duration)))
for name,theme,kind,duration in [('guard_break',5,'release',.18),('no_stamina',1,'hit',.08),('power_ready',4,'shot',.12)]:
    report.append(save(name,synth(theme,kind,duration)))
(ROOT/'docs'/'combat_audio_assets.json').write_text(json.dumps(dict(generator='tools/generate_combat_audio.py',provenance='Original deterministic synthesis; no external recordings',assets=report),indent=2)+'\n')
print(f'Generated {len(report)} original mono PCM combat effects.')
