"""Generate immutable C++ combat data from the plan's canonical JSON."""
from pathlib import Path
import json
r=Path(__file__).resolve().parents[1];j=json.loads((r/'config/balance.json').read_text())
chars=j['characters'];assert len(chars)==13 and len({c['id'] for c in chars})==13
count=len(chars)
technique_ids={'none':0,'air_brake':1,'shadow_burst':2,'volt_step':3,'shell_roll':4}
techniques=j['techniques']
assert set(techniques)=={'air_brake','shadow_burst','volt_step','shell_roll'}
limits={'air_brake':{'stamina_cost':(4,16),'duration_ticks':(6,18),'max_fall_speed_q8':(64,256)},
        'shadow_burst':{'stamina_cost':(4,16),'startup_ticks':(8,18),'active_ticks':(2,6),'damage':(1,12)},
        'volt_step':{'stamina_cost':(8,20),'double_tap_ticks':(6,12),'startup_ticks':(1,4),'active_ticks':(4,8),'recovery_ticks':(6,14),'speed_q8':(512,1280),'cooldown_ticks':(18,40)},
        'shell_roll':{'stamina_cost':(4,12),'motion_ticks':(6,12),'speed_q8':(256,768)}}
for name,bounds in limits.items():
 for field,(lo,hi) in bounds.items():
  value=techniques[name][field];assert type(value) is int and lo<=value<=hi,(name,field)
box=techniques['shadow_burst']['hitbox']
assert len(box)==4 and all(type(v) is int for v in box)
assert -15<=box[0]<0<box[2]<=15 and -12<=box[1]<0<box[3]<=12
tech=['#pragma once','namespace pkmn {',
      'enum Technique { TECHNIQUE_NONE, TECHNIQUE_AIR_BRAKE, TECHNIQUE_SHADOW_BURST, TECHNIQUE_VOLT_STEP, TECHNIQUE_SHELL_ROLL };']
for name,values in techniques.items():
 for field,value in values.items():
  if isinstance(value,int):tech.append(f'inline constexpr int {name}_{field}={value};')
  else:tech.append(f'inline constexpr int {name}_{field}[4]={{'+','.join(map(str,value))+'};')
