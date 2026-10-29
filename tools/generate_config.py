"""Generate immutable C++ combat data from the plan's canonical JSON."""
from pathlib import Path
import json
r=Path(__file__).resolve().parents[1];j=json.loads((r/'config/balance.json').read_text())
chars=j['characters'];assert len(chars)==6 and len({c['id'] for c in chars})==6
out=['#pragma once','#include <stdint.h>','namespace pkmn {',
'struct CharacterDef { const char* name; int hp,sta,regen,speed,melee_damage,fire_damage,super_damage,melee_start,melee_active,melee_total,fire_start,fire_total,shot_speed; };',
'inline constexpr CharacterDef characters[6] = {']
for c in chars:
 m=c['melee'];f=c['fire'];v=[c['max_hp'],c['max_stamina'],c['stamina_regen_per_second'],c['walk_speed_q8'],m['damage'],f['damage'],c['super']['damage'],m['startup_ticks'],m['active_ticks'],sum(m[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['startup_ticks'],sum(f[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['speed_q8']]
 out.append('{"'+c['name'].upper()+'",'+','.join(map(str,v))+'},')
out+=['};']
for group in ['world','resources','motion','reactions','input','match']:
 for name,value in j[group].items():
  if isinstance(value,(int,bool)):out.append(f'inline constexpr int {group}_{name} = {int(value)};')
for action,v in j['actions'].items():
 for name,value in v.items():
  if isinstance(value,(int,bool)):out.append(f'inline constexpr int {action}_{name} = {int(value)};')
for name,v in j['collision'].items():
 if isinstance(v,int):out.append(f'inline constexpr int collision_{name} = {v};')
 else:out.append(f'inline constexpr int collision_{name}[4] = {{{",".join(map(str,v))}}};')
out.append('}')
(r/'include/pkmn_character_defs.h').write_text('\n'.join(out)+'\n')
print('Generated six character definitions and shared rules.')
