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
void CombatCore::emit(EventKind kind,int owner,int target,bool sup){
 if(event_count>=max_events)__builtin_trap();
 const auto&v=f[target];int x=v.x,y=v.y-18*256;if(kind==EV_SHOT||kind==EV_RELEASE){const auto&b=p[owner+(kind==EV_RELEASE?2:0)];x=b.x;y=b.y;}events[event_count++]={kind,owner,target,x,y,sup,f[owner].attack_id};
}
// Existing three-tick queue: accept a descending jump only if touchdown occurs
// in time to consume it before expiry. Jump height, cost and airborne motion stay unchanged.
bool CombatCore::jump_buffer_eligible(const Fighter&v)const{
 if(grounded(v))return true;if(v.vy<=0)return false;
 int y=v.y,vy=v.vy;for(int n=0;n<input_buffer_ticks-1;n++){vy=minv(motion_max_fall_velocity_q8,vy+motion_gravity_q8);y+=vy;if(y>=world_floor_y*256)return true;}return false;
}
void CombatCore::reset(int player,int cpu,uint32_t match_seed){event_count=0;event_frame=0;tick=0;remaining=match_round_ticks;hitstop=outcome=feedback=0;next_id=1;seed=match_seed?match_seed:1;for(auto&v:p)v=Projectile{};for(int i=0;i<2;i++){f[i]=Fighter{};auto&v=f[i];v.character=i?cpu:player;v.x=(i?world_cpu_spawn_x:world_player_spawn_x)*256;v.hp=characters[v.character].hp;v.sta=characters[v.character].sta*resources_stamina_subunits_per_point;v.face=i?-1:1;}}
void CombatCore::buffer(int who,Input input){auto&v=f[who];const int keys[5]={R,UP,DOWN,B,A};for(int k:keys)if(input.pressed&k){int cost=k==R?super_stamina_cost:k==UP?jump_stamina_cost:k==DOWN?squeeze_stamina_cost:k==B?fire_stamina_cost:melee_stamina_cost;
 if((k==B&&(p[who].active||p[who].reserved))||(k==R&&(p[who+2].active||p[who+2].reserved||v.action==BLOCK))||(k==R&&(v.sp<100||!grounded(v)))||(k==DOWN&&(!grounded(v)||tick<v.squeeze_ready))||(k==UP&&!jump_buffer_eligible(v)))continue;
 if(v.sta<cost*60){emit(EV_TIRED,who,who);continue;}
 if(!v.pending||tick>=v.pending_until||priority(k)>=priority(v.pending)){v.pending=k;v.pending_until=tick+input_buffer_ticks;}break;}}
