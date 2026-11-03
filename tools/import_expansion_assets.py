"""Import the four additions from the existing pinned, credited source."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from urllib.request import urlopen
import hashlib,json,xml.etree.ElementTree as ET
r=Path(__file__).resolve().parents[1];dest=r/'assets/source/animations';rev=(dest/'UPSTREAM_REVISION').read_text().strip()
choices={'Idle':['Idle'],'Walk':['Walk'],'Attack':['Attack'],'Shoot':['Shoot','SpAttack','Strike'],'Hop':['Hop'],'Charge':['Charge'],'Cringe':['Cringe','Hurt'],'Hurt':['Hurt'],'Sleep':['Sleep'],'Sit':['Sit','Idle'],'Pose':['Pose','Idle'],'Faint':['Faint','Hurt'],'Laying':['Laying','Sleep']}
manifest=json.loads((dest/'source_manifest.json').read_text());provenance=[]
def get(url):
 with urlopen(url,timeout=30) as response:return response.read()
for name,number in [('vulpix','0037'),('gastly','0092'),('riolu','0447'),('totodile','0158')]:
 directory=dest/name;directory.mkdir(exist_ok=True);base=f'https://raw.githubusercontent.com/PMDCollab/SpriteCollab/{rev}/sprite/{number}/';xml=get(base+'AnimData.xml');(directory/'AnimData.xml').write_bytes(xml)
 animations={v.findtext('Name'):v for v in ET.fromstring(xml).find('Anims')}
 def resolve(source):
  item=animations[source]
  if item.findtext('CopyOf'):return resolve(item.findtext('CopyOf'))
  return dict(source=source,width=int(item.findtext('FrameWidth')),height=int(item.findtext('FrameHeight')),durations=[int(v.text) for v in item.find('Durations')],hit=int(item.findtext('HitFrame') or 0),**{'return':int(item.findtext('ReturnFrame') or 0)})
 mapped={key:resolve(next(v for v in values if v in animations)) for key,values in choices.items()};files=sorted({'credits.txt'}|{v['source']+suffix for v in mapped.values() for suffix in ['-Anim.png','-Offsets.png']})
 def save(file):
  data=get(base+file);(directory/file).write_bytes(data);return dict(file=file,sha256=hashlib.sha256(data).hexdigest(),bytes=len(data))
 with ThreadPoolExecutor(max_workers=6) as pool:entries=list(pool.map(save,files))
 manifest[name]=mapped;provenance.append(dict(character=name,upstream='sprite/'+number,revision=rev,files=[dict(file='AnimData.xml',sha256=hashlib.sha256(xml).hexdigest(),bytes=len(xml))]+entries));print(name,len(entries),'source files',flush=True)
(dest/'source_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n');(dest/'expansion_sources.json').write_text(json.dumps(provenance,indent=2)+'\n')