tech.append('}')
(r/'include/pkmn_technique_defs.h').write_text('\n'.join(tech)+'\n')
out=['#pragma once','#include <stdint.h>','#include "pkmn_technique_defs.h"','namespace pkmn {',
'struct CharacterDef { const char* name; int hp,sta,regen,speed,melee_damage,fire_damage,super_damage,melee_start,melee_active,melee_total,fire_start,fire_total,shot_speed; int melee_reach,melee_lunge,guard_percent,fire_guard_percent,fire_knockback,punish_bonus; int melee_guard_percent,gravity_percent,jump_percent; bool melee_followup; int technique; const char* role; const char* trait; const char* tip1; const char* tip2; };',
f'inline constexpr int character_count = {count};',
f'inline constexpr CharacterDef characters[{count}] = {{']
for c in chars:
 m=c['melee'];f=c['fire'];v=[c['max_hp'],c['max_stamina'],c['stamina_regen_per_second'],c['walk_speed_q8'],m['damage'],f['damage'],c['super']['damage'],m['startup_ticks'],m['active_ticks'],sum(m[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['startup_ticks'],sum(f[k] for k in ['startup_ticks','active_ticks','recovery_ticks']),f['speed_q8']]
 s=c['style'];bounds={'melee_reach_px':(16,40),'melee_lunge_q8':(0,192),'guard_impact_percent':(50,125),'fire_guard_percent':(100,150),'fire_knockback_q8':(256,896),'recovery_punish_bonus':(0,4),'melee_guard_percent':(100,150),'gravity_percent':(70,110),'jump_percent':(90,110)}
 for name,(lo,hi) in bounds.items():assert isinstance(s[name],int) and lo<=s[name]<=hi,(c['id'],name)
 assert isinstance(s['melee_followup'],bool)
 for name in ['role','trait_name','tip_1','tip_2']:assert s[name].isascii() and len(s[name])<=30
 v.extend(s[name] for name in bounds);v.append(int(s['melee_followup']))
 assert s['technique'] in technique_ids
 v.append(technique_ids[s['technique']])
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
print('Generated thirteen character definitions and shared rules.')

# CPU tuning is separate from the combat rules: no difficulty stat modifiers.
cpu=json.loads((r/'config/cpu.json').read_text())
assert cpu['version']==j['ruleset_version']
assert cpu['history_ticks']==32 and cpu['memory_actions']==6
assert len(cpu['difficulties'])==3 and len(cpu['styles'])==count
fields=['reaction_ticks','think_ticks','defense_percent','super_evade_percent','punish_percent','followup_percent','mistake_percent','hesitate_ticks','plan_ticks','adapt_percent']
style=['distance_px','melee_percent','fire_percent','guard_bonus','punish_bonus','recover_below','resume_at']
lines=['#pragma once','namespace pkmn {']
assert type(cpu['signature_zone_burst_percent']) is int and 0<=cpu['signature_zone_burst_percent']<=25
for k in ['history_ticks','memory_actions','memory_expire_ticks','signature_zone_burst_percent']:lines.append(f'inline constexpr int cpu_{k} = {cpu[k]};')
lines+=['struct CpuDifficulty { int '+','.join(fields)+'; };','inline constexpr CpuDifficulty cpu_difficulties[3] = {']
for d in cpu['difficulties']:
 assert 0<d['reaction_ticks']<cpu['history_ticks'] and 1<=d['think_ticks']<=12
 assert 1<=d['hesitate_ticks']<=30 and 30<=d['plan_ticks']<=180
 for k in fields:
  if k.endswith('_percent'):assert 0<=d[k]<=100
 lines.append('{'+','.join(str(d[k]) for k in fields)+'},')
lines+=['};','struct CpuStyle { int '+','.join(style)+'; };',f'inline constexpr CpuStyle cpu_styles[{count}] = {{']
for c,s in zip(chars,cpu['styles']):
 assert s['character']==c['id'] and 16<=s['distance_px']<=100
 assert 12<=s['recover_below']<s['resume_at']<=60
 for k in ['melee_percent','fire_percent','guard_bonus','punish_bonus']:assert 0<=s[k]<=100
 lines.append('{'+','.join(str(s[k]) for k in style)+'},')
lines+=['};','}']
(r/'include/pkmn_cpu_defs.h').write_text('\n'.join(lines)+'\n')
print('Generated three difficulty profiles and thirteen CPU styles.')

# One source of truth for the native setup geometry and asset painter.
ui=json.loads((r/'config/selection.json').read_text())
assert ui['columns']==5 and ui['grid_x']+(ui['columns']-1)*ui['step_x']+ui['card_width']<=232
assert ui['grid_y']+((len(chars)-1)//ui['columns'])*ui['step_y']+ui['card_height']<=102
lines=['#pragma once','namespace pkmn {']
for key,value in ui.items():
 if isinstance(value,int):lines.append(f'inline constexpr int selection_{key}={value};')
 else:lines.append(f'inline constexpr int selection_{key}[{len(value)}]={{'+','.join(map(str,value))+'};')
lines+=['inline int selection_row_y(bool training,int row){return training?selection_practice_rows[row]:selection_quick_rows[row];}','}']
(r/'include/pkmn_selection_layout.h').write_text('\n'.join(lines)+'\n')
print('Generated single-screen selection layout.')

# Authored Cup routes are shared by the native game and the asset painter.
cups=json.loads((r/'config/cups.json').read_text())
assert len(cups)==3
ids=[c['id'] for c in chars]
lines=['#pragma once','namespace pkmn {',
       'inline constexpr int cup_count=3,cup_rivals=3;',
       'struct CupRival { const char* name; int fighter,arena; const char* cue; };',
       'struct CupDefinition { const char* name; const char* goal; int target; CupRival rivals[cup_rivals]; };',
       'inline constexpr CupDefinition cups[cup_count] = {']
for cup in cups:
 assert len(cup['rivals'])==3 and 1<=cup['target']<=9 and len(cup['goal'])<=25
 lines.append('{'+json.dumps(cup['name'])+','+json.dumps(cup['goal'])+','+str(cup['target'])+',{')
 for rival in cup['rivals']:
  assert rival['fighter'] in ids and 0<=rival['arena']<5 and len(rival['cue'])<=32 and len(rival['name'])<=8
  lines.append('{'+json.dumps(rival['name'])+','+str(ids.index(rival['fighter']))+','+str(rival['arena'])+','+json.dumps(rival['cue'])+'},')
 lines.append('}},')
lines+=['};','}']
(r/'include/pkmn_cup_defs.h').write_text('\n'.join(lines)+'\n')
print('Generated three authored Rival Cups.')
