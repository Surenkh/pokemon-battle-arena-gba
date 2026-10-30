#include "pkmn_combat_core.h"
namespace pkmn {
static int minv(int a,int b){return a<b?a:b;}static int maxv(int a,int b){return a>b?a:b;}static int absv(int a){return a<0?-a:a;}static int clamp(int n,int a,int b){return maxv(a,minv(n,b));}
static int approach(int value,int target,int amount){return value<target?minv(value+amount,target):maxv(value-amount,target);}
struct Rect{int l,t,r,b;};
static bool overlaps(Rect a,Rect b){return a.l<b.r&&a.r>b.l&&a.t<b.b&&a.b>b.t;}
static Rect box(int x,int y,const int* b){return{x+b[0]*256,y+b[1]*256,x+b[2]*256,y+b[3]*256};}
struct Fraction {int n,d;};
static bool less(Fraction a,Fraction b){return int64_t(a.n)*b.d<int64_t(b.n)*a.d;}
// Relative swept AABB with strict overlap, rational contact ordering.
static bool sweep(Rect a,int dx,int dy,Rect b,int bx,int by,Fraction& time){
 Fraction enter{0,1},exit{1,1};int v[2]={dx-bx,dy-by},al[2]={a.l,a.t},ar[2]={a.r,a.b},bl[2]={b.l,b.t},br[2]={b.r,b.b};
 for(int axis=0;axis<2;axis++){if(!v[axis]){if(al[axis]>=br[axis]||ar[axis]<=bl[axis])return false;continue;}
  Fraction en,ex;if(v[axis]>0){en={bl[axis]-ar[axis],v[axis]};ex={br[axis]-al[axis],v[axis]};}else{en={al[axis]-br[axis],-v[axis]};ex={ar[axis]-bl[axis],-v[axis]};}
  if(less(enter,en)){enter=en;}
  if(less(ex,exit)){exit=ex;}
 }if(!less(enter,exit)||!less(enter,{1,1})||!less({0,1},exit))return false;time=enter;return true;
}
static int priority(int v){if(v==R)return 5;if(v==UP)return 4;if(v==DOWN)return 3;if(v==B)return 2;if(v==A)return 1;return 0;}
void CombatCore::emit(EventKind kind,int owner,int target,bool sup,int damage,bool followup,uint32_t attack){
 if(event_count>=max_events)__builtin_trap();
 const auto&v=f[target];int x=v.x,y=v.y-18*256;if(kind==EV_SHOT||kind==EV_RELEASE){const auto&b=p[owner+(kind==EV_RELEASE?2:0)];x=b.x;y=b.y;}events[event_count++]={kind,owner,target,x,y,sup,attack?attack:f[owner].attack_id,damage,followup};
}
bool CombatCore::recovering(int who)const{
 const auto&v=f[who];const auto&d=characters[v.character];
 if(tick>=v.ends||!v.hp)return false;
 return v.action==MELEE?age(v)>=d.melee_start+d.melee_active:v.action==FIRE?age(v)>=d.fire_start+1:v.action==SUPER?age(v)>=super_startup_ticks+super_active_ticks:false;
}
bool CombatCore::can_followup(int who,int button,int ahead)const{
 const auto&v=f[who];const auto&d=characters[v.character];
 if(ahead<0||ahead>=input_buffer_ticks||v.action!=MELEE||v.chain_depth||v.confirmed_at<0||!grounded(v)||v.vy<0||tick+ahead>=v.ends)return false;
 if(button!=B&&(button!=A||!d.melee_followup))return false;
 if(v.sta<(button==B?fire_stamina_cost:melee_stamina_cost)*60)return false;
 if(button==B&&(p[who].active||p[who].reserved))return false;
 const int startup=button==B?d.fire_start:d.melee_start;
 return recovering(who)&&tick+ahead>=v.ends-followup_recovery_cut_ticks&&tick+ahead+startup>=v.confirmed_at+reactions_normal_hitstun_ticks+followup_escape_ticks;
}
// Existing three-tick queue: accept a descending jump only if touchdown occurs
// in time to consume it before expiry. Jump height, cost and airborne motion stay unchanged.
bool CombatCore::jump_buffer_eligible(const Fighter&v)const{
 if(grounded(v))return true;if(v.vy<=0)return false;
 int y=v.y,vy=v.vy;for(int n=0;n<input_buffer_ticks-1;n++){vy=minv(motion_max_fall_velocity_q8,vy+motion_gravity_q8);y+=vy;if(y>=world_floor_y*256)return true;}return false;
}
void CombatCore::reset(int player,int cpu,uint32_t match_seed){training=false;event_count=0;event_frame=0;tick=0;remaining=match_round_ticks;hitstop=outcome=feedback=0;next_id=1;seed=match_seed?match_seed:1;for(auto&v:p)v=Projectile{};for(int i=0;i<2;i++){f[i]=Fighter{};auto&v=f[i];v.character=i?cpu:player;v.x=(i?world_cpu_spawn_x:world_player_spawn_x)*256;v.hp=characters[v.character].hp;v.sta=characters[v.character].sta*resources_stamina_subunits_per_point;v.face=i?-1:1;}}
void CombatCore::buffer(int who,Input input){auto&v=f[who];const int keys[5]={R,UP,DOWN,B,A};for(int k:keys)if(input.pressed&k){int cost=k==R?super_stamina_cost:k==UP?jump_stamina_cost:k==DOWN?squeeze_stamina_cost:k==B?fire_stamina_cost:melee_stamina_cost;
 if((k==B&&(p[who].active||p[who].reserved))||(k==R&&(p[who+2].active||p[who+2].reserved||v.action==BLOCK))||(k==R&&(v.sp<100||!grounded(v)))||(k==DOWN&&(!grounded(v)||tick<v.squeeze_ready))||(k==UP&&!jump_buffer_eligible(v)))continue;
 if(v.sta<cost*60){emit(EV_TIRED,who,who);continue;}
 if(!v.pending||tick>=v.pending_until||priority(k)>=priority(v.pending)){v.pending=k;v.pending_until=tick+input_buffer_ticks;}break;}}
void CombatCore::frame(Input player,Input cpu){if(outcome){begin_events();return;}if(hitstop){begin_events();buffer(0,player);buffer(1,cpu);--hitstop;feedback=0;return;}step(player,cpu);}
void CombatCore::cancel(int who){auto&v=f[who];if(v.action==SQUEEZE)v.squeeze_ready=tick+squeeze_cooldown_after_end_ticks;for(auto&b:p)if(b.owner==who&&b.reserved)b.reserved=false;}
void CombatCore::enter(int who,Action a,int duration,int cost,bool followup){auto&v=f[who];v.action=a;v.began=tick;v.ends=tick+duration;v.sta-=cost*60;v.last_spend=tick;v.attack_id=next_id++;v.melee_contact=false;v.spawned=false;v.confirmed_at=-100;v.chain_depth=followup?1:0;}
void CombatCore::action(int who,Input input){auto&v=f[who];const auto&d=characters[v.character];bool ground=grounded(v);
 if(v.action!=NONE&&v.action!=BLOCK&&tick>=v.ends){if(v.action==SQUEEZE)v.squeeze_ready=tick+squeeze_cooldown_after_end_ticks;v.action=NONE;}
 if(v.action==BLOCK&&!(input.held&L))v.action=NONE;
 if(v.action==NONE||v.action==BLOCK){int delta=f[1-who].x-v.x;if(delta)v.face=delta>0?1:-1;}
 buffer(who,input);if(tick>=v.pending_until)v.pending=0;
 int request=v.pending;bool neutral=v.action==NONE,defensive=neutral||v.action==BLOCK;
 bool followup=can_followup(who,request);
 bool can=request==R?(neutral&&ground&&v.sp==100&&!p[who+2].active&&!p[who+2].reserved):request==UP?(defensive&&ground):request==DOWN?(defensive&&ground&&tick>=v.squeeze_ready):request==B?((neutral||followup)&&!p[who].active&&!p[who].reserved):request==A?(neutral||followup):false;
 if(can){int cost=request==R?super_stamina_cost:request==UP?jump_stamina_cost:request==DOWN?squeeze_stamina_cost:request==B?fire_stamina_cost:melee_stamina_cost;
 if(v.sta>=cost*60){v.pending=0;if(request==UP){v.action=NONE;v.vy=motion_jump_velocity_q8;v.took_off_at=tick;v.sta-=cost*60;v.last_spend=tick;feedback|=JUMPED;emit(EV_JUMP,who,who);if(v.landed_at==tick-1&&!(input.pressed&UP))emit(EV_LANDING_JUMP,who,who);}
 else if(request==DOWN)enter(who,SQUEEZE,squeeze_startup_ticks+squeeze_active_ticks+squeeze_recovery_ticks,cost);
 else if(request==A){enter(who,MELEE,d.melee_total,cost,followup);feedback|=SWUNG;emit(EV_MELEE,who,who,false,0,followup);}
 else if(request==B){enter(who,FIRE,d.fire_total,cost,followup);p[who].reserved=true;p[who].owner=who;}
 else{enter(who,SUPER,super_startup_ticks+super_active_ticks+super_recovery_ticks,cost);v.sp-=super_power_cost;p[who+2].reserved=true;p[who+2].owner=who;feedback|=CHARGED;emit(EV_CHARGE,who,who);}
 if(followup)emit(EV_FOLLOWUP,who,who,false,0,true);
 }}
 if(v.action==NONE&&ground&&v.vy>=0&&(input.held&L)&&v.sta>=resources_block_enter_min_stamina*60)v.action=BLOCK;
 if(v.action==BLOCK){v.sta=maxv(0,v.sta-resources_block_drain_subunits_per_tick);v.last_spend=tick;if(!v.sta){v.action=GUARD_BREAK;v.began=tick;v.ends=tick+reactions_guard_break_ticks;feedback|=BROKEN;emit(EV_BREAK,who,who);}}
 int dir=((input.held&RIGHT)?1:0)-((input.held&LEFT)?1:0);
 if(v.action==NONE){
 bool on_floor=ground&&v.vy>=0;
 int target=dir*(on_floor?d.speed:d.speed*motion_air_speed_numerator/motion_air_speed_denominator);
 int acceleration=on_floor?(dir?(dir*v.vx<0?motion_ground_turn_acceleration_q8:motion_ground_acceleration_q8):motion_ground_braking_q8):(dir?motion_air_acceleration_q8:motion_air_drag_q8);
 v.vx=approach(v.vx,target,acceleration);
 }
 else if(ground||v.action==HITSTUN||v.action==GUARD_BREAK||v.action==BLOCKSTUN)v.vx=v.action==MELEE&&ground&&v.vy>=0&&age(v)<d.melee_start?v.face*d.melee_lunge:0;
}
void CombatCore::physics(){int previous_order=f[0].x<=f[1].x?1:-1;
 for(auto&v:f){int old_x=v.x;v.x=clamp(v.x+v.vx+v.kb,world_min_fighter_x*256,world_max_fighter_x*256);
 if(v.x==world_min_fighter_x*256&&v.vx<0)v.vx=0;
 if(v.x==world_max_fighter_x*256&&v.vx>0)v.vx=0;
 if(grounded(v)&&v.action==NONE&&v.vy>=0)v.stride_q8=(v.stride_q8+absv(v.x-old_x))%motion_walk_stride_q8;
 v.kb=approach(v.kb,0,motion_knockback_decay_q8);
 if(v.y<world_floor_y*256||v.vy<0){v.vy=minv(motion_max_fall_velocity_q8,v.vy+motion_gravity_q8);v.y+=v.vy;
 if(v.y>=world_floor_y*256){v.y=world_floor_y*256;v.vy=0;v.landed_at=tick;}}
 }
 const int*b0=low(f[0])?collision_squeezed_pushbox:collision_standing_pushbox;const int*b1=low(f[1])?collision_squeezed_pushbox:collision_standing_pushbox;Rect a=box(f[0].x,f[0].y,b0),b=box(f[1].x,f[1].y,b1);
 if(overlaps(a,b)){int order=f[0].x==f[1].x?previous_order:(f[0].x<f[1].x?1:-1);int gap=16*256-absv(f[0].x-f[1].x);int x0=clamp(f[0].x-order*(gap/2),world_min_fighter_x*256,world_max_fighter_x*256);int x1=clamp(f[1].x+order*(gap-gap/2),world_min_fighter_x*256,world_max_fighter_x*256);int left=16*256-absv(x0-x1);if(left>0){if(x0==world_min_fighter_x*256||x0==world_max_fighter_x*256)x1=clamp(x1+order*left,world_min_fighter_x*256,world_max_fighter_x*256);else x0=clamp(x0-order*left,world_min_fighter_x*256,world_max_fighter_x*256);}f[0].x=x0;f[1].x=x1;}}
void CombatCore::spawn(){for(int who=0;who<2;who++){auto&v=f[who];const auto&d=characters[v.character];bool sup=v.action==SUPER,fire=v.action==FIRE;if((fire||sup)&&!v.spawned&&age(v)==(sup?super_startup_ticks:d.fire_start)){auto&b=p[who+(sup?2:0)];b=Projectile{};b.active=true;b.owner=who;b.super=sup;b.followup=v.chain_depth>0;b.id=v.attack_id;b.x=v.x+v.face*(sup?collision_super_spawn_forward_px:collision_fire_spawn_forward_px)*256;b.y=(sup?world_floor_y*256:v.y)-collision_projectile_spawn_above_feet_px*256;b.px=b.x;b.py=b.y;b.vx=v.face*(sup?super_projectile_speed_q8:d.shot_speed);b.damage=sup?d.super_damage:d.fire_damage;b.born=tick;v.spawned=true;feedback|=sup?SUPER_FIRED:SHOT;emit(sup?EV_RELEASE:EV_SHOT,who,who,sup,0,b.followup,b.id);}}
 for(auto&b:p)if(b.active){b.px=b.x;b.py=b.y;if(tick>=b.born+(b.super?super_lifetime_ticks:fire_lifetime_ticks)){b.active=false;continue;}if(tick>b.born)b.x+=b.vx;}}
struct Contact {Fraction time;int kind,source,target;uint32_t id;};
void CombatCore::collide(const Fighter previous[2]){Contact contacts[16];int count=0;auto add=[&](Contact c){if(count>=16){__builtin_trap();}contacts[count++]=c;};
 Rect hurt[2];for(int i=0;i<2;i++)hurt[i]=box(f[i].x,f[i].y,low(f[i])?collision_squeezed_hurtbox:collision_standing_hurtbox);
 for(int i=0;i<2;i++){auto&v=f[i];const auto&d=characters[v.character];if(v.action==MELEE&&!v.melee_contact&&age(v)>=d.melee_start&&age(v)<d.melee_start+d.melee_active){int hb[4]={collision_right_melee_hitbox[0],collision_right_melee_hitbox[1],d.melee_reach,collision_right_melee_hitbox[3]};if(v.face<0){hb[0]=-d.melee_reach;hb[2]=-collision_right_melee_hitbox[0];}if(overlaps(box(v.x,v.y,hb),hurt[1-i]))add({{1,1},2,i,1-i,v.attack_id});}}
 for(int i=0;i<4;i++)if(p[i].active){auto&b=p[i];int target=1-b.owner;const int*shape=b.super?collision_super_hitbox:collision_fire_hitbox;Rect start=box(b.px,b.py,shape);Fraction time;bool hit=false;if(b.born==tick){hit=overlaps(box(b.x,b.y,shape),hurt[target]);time={1,1};}else{auto&old=previous[target];const int*hb=low(f[target])?collision_squeezed_hurtbox:collision_standing_hurtbox;hit=sweep(start,b.x-b.px,0,box(old.x,old.y,hb),f[target].x-old.x,f[target].y-old.y,time);}if(hit)add({time,1,i,target,b.id});
 for(int j=i+1;j<4;j++)if(p[j].active&&p[j].owner!=b.owner&&!(p[j].super&&b.super)){auto&c=p[j];bool clash;if(b.born==tick||c.born==tick){clash=overlaps(box(b.x,b.y,shape),box(c.x,c.y,c.super?collision_super_hitbox:collision_fire_hitbox));time={1,1};}else clash=sweep(start,b.x-b.px,0,box(c.px,c.py,c.super?collision_super_hitbox:collision_fire_hitbox),c.x-c.px,0,time);if(clash)add({time,0,i,j,b.id<c.id?b.id:c.id});}}
 for(int i=1;i<count;i++){Contact c=contacts[i];int j=i;while(j>0){auto&b=contacts[j-1];bool before=less(c.time,b.time)||(!less(b.time,c.time)&&!less(c.time,b.time)&&(c.kind==0&&b.kind!=0?true:c.kind!=0&&b.kind==0?false:c.id<b.id));if(!before)break;contacts[j]=b;--j;}contacts[j]=c;}
 bool guard[2]={f[0].action==BLOCK||f[0].action==BLOCKSTUN,f[1].action==BLOCK||f[1].action==BLOCKSTUN};bool invuln[2]={tick<f[0].invuln_until,tick<f[1].invuln_until};int damage[2]={},stun[2]={},reaction[2]={},kb[2]={},lift[2]={};
 for(int k=0;k<count;k++){auto c=contacts[k];if(c.kind==0){auto&a=p[c.source];auto&b=p[c.target];if(!a.active||!b.active)continue;if(!a.super)a.active=false;if(!b.super)b.active=false;continue;}
 int attacker,amount,dir;bool sup=false,followup=false;if(c.kind==1){auto&b=p[c.source];if(!b.active)continue;b.active=false;attacker=b.owner;amount=b.damage;dir=b.vx>0?1:-1;sup=b.super;followup=b.followup;}else{attacker=c.source;f[attacker].melee_contact=true;amount=characters[f[attacker].character].melee_damage;dir=f[attacker].face;followup=f[attacker].chain_depth>0;}
 int target=c.target;auto&v=f[target];if(invuln[target])continue;
 if(!sup&&guard[target]&&grounded(v)&&v.face==-dir){int points=(amount*resources_guard_cost_numerator+resources_guard_cost_denominator-1)/resources_guard_cost_denominator;int pressure=c.kind==1?characters[f[attacker].character].fire_guard_percent:100;int impact=(points*pressure*characters[v.character].guard_percent+9999)/10000*60;
 if(v.sta>=impact){v.sta-=impact;v.last_spend=tick;feedback|=BLOCKED;emit(EV_BLOCK,attacker,target);hitstop=maxv(hitstop,reactions_block_hitstop_display_ticks);if(!v.sta){guard[target]=false;reaction[target]=GUARD_BREAK;stun[target]=reactions_guard_break_ticks;feedback|=BROKEN;emit(EV_BREAK,attacker,target);}else if(stun[target]<reactions_blockstun_ticks){stun[target]=reactions_blockstun_ticks;reaction[target]=BLOCKSTUN;}continue;}
 v.sta=0;v.last_spend=tick;reaction[target]=GUARD_BREAK;stun[target]=reactions_guard_break_ticks;feedback|=BROKEN;emit(EV_BREAK,attacker,target);}
 if(c.kind==2){if(recovering(target)){amount+=characters[f[attacker].character].punish_bonus;emit(EV_PUNISH,attacker,target,false,amount,followup,c.id);}if(!f[attacker].chain_depth)f[attacker].confirmed_at=tick;}
 guard[target]=false;damage[target]+=amount;int duration=sup?reactions_super_hitstun_ticks:reactions_normal_hitstun_ticks;if(duration>stun[target]){stun[target]=duration;reaction[target]=HITSTUN;}int magnitude=sup?motion_super_knockback_q8:c.kind==1?characters[f[attacker].character].fire_knockback:motion_normal_knockback_q8;if(magnitude>absv(kb[target]))kb[target]=dir*magnitude;lift[target]=minv(lift[target],sup?motion_super_hit_lift_q8:motion_normal_hit_lift_q8);
 hitstop=maxv(hitstop,sup?reactions_super_hitstop_display_ticks:reactions_normal_hitstop_display_ticks);feedback|=HIT;emit(EV_HIT,attacker,target,sup,amount,followup,c.id);
 if(!sup){int old=f[attacker].sp;f[attacker].sp=minv(100,old+(c.kind==2?resources_melee_clean_hit_power:resources_fire_clean_hit_power));if(old<100&&f[attacker].sp==100){feedback|=CHARGED;emit(EV_READY,attacker,attacker);}}}
 for(int i=0;i<2;i++){auto&v=f[i];v.hp=maxv(0,v.hp-damage[i]);if(stun[i]){cancel(i);v.action=Action(reaction[i]);v.began=tick+1;v.ends=tick+1+stun[i];v.vx=0;}if(damage[i]){v.invuln_until=tick+1+reactions_post_hit_invulnerability_ticks;v.kb=kb[i];v.vy=minv(v.vy,lift[i]);}}
 for(auto&b:p)if(b.active&&(b.x < -16*256||b.x>256*256))b.active=false;
}
void CombatCore::resources(){for(auto&v:f)if(v.action==NONE&&tick-v.last_spend>=resources_stamina_regen_delay_ticks)v.sta=minv(characters[v.character].sta*60,v.sta+characters[v.character].regen);}
void CombatCore::step(Input player,Input cpu){begin_events();if(outcome)return;feedback=0;Fighter previous[2]={f[0],f[1]};action(0,player);action(1,cpu);physics();spawn();collide(previous);resources();if(!training)--remaining;if(!f[0].hp||!f[1].hp)outcome=!f[0].hp&&!f[1].hp?3:!f[1].hp?1:2;else if(!training&&remaining<=0){int a=f[0].hp*characters[f[1].character].hp,b=f[1].hp*characters[f[0].character].hp;outcome=a>b?1:a<b?2:3;}if(outcome){for(auto&b:p)b=Projectile{};hitstop=0;for(auto&v:f)v.pending=0;}++tick;}
uint32_t CombatCore::checksum()const{uint32_t h=2166136261u;auto mix=[&](uint32_t n){h=(h^n)*16777619u;};mix(seed);mix(training);mix(tick);mix(remaining);mix(hitstop);mix(outcome);mix(next_id);for(auto&v:f){mix(v.character);mix(v.x);mix(v.y);mix(v.vx);mix(v.vy);mix(v.hp);mix(v.sta);mix(v.sp);mix(v.kb);mix(v.face);mix(v.action);mix(v.began);mix(v.ends);mix(v.invuln_until);mix(v.squeeze_ready);mix(v.last_spend);mix(v.pending);mix(v.pending_until);mix(v.attack_id);mix(v.melee_contact);mix(v.spawned);mix(v.stride_q8);mix(v.landed_at);mix(v.took_off_at);mix(v.confirmed_at);mix(v.chain_depth);}for(auto&b:p){mix(b.active);mix(b.reserved);mix(b.super);mix(b.followup);mix(b.owner);mix(b.id);mix(b.x);mix(b.y);mix(b.px);mix(b.py);mix(b.vx);mix(b.born);mix(b.damage);}return h;}
// CPU decisions only produce the same button requests available to a player.
uint32_t CpuController::random(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
void CpuController::reset(int level,uint32_t value){
 difficulty=clamp(level,0,2);rng=value?value:1;stored=-1;held=0;cached={};
 seen_attack=combo_attack=0;resting=combo_allowed=false;
 for(int i=0;i<3;++i){threat[i]=0;respond[i]=false;dodge[i]=0;}
 next_plan=plan=jitter=hesitate_until=next_shot=rest_began=memory_at=0;last_seen=-1000;
 last_hp=last_power=-1;last_progress=0;
 for(auto&m:memory)m=NONE;
 diagnostics={};
}
Input CpuController::input(const CombatCore&core,int who){
 if(core.hitstop||core.outcome)return{held,0};
 if(stored==core.tick)return cached; // Idempotent queries; no repeated random rolls.
 const auto&me=core.f[who];
 if(me.hp!=last_hp||me.sp!=last_power){last_progress=core.tick;last_hp=me.hp;last_power=me.sp;}
 const auto&profile=cpu_difficulties[difficulty];const auto&style=cpu_styles[me.character];
 const auto&def=characters[me.character];
 auto&fresh=history[core.tick%cpu_history_ticks];const auto&visible=core.f[1-who];
 fresh.enemy={visible.character,visible.x,visible.y,visible.vx,visible.vy,visible.face,visible.action,visible.began,visible.attack_id};
 for(int i=0;i<4;++i){const auto&b=core.p[i];fresh.shots[i]={b.active,b.super,b.owner,b.x,b.y,b.vx,b.id};}
 if(stored<0)for(auto&o:history)o=fresh;
 stored=core.tick;
 const int observed=maxv(0,core.tick-profile.reaction_ticks),elapsed=core.tick-observed;
 const auto&o=history[observed%cpu_history_ticks];const auto&enemy=o.enemy;const auto&ed=characters[enemy.character];
 // Short, bounded extrapolation of already visible movement. No current enemy query.
 const int ex=clamp(enemy.x+enemy.vx*minv(elapsed,6),world_min_fighter_x*256,world_max_fighter_x*256);
 const int delta=ex-me.x,dist=absv(delta)/256,toward=delta>=0?RIGHT:LEFT,away=delta>=0?LEFT:RIGHT;
 const int enemy_age=core.tick-enemy.began;
 const bool neutral=me.action==NONE||me.action==BLOCK,ground=core.grounded(me);
 const bool low_enemy=enemy.action==SQUEEZE&&enemy_age>=squeeze_startup_ticks&&enemy_age<squeeze_startup_ticks+squeeze_active_ticks;
 const bool target_low=enemy.y>=108*256; // Don't shoot across a visibly high jump.
 if(core.tick-last_seen>cpu_memory_expire_ticks){for(auto&m:memory)m=NONE;}
 if((enemy.action==MELEE||enemy.action==FIRE||enemy.action==SUPER)&&enemy.attack&&enemy.attack!=seen_attack){
  seen_attack=enemy.attack;memory[memory_at]=enemy.action;memory_at=(memory_at+1)%cpu_memory_actions;last_seen=core.tick;
 }
 diagnostics.observed_tick=observed;diagnostics.melee_memory=diagnostics.fire_memory=diagnostics.super_memory=0;
 for(auto m:memory){diagnostics.melee_memory+=m==MELEE;diagnostics.fire_memory+=m==FIRE;diagnostics.super_memory+=m==SUPER;}
 auto finish=[&](int next,int intent)->Input{
  diagnostics.intent=intent;held=uint16_t(next)&uint16_t(LEFT|RIGHT|L);
  cached={uint16_t(next),uint16_t(next&int(A|B|R|UP|DOWN))};return cached;
 };
 if(!resting&&me.sta<style.recover_below*60){resting=true;rest_began=core.tick;}
 else if(resting&&me.sta>=style.resume_at*60)resting=false;
 diagnostics.resting=resting;
 // Own hit confirms can be queued outside the perception cadence. They use the
 // unchanged public combat API, full costs, startup, and one-follow-up limit.
 if(core.can_followup(who,B,input_buffer_ticks-1)||core.can_followup(who,A,input_buffer_ticks-1)){
  if(combo_attack!=me.attack_id){combo_attack=me.attack_id;combo_allowed=roll(profile.followup_percent);}
  if(combo_allowed){
   int button=0;
   if(def.melee_followup&&dist<def.melee_reach+5&&target_low&&core.can_followup(who,A,input_buffer_ticks-1))button=A;
   else if(dist<145&&target_low&&!low_enemy&&core.can_followup(who,B,input_buffer_ticks-1))button=B;
   if(button){combo_allowed=false;++diagnostics.followups;return finish(button,FOLLOW);}
  }
 }
 if(core.tick%profile.think_ticks)return finish(held,diagnostics.intent);
 ++diagnostics.decisions;
 if(core.tick>=next_plan){plan=int(random()%3);jitter=int(random()%11)-5;next_plan=core.tick+profile.plan_ticks;}
 const bool closing=core.tick-last_progress>240||(!core.training&&core.remaining<600);
 diagnostics.closing=closing;
 diagnostics.plan=plan;diagnostics.target=closing?24:clamp(style.distance_px+(plan==0?-8:plan==2?8:0)+jitter,18,94);
 const int reach=def.melee_reach+6,enemy_reach=ed.melee_reach+7;
 const int recovery_start=enemy.action==MELEE?ed.melee_start+ed.melee_active:enemy.action==FIRE?ed.fire_start+1:enemy.action==SUPER?super_startup_ticks+super_active_ticks:999;
 const int total=enemy.action==MELEE?ed.melee_total:enemy.action==FIRE?ed.fire_total:super_startup_ticks+super_active_ticks+super_recovery_ticks;
 const int left=total-enemy_age;
 const bool recovery=enemy_age>=recovery_start&&left>def.melee_start;
 const bool startup=enemy.action==SUPER&&enemy_age>=0&&enemy_age+def.melee_start<super_startup_ticks;
 // A visible whiff/recovery or interruptible super charge, not the opponent's
 // private end tick, pending input, hit flags, invulnerability or stamina.
 if(neutral&&ground&&target_low&&dist<reach&&me.sta>=melee_stamina_cost*60&&(recovery||startup)&&roll(profile.punish_percent+style.punish_bonus)){
  ++diagnostics.punishes;return finish(A,PUNISH);
 }
 int eta=999;uint32_t threat_id=0;bool super_threat=false,melee_threat=false;
 for(const auto&b:o.shots)if(b.active&&b.owner!=who&&b.vx&&((me.x-b.x)*int64_t(b.vx)>0)&&b.y>=92*256&&b.y<=world_floor_y*256){
  const int margin=b.super?13:10;
  const int arrival=maxv(0,absv(me.x-b.x)-margin*256)/absv(b.vx)-elapsed;
  if(arrival>=-profile.think_ticks&&arrival<eta){eta=arrival;threat_id=b.id;super_threat=b.super;}
 }
 if(enemy.action==SUPER&&enemy.attack&&enemy_age<super_startup_ticks){
  const int arrival=super_startup_ticks-enemy_age+maxv(0,dist-collision_super_spawn_forward_px-13)/5;
  if(arrival<eta){eta=arrival;threat_id=enemy.attack;super_threat=true;}
 }
 if(enemy.action==MELEE&&enemy.attack&&dist<enemy_reach&&target_low&&enemy_age<ed.melee_start+ed.melee_active){
  const int arrival=maxv(0,ed.melee_start-enemy_age);
  if(arrival<eta){eta=arrival;threat_id=enemy.attack;super_threat=false;melee_threat=true;}
 }
 const int response_slot=super_threat?1:melee_threat?2:0;
 if(threat_id&&threat_id!=threat[response_slot]){
  threat[response_slot]=threat_id;++diagnostics.response_choices;
  const int habit=diagnostics.fire_memory>=4?profile.adapt_percent:0;
  respond[response_slot]=roll(super_threat?profile.super_evade_percent:profile.defense_percent+style.guard_bonus+habit);
  // Squirtle favors its efficient guard. Repeated projectiles encourage a
  // dodge on higher levels; one choice per visible attack, never per frame.
  dodge[response_slot]=int(random()%2);
  if(!super_threat&&(melee_threat||me.character==2||!roll(20+habit)))dodge[response_slot]=2;
 }
 const int choice=dodge[response_slot];
 if(neutral&&ground&&respond[response_slot]&&eta>=0){
  if(super_threat){
   if(eta<=18&&eta>=5&&choice==0&&me.sta>=jump_stamina_cost*60){++diagnostics.defenses;return finish(UP,DEFEND);}
   if(eta<=20&&eta>=squeeze_startup_ticks&&core.tick>=me.squeeze_ready&&me.sta>=squeeze_stamina_cost*60){++diagnostics.defenses;return finish(DOWN,DEFEND);}
   if(eta<=18&&eta>=5&&me.sta>=jump_stamina_cost*60){++diagnostics.defenses;return finish(UP,DEFEND);}
  }else if(eta<=18){
   const int amount=melee_threat?ed.melee_damage:ed.fire_damage;
   const int points=(amount*resources_guard_cost_numerator+resources_guard_cost_denominator-1)/resources_guard_cost_denominator;
   const int impact=(points*(melee_threat?100:ed.fire_guard_percent)*def.guard_percent+9999)/10000;
   if((choice==2||eta<5)&&me.sta>=(impact+4)*60){++diagnostics.defenses;return finish(L,DEFEND);}
   if(!melee_threat&&eta>=5&&choice==0&&me.sta>=jump_stamina_cost*60){++diagnostics.defenses;return finish(UP,DEFEND);}
   if(!melee_threat&&eta>=squeeze_startup_ticks&&core.tick>=me.squeeze_ready&&me.sta>=squeeze_stamina_cost*60){++diagnostics.defenses;return finish(DOWN,DEFEND);}
  }
 }
 if(!neutral)return finish(0,diagnostics.intent); // Never buffer speculative attacks through stun.
 if(resting){
  // A short disengage, then recover in place: avoid endless retreat loops.
  const bool room=delta>=0?me.x>(world_min_fighter_x+6)*256:me.x<(world_max_fighter_x-6)*256;
  return finish(ground&&room&&dist<enemy_reach+8&&core.tick-rest_began<24?away:0,RECOVER);
 }
 if(core.tick<hesitate_until)return finish(0,HESITATE);
 if(roll(profile.mistake_percent)){hesitate_until=core.tick+profile.hesitate_ticks;++diagnostics.hesitations;return finish(0,HESITATE);}
 if(!ground)return finish(dist>diagnostics.target?toward:0,SPACE);
 // Brief, fallible prediction of a repeated visible melee habit. No prediction
 // before four observations, no always-on guard and no read of the next input.
 if(difficulty&&diagnostics.melee_memory>=4&&dist<enemy_reach+6&&plan==2&&core.tick%48<8&&me.sta>=40*60&&roll(profile.adapt_percent))return finish(L,DEFEND);
 const bool own_shot=core.p[who].active||core.p[who].reserved;
 if(me.sp==100&&me.sta>=(super_stamina_cost+8)*60&&dist>40&&dist<145&&target_low&&!low_enemy&&enemy.action!=SUPER&&!core.p[who+2].active&&!core.p[who+2].reserved&&roll(65+(plan==0?15:0))){
  if(me.action==BLOCK)return finish(0,SUPER_ATTACK); // Release block before R, like a player.
  return finish(R,SUPER_ATTACK);
 }
 if(target_low&&dist<reach&&me.sta>=melee_stamina_cost*60&&roll(style.melee_percent+(plan==0?10:0)))return finish(A,PRESSURE);
 if(closing&&dist>=reach)return finish(toward,APPROACH);
 int fire_chance=style.fire_percent+(plan==2?10:plan==0?-12:0);
 if(enemy.action==BLOCK||enemy.action==BLOCKSTUN)fire_chance+=me.character==1?15:-12;
 if(!own_shot&&core.tick>=next_shot&&dist>reach+12&&dist<150&&target_low&&!low_enemy&&enemy.action!=SUPER&&me.sta>=(fire_stamina_cost+style.recover_below/2)*60&&roll(fire_chance)){
  next_shot=core.tick+def.fire_total+18+int(random()%19);return finish(B,PRESSURE);
 }
 if(dist>diagnostics.target+5)return finish(toward,APPROACH);
 // Long-reach Bulbasaur holds its range. Other fighters can step out briefly;
 // a corner removes retreat, so closing in remains a reliable counter.
 const bool retreat_room=delta>=0?me.x>(world_min_fighter_x+6)*256:me.x<(world_max_fighter_x-6)*256;
 if(!closing&&dist<diagnostics.target-5&&(plan==2||me.character==2)&&me.sta>=28*60&&retreat_room)return finish(away,SPACE);
 if(dist>=reach&&core.tick%36<18)return finish(toward,APPROACH);
 return finish(0,SPACE);
}
}