void CombatCore::frame(Input player,Input cpu){if(outcome){begin_events();return;}if(hitstop){begin_events();buffer(0,player);buffer(1,cpu);--hitstop;feedback=0;return;}step(player,cpu);}
void CombatCore::cancel(int who){auto&v=f[who];if(v.action==SQUEEZE)v.squeeze_ready=tick+squeeze_cooldown_after_end_ticks;for(auto&b:p)if(b.owner==who&&b.reserved)b.reserved=false;}
void CombatCore::enter(int who,Action a,int duration,int cost){auto&v=f[who];v.action=a;v.began=tick;v.ends=tick+duration;v.sta-=cost*60;v.last_spend=tick;v.attack_id=next_id++;v.melee_contact=false;v.spawned=false;}
void CombatCore::action(int who,Input input){auto&v=f[who];const auto&d=characters[v.character];bool ground=grounded(v);
 if(v.action!=NONE&&v.action!=BLOCK&&tick>=v.ends){if(v.action==SQUEEZE)v.squeeze_ready=tick+squeeze_cooldown_after_end_ticks;v.action=NONE;}
 if(v.action==BLOCK&&!(input.held&L))v.action=NONE;
 if(v.action==NONE||v.action==BLOCK){int delta=f[1-who].x-v.x;if(delta)v.face=delta>0?1:-1;}
 buffer(who,input);if(tick>=v.pending_until)v.pending=0;
 int request=v.pending;bool neutral=v.action==NONE,defensive=neutral||v.action==BLOCK;
 bool can=request==R?(neutral&&ground&&v.sp==100&&!p[who+2].active&&!p[who+2].reserved):request==UP?(defensive&&ground):request==DOWN?(defensive&&ground&&tick>=v.squeeze_ready):request==B?(neutral&&!p[who].active&&!p[who].reserved):request==A?neutral:false;
 if(can){int cost=request==R?super_stamina_cost:request==UP?jump_stamina_cost:request==DOWN?squeeze_stamina_cost:request==B?fire_stamina_cost:melee_stamina_cost;
 if(v.sta>=cost*60){v.pending=0;if(request==UP){v.action=NONE;v.vy=motion_jump_velocity_q8;v.took_off_at=tick;v.sta-=cost*60;v.last_spend=tick;feedback|=JUMPED;emit(EV_JUMP,who,who);if(v.landed_at==tick-1&&!(input.pressed&UP))emit(EV_LANDING_JUMP,who,who);}
 else if(request==DOWN)enter(who,SQUEEZE,squeeze_startup_ticks+squeeze_active_ticks+squeeze_recovery_ticks,cost);
 else if(request==A){enter(who,MELEE,d.melee_total,cost);feedback|=SWUNG;emit(EV_MELEE,who,who);}
 else if(request==B){enter(who,FIRE,d.fire_total,cost);p[who].reserved=true;p[who].owner=who;}
 else{enter(who,SUPER,super_startup_ticks+super_active_ticks+super_recovery_ticks,cost);v.sp-=super_power_cost;p[who+2].reserved=true;p[who+2].owner=who;feedback|=CHARGED;emit(EV_CHARGE,who,who);}
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
 else if(ground||v.action==HITSTUN||v.action==GUARD_BREAK||v.action==BLOCKSTUN)v.vx=0;
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
void CombatCore::spawn(){for(int who=0;who<2;who++){auto&v=f[who];const auto&d=characters[v.character];bool sup=v.action==SUPER,fire=v.action==FIRE;if((fire||sup)&&!v.spawned&&age(v)==(sup?super_startup_ticks:d.fire_start)){auto&b=p[who+(sup?2:0)];b=Projectile{};b.active=true;b.owner=who;b.super=sup;b.id=v.attack_id;b.x=v.x+v.face*(sup?collision_super_spawn_forward_px:collision_fire_spawn_forward_px)*256;b.y=(sup?world_floor_y*256:v.y)-collision_projectile_spawn_above_feet_px*256;b.px=b.x;b.py=b.y;b.vx=v.face*(sup?super_projectile_speed_q8:d.shot_speed);b.damage=sup?d.super_damage:d.fire_damage;b.born=tick;v.spawned=true;feedback|=sup?SUPER_FIRED:SHOT;emit(sup?EV_RELEASE:EV_SHOT,who,who,sup);}}
 for(auto&b:p)if(b.active){b.px=b.x;b.py=b.y;if(tick>=b.born+(b.super?super_lifetime_ticks:fire_lifetime_ticks)){b.active=false;continue;}if(tick>b.born)b.x+=b.vx;}}
struct Contact {Fraction time;int kind,source,target;uint32_t id;};
void CombatCore::collide(const Fighter previous[2]){Contact contacts[16];int count=0;auto add=[&](Contact c){if(count>=16){__builtin_trap();}contacts[count++]=c;};
 Rect hurt[2];for(int i=0;i<2;i++)hurt[i]=box(f[i].x,f[i].y,low(f[i])?collision_squeezed_hurtbox:collision_standing_hurtbox);
 for(int i=0;i<2;i++){auto&v=f[i];const auto&d=characters[v.character];if(v.action==MELEE&&!v.melee_contact&&age(v)>=d.melee_start&&age(v)<d.melee_start+d.melee_active){int hb[4]={collision_right_melee_hitbox[0],collision_right_melee_hitbox[1],collision_right_melee_hitbox[2],collision_right_melee_hitbox[3]};if(v.face<0){hb[0]=-collision_right_melee_hitbox[2];hb[2]=-collision_right_melee_hitbox[0];}if(overlaps(box(v.x,v.y,hb),hurt[1-i]))add({{1,1},2,i,1-i,v.attack_id});}}
 for(int i=0;i<4;i++)if(p[i].active){auto&b=p[i];int target=1-b.owner;const int*shape=b.super?collision_super_hitbox:collision_fire_hitbox;Rect start=box(b.px,b.py,shape);Fraction time;bool hit=false;if(b.born==tick){hit=overlaps(box(b.x,b.y,shape),hurt[target]);time={1,1};}else{auto&old=previous[target];const int*hb=low(f[target])?collision_squeezed_hurtbox:collision_standing_hurtbox;hit=sweep(start,b.x-b.px,0,box(old.x,old.y,hb),f[target].x-old.x,f[target].y-old.y,time);}if(hit)add({time,1,i,target,b.id});
 for(int j=i+1;j<4;j++)if(p[j].active&&p[j].owner!=b.owner&&!(p[j].super&&b.super)){auto&c=p[j];bool clash;if(b.born==tick||c.born==tick){clash=overlaps(box(b.x,b.y,shape),box(c.x,c.y,c.super?collision_super_hitbox:collision_fire_hitbox));time={1,1};}else clash=sweep(start,b.x-b.px,0,box(c.px,c.py,c.super?collision_super_hitbox:collision_fire_hitbox),c.x-c.px,0,time);if(clash)add({time,0,i,j,b.id<c.id?b.id:c.id});}}
 for(int i=1;i<count;i++){Contact c=contacts[i];int j=i;while(j>0){auto&b=contacts[j-1];bool before=less(c.time,b.time)||(!less(b.time,c.time)&&!less(c.time,b.time)&&(c.kind==0&&b.kind!=0?true:c.kind!=0&&b.kind==0?false:c.id<b.id));if(!before)break;contacts[j]=b;--j;}contacts[j]=c;}
 bool guard[2]={f[0].action==BLOCK||f[0].action==BLOCKSTUN,f[1].action==BLOCK||f[1].action==BLOCKSTUN};bool invuln[2]={tick<f[0].invuln_until,tick<f[1].invuln_until};int damage[2]={},stun[2]={},reaction[2]={},kb[2]={},lift[2]={};
 for(int k=0;k<count;k++){auto c=contacts[k];if(c.kind==0){auto&a=p[c.source];auto&b=p[c.target];if(!a.active||!b.active)continue;if(!a.super)a.active=false;if(!b.super)b.active=false;continue;}
 int attacker,amount,dir;bool sup=false;if(c.kind==1){auto&b=p[c.source];if(!b.active)continue;b.active=false;attacker=b.owner;amount=b.damage;dir=b.vx>0?1:-1;sup=b.super;}else{attacker=c.source;f[attacker].melee_contact=true;amount=characters[f[attacker].character].melee_damage;dir=f[attacker].face;}
 int target=c.target;auto&v=f[target];if(invuln[target])continue;
 if(!sup&&guard[target]&&grounded(v)&&v.face==-dir){int impact=(amount*resources_guard_cost_numerator+resources_guard_cost_denominator-1)/resources_guard_cost_denominator*60;
 if(v.sta>=impact){v.sta-=impact;v.last_spend=tick;feedback|=BLOCKED;emit(EV_BLOCK,attacker,target);hitstop=maxv(hitstop,reactions_block_hitstop_display_ticks);if(!v.sta){guard[target]=false;reaction[target]=GUARD_BREAK;stun[target]=reactions_guard_break_ticks;feedback|=BROKEN;emit(EV_BREAK,attacker,target);}else if(stun[target]<reactions_blockstun_ticks){stun[target]=reactions_blockstun_ticks;reaction[target]=BLOCKSTUN;}continue;}
 v.sta=0;v.last_spend=tick;reaction[target]=GUARD_BREAK;stun[target]=reactions_guard_break_ticks;feedback|=BROKEN;emit(EV_BREAK,attacker,target);}
 guard[target]=false;damage[target]+=amount;int duration=sup?reactions_super_hitstun_ticks:reactions_normal_hitstun_ticks;if(duration>stun[target]){stun[target]=duration;reaction[target]=HITSTUN;}int magnitude=sup?motion_super_knockback_q8:motion_normal_knockback_q8;if(magnitude>absv(kb[target]))kb[target]=dir*magnitude;lift[target]=minv(lift[target],sup?motion_super_hit_lift_q8:motion_normal_hit_lift_q8);
 hitstop=maxv(hitstop,sup?reactions_super_hitstop_display_ticks:reactions_normal_hitstop_display_ticks);feedback|=HIT;emit(EV_HIT,attacker,target,sup);
 if(!sup){int old=f[attacker].sp;f[attacker].sp=minv(100,old+(c.kind==2?resources_melee_clean_hit_power:resources_fire_clean_hit_power));if(old<100&&f[attacker].sp==100){feedback|=CHARGED;emit(EV_READY,attacker,attacker);}}}
 for(int i=0;i<2;i++){auto&v=f[i];v.hp=maxv(0,v.hp-damage[i]);if(stun[i]){cancel(i);v.action=Action(reaction[i]);v.began=tick+1;v.ends=tick+1+stun[i];v.vx=0;}if(damage[i]){v.invuln_until=tick+1+reactions_post_hit_invulnerability_ticks;v.kb=kb[i];v.vy=minv(v.vy,lift[i]);}}
 for(auto&b:p)if(b.active&&(b.x < -16*256||b.x>256*256))b.active=false;
}
void CombatCore::resources(){for(auto&v:f)if(v.action==NONE&&tick-v.last_spend>=resources_stamina_regen_delay_ticks)v.sta=minv(characters[v.character].sta*60,v.sta+characters[v.character].regen);}
void CombatCore::step(Input player,Input cpu){begin_events();if(outcome)return;feedback=0;Fighter previous[2]={f[0],f[1]};action(0,player);action(1,cpu);physics();spawn();collide(previous);resources();--remaining;if(!f[0].hp||!f[1].hp)outcome=!f[0].hp&&!f[1].hp?3:!f[1].hp?1:2;else if(remaining<=0){int a=f[0].hp*characters[f[1].character].hp,b=f[1].hp*characters[f[0].character].hp;outcome=a>b?1:a<b?2:3;}if(outcome){for(auto&b:p)b=Projectile{};hitstop=0;for(auto&v:f)v.pending=0;}++tick;}
uint32_t CombatCore::checksum()const{uint32_t h=2166136261u;auto mix=[&](uint32_t n){h=(h^n)*16777619u;};mix(seed);mix(tick);mix(remaining);mix(hitstop);mix(outcome);mix(next_id);for(auto&v:f){mix(v.character);mix(v.x);mix(v.y);mix(v.vx);mix(v.vy);mix(v.hp);mix(v.sta);mix(v.sp);mix(v.kb);mix(v.face);mix(v.action);mix(v.began);mix(v.ends);mix(v.invuln_until);mix(v.squeeze_ready);mix(v.last_spend);mix(v.pending);mix(v.pending_until);mix(v.attack_id);mix(v.melee_contact);mix(v.spawned);mix(v.stride_q8);mix(v.landed_at);mix(v.took_off_at);}for(auto&b:p){mix(b.active);mix(b.reserved);mix(b.super);mix(b.owner);mix(b.id);mix(b.x);mix(b.y);mix(b.px);mix(b.py);mix(b.vx);mix(b.born);mix(b.damage);}return h;}
uint32_t CpuController::random(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
void CpuController::reset(int level,uint32_t value){difficulty=clamp(level,0,2);rng=value?value:1;stored=-1;held=0;threat=0;evade=false;dodge=0;}
Input CpuController::input(const CombatCore&core,int who){if(core.hitstop||core.outcome)return{held,0};if(stored!=core.tick){auto&o=history[core.tick%32];o.f[0]=core.f[0];o.f[1]=core.f[1];for(int i=0;i<4;i++)o.p[i]=core.p[i];if(stored<0)for(int i=0;i<32;i++)history[i]=o;stored=core.tick;}
 held&=uint16_t(LEFT|RIGHT|L);if(core.tick%6)return{held,0};const int delays[3]={18,12,8},rates[3]={45,70,85};int delay=delays[difficulty],at=maxv(0,core.tick-delay);const auto&o=history[at%32];const auto&me=core.f[who];const auto&enemy=o.f[1-who];int delta=enemy.x-me.x,dist=absv(delta)/256;uint16_t next=0;
 uint32_t threat_id=0;int eta=999;bool super_threat=false;for(const auto&b:o.p)if(b.active&&b.owner!=who&&((me.x-b.x)*int64_t(b.vx)>0)){int arrival=absv(me.x-b.x)/absv(b.vx)-delay;if(arrival<eta){eta=arrival;super_threat=b.super;threat_id=b.id;}}
 if(enemy.action==SUPER&&enemy.attack_id){int arrival=super_startup_ticks-(at-enemy.began)-delay+maxv(0,dist-collision_super_spawn_forward_px-13)/5;if(arrival<eta){eta=arrival;super_threat=true;threat_id=enemy.attack_id;}}
 if(super_threat&&threat_id!=threat){threat=threat_id;evade=int(random()%100)<rates[difficulty];dodge=random()%2;}
 bool neutral=me.action==NONE||me.action==BLOCK;bool ground=core.grounded(me);
 if(super_threat&&evade&&eta>=0&&eta<=(dodge?15:18)&&neutral&&ground&&me.sta>=18*60){if(dodge&&core.tick>=me.squeeze_ready)next=DOWN;else next=UP;}
 else if(!super_threat&&eta>=0&&eta<18&&neutral&&ground&&me.sta>=20*60)next=L;
 else if(neutral){if(me.sp==100&&me.sta<25*60){next=delta>0?LEFT:RIGHT;}
 else if(me.sp==100&&me.sta>=25*60&&ground&&dist>38&&dist<155&&enemy.y>=110*256)next=R;
 else if(dist<27&&me.sta>=8*60)next=A;
 else if(dist>42&&dist<175&&me.sta>=25*60&&random()%100<65)next=B;
 else if(dist>23)next=delta>0?RIGHT:LEFT;
 else if(me.sta<18*60)next=delta>0?LEFT:RIGHT;
 if(!next&&ground&&random()%100<5&&me.sta>=40*60)next=UP;}
 uint16_t pressed=next&uint16_t(A|B|R|UP|DOWN);held=next&uint16_t(LEFT|RIGHT|L);return{uint16_t(next),pressed};}
}
