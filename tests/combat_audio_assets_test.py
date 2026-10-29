"""Validate the actual PCM and content imports, not just generator settings."""
from pathlib import Path
import array, hashlib, json, math, wave

r=Path(__file__).resolve().parents[1]
meta=json.loads((r/'docs/combat_audio_assets.json').read_text())
assert len(meta['assets'])==27
groups={kind:set() for kind in ['shot','hit','charge','release']}
for item in meta['assets']:
    path=r/'audio'/(item['name']+'.wav')
    assert hashlib.sha256(path.read_bytes()).hexdigest()==item['sha256']
    with wave.open(str(path)) as w:
        assert (w.getnchannels(),w.getsampwidth(),w.getframerate())==(1,2,16384)
        assert w.getnframes()==item['samples']
        data=w.readframes(w.getnframes())
    samples=array.array('h',data)
    assert samples and max(abs(v) for v in samples)<24000
    assert math.sqrt(sum(v*v for v in samples)/len(samples))>300
    assert abs(sum(samples)/len(samples))<1200
    assert samples[0]==0 and abs(samples[-1])<200
    info=json.loads((r/'content'/('sound_'+item['name']+'.json')).read_text())
    assert info['type']=='sound' and (r/info['path']).is_file()
    for kind in groups:
        if item['name'].startswith(kind+'_fx_'):groups[kind].add(hashlib.sha256(data).hexdigest())
assert all(len(hashes)==6 for hashes in groups.values())
print('PASS 27 original PCM effects: non-silent, bounded, ramped, importable; six distinct themes per bank.')
