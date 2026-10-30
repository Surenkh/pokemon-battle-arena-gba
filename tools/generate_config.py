"""Generate immutable C++ combat data from the plan's canonical JSON."""
from pathlib import Path
import json
r=Path(__file__).resolve().parents[1];j=json.loads((r/'config/balance.json').read_text())
chars=j['characters'];assert len(chars)==6 and len({c['id'] for c in chars})==6
out=['#pragma once','#include <stdint.h>','namespace pkmn {',
'struct CharacterDef { const char* name; int hp,sta,regen,speed,melee_damage,fire_damage,super_damage,melee_start,melee_active,melee_total,fire_start,fire_total,shot_speed; int melee_reach,melee_lunge,guard_percent,fire_guard_percent,fire_knockback,punish_bonus; bool melee_followup; const char* role; const char* trait; const char* tip1; const char* tip2; };',
'inline constexpr CharacterDef characters[6] = {']
for c in chars:
 m=c['melee'];f=c['fire'];v=[c['max_hp'],c['max_stamina'],c['stamina_regen_per_second'],c['walk_speed_q8'],m['damage'],f['damage'],c['super']['damage'],m['startup_ticks'],m['active_ticks'],sum(m[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['startup_ticks'],sum(f[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['speed_q8']]
 s=c['style'];bounds={'melee_reach_px':(16,40),'melee_lunge_q8':(0,192),'guard_impact_percent':(50,125),'fire_guard_percent':(100,150),'fire_knockback_q8':(256,896),'recovery_punish_bonus':(0,4)}
 for name,(lo,hi) in bounds.items():assert isinstance(s[name],int) and lo<=s[name]<=hi,(c['id'],name)
 assert isinstance(s['melee_followup'],bool)
 for name in ['role','trait_name','tip_1','tip_2']:assert s[name].isascii() and len(s[name])<=30
 v.extend(s[name] for name in bounds);v.append(int(s['melee_followup']))
 strings=[s[k] for k in ['role','trait_name','tip_1','tip_2']]
 out.append('{'+json.dumps(c['name'].upper())+','+','.join(map(str,v))+','+','.join(json.dumps(s) for s in strings)+'},')
out+=['};']
for group in ['world','resources','motion','reactions','input','match','followup','practice']:
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
