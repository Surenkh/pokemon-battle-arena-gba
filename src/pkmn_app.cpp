#include "pkmn_app.h"
#include "pkmn_combat_core.h"
#include "pkmn_animation.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_item.h"
#include "bn_sprite_font.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_pk_pikachu.h"
#include "bn_sprite_items_pk_charmander.h"
#include "bn_sprite_items_pk_squirtle.h"
#include "bn_sprite_items_pk_bulbasaur.h"
#include "bn_sprite_items_pk_eevee.h"
#include "bn_sprite_items_pk_meowth.h"
#include "bn_sprite_items_shot_0.h"
#include "bn_sprite_items_shot_1.h"
#include "bn_sprite_items_shot_2.h"
#include "bn_sprite_items_shot_3.h"
#include "bn_sprite_items_shot_4.h"
#include "bn_sprite_items_shot_5.h"
#include "bn_sprite_items_wave_0.h"
#include "bn_sprite_items_wave_1.h"
#include "bn_sprite_items_wave_2.h"
#include "bn_sprite_items_wave_3.h"
#include "bn_sprite_items_wave_4.h"
#include "bn_sprite_items_wave_5.h"
#include "bn_sprite_items_focus.h"
#include "bn_sprite_items_bars.h"
#include "bn_sprite_items_shield.h"
#include "bn_sprite_items_dust.h"
#include "bn_sprite_items_impact.h"
#include "bn_sprite_items_ground_shadow.h"
#include "bn_sprite_items_spark.h"
#include "bn_sprite_items_pkfont.h"
#include "bn_regular_bg_items_title.h"
#include "bn_regular_bg_items_ensemble.h"
#include "bn_regular_bg_items_selector.h"
#include "bn_regular_bg_items_arena.h"
#include "bn_regular_bg_items_victory.h"
#include "bn_regular_bg_items_defeat.h"
#include "bn_regular_bg_items_draw.h"
#include "bn_regular_bg_items_pause.h"
#include "bn_regular_bg_items_help.h"
extern "C" { volatile int pkmn_debug[32] = {}; }
namespace pkmn {
enum Screen { TITLE, SELECTOR, COUNTDOWN, FIGHTING, PAUSED, ROUND_END, RESULT, HELP, RESUME_COUNTDOWN };
static const bn::sprite_item* const bodies[]={&bn::sprite_items::pk_pikachu,&bn::sprite_items::pk_charmander,&bn::sprite_items::pk_squirtle,&bn::sprite_items::pk_bulbasaur,&bn::sprite_items::pk_eevee,&bn::sprite_items::pk_meowth};
static const bn::sprite_item* const shots[]={&bn::sprite_items::shot_0,&bn::sprite_items::shot_1,&bn::sprite_items::shot_2,&bn::sprite_items::shot_3,&bn::sprite_items::shot_4,&bn::sprite_items::shot_5};
static const bn::sprite_item* const waves[]={&bn::sprite_items::wave_0,&bn::sprite_items::wave_1,&bn::sprite_items::wave_2,&bn::sprite_items::wave_3,&bn::sprite_items::wave_4,&bn::sprite_items::wave_5};
static Input input(){Input i;const bool held[]={bn::keypad::a_held(),bn::keypad::b_held(),bn::keypad::select_held(),bn::keypad::start_held(),bn::keypad::right_held(),bn::keypad::left_held(),bn::keypad::up_held(),bn::keypad::down_held(),bn::keypad::r_held(),bn::keypad::l_held()};const bool press[]={bn::keypad::a_pressed(),bn::keypad::b_pressed(),bn::keypad::select_pressed(),bn::keypad::start_pressed(),bn::keypad::right_pressed(),bn::keypad::left_pressed(),bn::keypad::up_pressed(),bn::keypad::down_pressed(),bn::keypad::r_pressed(),bn::keypad::l_pressed()};for(int n=0;n<10;n++){if(held[n])i.held|=1<<n;if(press[n])i.pressed|=1<<n;}return i;}
static CombatCore core;static CpuController ai;static constexpr bn::sprite_font font(bn::sprite_items::pkfont);
class Renderer {
 int group=-1,key=-1;bn::optional<bn::regular_bg_ptr> bg,ensemble;
 bn::optional<bn::sprite_ptr> fighters[2],shield[2],bullet[4],bars[6],focus,sparks[5];
 bn::optional<bn::sprite_ptr> shadows[2],portraits[6];
 struct Particle{bn::optional<bn::sprite_ptr> sprite;int x=0,y=0,born=0,kind=0;};Particle particles[6];
 int tile[2]={-1,-1},portrait_tile[6]={-1,-1,-1,-1,-1,-1},last_fx_tick=-1,result_started=0,ko_age=0;
 void pose(int i,int ch,Pose p){if(tile[i]!=p.frame){fighters[i]->set_tiles(bodies[ch]->tiles_item(),p.frame);tile[i]=p.frame;}pkmn_debug[25+i]=p.frame;pkmn_debug[27+i]=p.clip;}
 void particle(int x,int y,int kind){for(auto&p:particles)if(!p.sprite){p.x=x;p.y=y;p.kind=kind;p.born=core.tick;p.sprite=(kind?bn::sprite_items::impact:bn::sprite_items::dust).create_sprite(0,0);p.sprite->set_z_order(-2);break;}}
 bn::sprite_text_generator text{font};bn::vector<bn::sprite_ptr,96> labels;
 void line(int x,int y,bn::string_view value,bool center=false,int scale=1){int first=labels.size();if(center)text.set_center_alignment();else text.set_left_alignment();text.generate(x-120,y-80,value,labels);if(scale>1){if(scale>2)scale=2;for(int i=first;i<labels.size();i++){auto&sp=labels[i];sp.set_position(x-120+(sp.x()-(x-120))*scale,y-80);sp.set_scale(scale);}}}
 void clear(){labels.clear();bg.reset();ensemble.reset();focus.reset();for(auto&v:fighters)v.reset();for(auto&v:shield)v.reset();for(auto&v:bullet)v.reset();for(auto&v:bars)v.reset();for(auto&v:sparks)v.reset();for(auto&v:shadows)v.reset();for(auto&v:portraits)v.reset();for(auto&v:particles)v.sprite.reset();for(auto&v:tile)v=-1;for(auto&v:portrait_tile)v=-1;last_fx_tick=-1;ko_age=0;key=-1;}
 void backdrop(const bn::regular_bg_item&item){bg=item.create_bg(8,48);bg->set_priority(3);}
public:
 void draw(Screen screen,int selected,int cpu,int difficulty,int choice,bool level,int count,int frame){
  int next=screen==TITLE?0:screen==SELECTOR?1:(screen==COUNTDOWN||screen==FIGHTING||screen==ROUND_END||screen==RESUME_COUNTDOWN)?2:screen==PAUSED?3:screen==RESULT?4:5;
  if(next!=group){clear();group=next;
   if(group==0){backdrop(bn::regular_bg_items::title);ensemble=bn::regular_bg_items::ensemble.create_bg(8,48);ensemble->set_priority(2);for(int i=0;i<5;i++)sparks[i]=bn::sprite_items::spark.create_sprite(-104+i*48,-24+(i%2)*46);}
   if(group==1){backdrop(bn::regular_bg_items::selector);focus=bn::sprite_items::focus.create_sprite(0,0);for(int i=0;i<6;i++){portraits[i]=bodies[i]->create_sprite(0,0);portraits[i]->set_scale(bn::fixed(.7));portraits[i]->set_position(44+(i%3)*76-120,bn::fixed(55)+(i/3)*51-26*bn::fixed(.7)-80);}}
   if(group==2){backdrop(bn::regular_bg_items::arena);for(int i=0;i<2;i++){fighters[i]=bodies[i?cpu:selected]->create_sprite(0,0);shield[i]=bn::sprite_items::shield.create_sprite(0,0);shield[i]->set_visible(false);shadows[i]=bn::sprite_items::ground_shadow.create_sprite(0,0);shadows[i]->set_z_order(5);for(int j=0;j<3;j++)bars[i*3+j]=bn::sprite_items::bars.create_sprite(0,0,j);}}
   if(group==3){backdrop(bn::regular_bg_items::pause);focus=bn::sprite_items::spark.create_sprite(0,0);}
   if(group==4){result_started=frame;backdrop(core.outcome==1?bn::regular_bg_items::victory:core.outcome==2?bn::regular_bg_items::defeat:bn::regular_bg_items::draw);fighters[0]=bodies[selected]->create_sprite(0,0);focus=bn::sprite_items::focus.create_sprite(0,0);}
   if(group==5)backdrop(bn::regular_bg_items::help);
  }
  if(group==0){int b=(frame/18)%8;ensemble->set_y(47+(b<4?b:7-b));for(int i=0;i<5;i++){sparks[i]->set_visible((frame/12+i)%4<2);sparks[i]->set_rotation_angle((frame*2+i*30)%360);}}
  if(group==1){for(int i=0;i<6;i++){auto p=loop_pose(i,CLIP_IDLE,frame+i*11);if(portrait_tile[i]!=p.frame){portraits[i]->set_tiles(bodies[i]->tiles_item(),p.frame);portrait_tile[i]=p.frame;}}focus->set_visible(!level);focus->set_scale(bn::fixed(72)/64,bn::fixed(49)/64);focus->set_position(44+(selected%3)*76-120,bn::fixed(44.5)+(selected/3)*51-80);}
  if(group==2){
   if(screen==ROUND_END)++ko_age;else ko_age=0;
   bool new_tick=core.tick!=last_fx_tick;last_fx_tick=core.tick;
   for(int i=0;i<2;i++){auto&v=core.f[i];auto&sp=*fighters[i];int age=core.tick-v.began-1;
    Pose current=fighter_pose(core,i);
    if(screen==COUNTDOWN||screen==RESUME_COUNTDOWN)current=loop_pose(v.character,CLIP_IDLE,frame+i*19);
    if(!v.hp)current=loop_pose(v.character,ko_age<24?CLIP_FAINT:CLIP_DOWN,ko_age,false);
    else if(screen==ROUND_END)current=loop_pose(v.character,CLIP_VICTORY,ko_age);
    pose(i,v.character,current);bn::fixed sy=1,sx=1,offset=0;
    if(v.action==MELEE)offset=v.face*(age<characters[v.character].melee_start?-1:3);
    else if(v.action==FIRE&&age>=characters[v.character].fire_start)offset=-v.face*2;
    if(current.clip==CLIP_LAND){sy=bn::fixed(.9);sx=bn::fixed(1.06);}
    int feet=v.y/256;if(screen==ROUND_END){feet+=ko_age*ko_age/8;if(feet>world_floor_y)feet=world_floor_y;}
    sp.set_scale(sx,sy);sp.set_rotation_angle(0);sp.set_horizontal_flip(v.face<0);sp.set_position(bn::fixed(v.x)/256-120+offset,feet-26*sy-80);sp.set_visible(!v.hp||!(core.tick<v.invuln_until&&(core.tick/3)%2));
    int height=world_floor_y-v.y/256;if(height<0)height=0;bn::fixed shadow_scale=bn::fixed(1)-bn::fixed(height)/80;if(shadow_scale<bn::fixed(.45))shadow_scale=bn::fixed(.45);
    shadows[i]->set_scale(shadow_scale);shadows[i]->set_position(bn::fixed(v.x)/256-120,world_floor_y-80);
    bool charge=v.action==SUPER&&age<super_startup_ticks;shield[i]->set_visible(v.action==BLOCK||v.action==BLOCKSTUN||charge);shield[i]->set_scale(charge?bn::fixed(1)+bn::fixed((core.tick/3)%3)/8:bn::fixed(1));shield[i]->set_position(bn::fixed(v.x)/256+v.face*11-120,bn::fixed(v.y)/256-17-80);shield[i]->set_horizontal_flip(v.face<0);
    if(new_tick){if(v.landed_at==core.tick-1||v.took_off_at==core.tick-1)particle(v.x/256,world_floor_y-4,0);else if(v.action==NONE&&core.grounded(v)&&v.vx&&core.tick%12==0)particle(v.x/256-v.face*8,world_floor_y-3,0);if(v.began==core.tick&&(v.action==HITSTUN||v.action==BLOCKSTUN||v.action==GUARD_BREAK))particle(v.x/256-v.face*6,v.y/256-18,1);}
    int values[]={v.hp,v.sta,v.sp},maxes[]={characters[v.character].hp,characters[v.character].sta*60,100};for(int j=0;j<3;j++){auto&bar=*bars[i*3+j];bn::fixed width=bn::fixed(71)*values[j]/maxes[j];bar.set_visible(values[j]>0);if(values[j]>0){bar.set_scale(width/64,bn::fixed(.125));bar.set_position((i?163:25)+width/2-120,18+j*9-80);}}}
   int live=0;for(auto&p:particles)if(p.sprite){int age=core.tick-p.born;if(age>=12){p.sprite.reset();continue;}++live;p.sprite->set_tiles((p.kind?bn::sprite_items::impact:bn::sprite_items::dust).tiles_item(),age/3);p.sprite->set_position(p.x-120,p.y-80-(p.kind?0:age/4));}pkmn_debug[29]=live;
   for(int i=0;i<4;i++){const auto&b=core.p[i];if(b.active){if(!bullet[i])bullet[i]=(b.super?waves[core.f[b.owner].character]:shots[core.f[b.owner].character])->create_sprite(0,0);bullet[i]->set_position(bn::fixed(b.x)/256-120,bn::fixed(b.y)/256-80);bullet[i]->set_horizontal_flip(b.vx<0);}else bullet[i].reset();}}
  if(group==3)focus->set_position(-83,-22+choice*20);
  if(group==4){auto&sp=*fighters[0];int age=frame-result_started;Pose current=loop_pose(selected,core.outcome==1?CLIP_VICTORY:core.outcome==2?(age<24?CLIP_FAINT:CLIP_DOWN):CLIP_IDLE,age,core.outcome!=2);pose(0,selected,current);bn::fixed scale=core.outcome==2?bn::fixed(1):bn::fixed(1.25);sp.set_scale(scale);sp.set_rotation_angle(0);sp.set_position(0,120-26*scale-80);focus->set_scale(bn::fixed(104)/64,bn::fixed(18)/64);focus->set_position((choice?180:60)-120,145-80);}

  int seconds=(core.remaining+59)/60,charge=(core.f[0].action==SUPER||core.f[1].action==SUPER)?1:0;
  int newkey=screen*100000+selected*10000+difficulty*1000+choice*100+(level?50:0)+(group==2?((screen==COUNTDOWN||screen==RESUME_COUNTDOWN)?count/60:seconds)+charge*200+(core.f[0].sp==100?400:0)+(core.f[1].sp==100?800:0):0)+(group==4?core.outcome*10000:0);
  if(newkey!=key){key=newkey;labels.clear();
   if(group==1){bn::string<64> stats="HP ";stats+=bn::to_string<8>(characters[selected].hp);stats+=" STA ";stats+=bn::to_string<8>(characters[selected].sta);stats+=" DMG ";stats+=bn::to_string<8>(characters[selected].super_damage);line(120,127,stats,true);bn::string<32> diff=level?"> LEVEL: ":"LEVEL: ";diff+=difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD";line(120,140,diff,true);}
   if(group==2){line(6,8,characters[selected].name);line(144,8,characters[cpu].name);line(120,14,bn::to_string<8>(seconds),true,2);if(screen==COUNTDOWN)line(120,71,bn::to_string<8>((count+59)/60),true,3);if(screen==RESUME_COUNTDOWN)line(120,71,"READY",true,2);if(screen==FIGHTING&&charge)line(120,49,"SUPER! JUMP OR SQUEEZE",true);else if(screen==FIGHTING){if(core.f[0].sp==100)line(48,47,"R READY",true);if(core.f[1].sp==100)line(192,47,"READY",true);}}
   if(group==4){bn::string<40> result=characters[selected].name;result+=core.outcome==1?" WINS!":core.outcome==2?" WILL RETURN!":" - DRAW";line(120,54,result,true);}
  }
 }
};
[[noreturn]] void run_app(){static Renderer renderer;Screen screen=TITLE,help_return=TITLE;int selected=0,cpu=1,difficulty=1,choice=0,count=0,frame=0;bool level=false;uint32_t seed=0x504b4d4e;int peak=0,missed=0;
 auto start_match=[&](){seed=seed*1664525u+1013904223u;core.reset(selected,cpu,seed);ai.reset(difficulty,seed^0xa5a5a5a5u);count=match_countdown_display_ticks;screen=COUNTDOWN;choice=0;};
 while(true){Input keys=input();++frame;bool confirm=keys.pressed&(A|START);
  if(screen==TITLE){if(confirm){screen=SELECTOR;bn::sound_items::menu.play();}else if(keys.pressed&SELECT){help_return=TITLE;screen=HELP;}}
  else if(screen==HELP){if(keys.pressed&(A|B|START|SELECT))screen=help_return;}
  else if(screen==SELECTOR){if(keys.pressed&B){screen=TITLE;level=false;}else if(keys.pressed&SELECT){help_return=SELECTOR;screen=HELP;}else if(confirm){seed^=uint32_t(frame);cpu=(selected+1+int(seed%5))%6;start_match();bn::sound_items::menu.play();}
   else if(level){if(keys.pressed&UP)level=false;if(keys.pressed&LEFT)difficulty=(difficulty+2)%3;if(keys.pressed&RIGHT)difficulty=(difficulty+1)%3;}
   else{if(keys.pressed&LEFT)selected=(selected+5)%6;if(keys.pressed&RIGHT)selected=(selected+1)%6;if(keys.pressed&UP)selected=(selected+3)%6;if(keys.pressed&DOWN){if(selected>=3)level=true;else selected+=3;}}}
  else if(screen==COUNTDOWN||screen==RESUME_COUNTDOWN){core.f[0].pending=core.f[1].pending=0;if(--count<=0){screen=FIGHTING;bn::sound_items::menu.play();}}
  else if(screen==FIGHTING){if(keys.pressed&START){screen=PAUSED;choice=0;core.f[0].pending=core.f[1].pending=0;}else{core.frame(keys,ai.input(core));if(core.feedback&SUPER_FIRED)bn::sound_items::superhit.play();else if(core.feedback&HIT)bn::sound_items::hit.play();else if(core.feedback&BLOCKED)bn::sound_items::block.play();else if(core.feedback&SHOT)bn::sound_items::fire.play();else if(core.feedback&JUMPED)bn::sound_items::jump.play();else if(core.feedback&SWUNG)bn::sound_items::swing.play();else if(core.feedback&CHARGED)bn::sound_items::menu.play();if(core.outcome){screen=ROUND_END;count=match_result_delay_display_ticks;}}}
  else if(screen==PAUSED){if(keys.pressed&(UP|LEFT))choice=(choice+3)%4;if(keys.pressed&(DOWN|RIGHT))choice=(choice+1)%4;if((keys.pressed&B)||(keys.pressed&START)){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}else if(confirm){if(choice==0){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}else if(choice==1)start_match();else if(choice==2){screen=SELECTOR;level=false;}else{help_return=PAUSED;screen=HELP;}}}
  else if(screen==ROUND_END){if(--count<=0){screen=RESULT;choice=0;bn::sound_items::menu.play();}}
  else if(screen==RESULT){if(keys.pressed&(LEFT|RIGHT|UP|DOWN))choice=1-choice;if(keys.pressed&B){screen=SELECTOR;level=false;}else if(confirm){if(choice==0)start_match();else{screen=SELECTOR;level=false;}}}
  renderer.draw(screen,selected,cpu,difficulty,choice,level,count,frame);
  int usage=bn::core::last_cpu_usage().data();if(usage>peak)peak=usage;missed+=bn::core::last_missed_frames();
  pkmn_debug[0]=0x504b4d4e;pkmn_debug[1]=screen;pkmn_debug[2]=selected;pkmn_debug[3]=cpu;pkmn_debug[4]=difficulty;pkmn_debug[5]=core.tick;pkmn_debug[6]=core.f[0].hp;pkmn_debug[7]=core.f[1].hp;pkmn_debug[8]=core.f[0].sta;pkmn_debug[9]=core.f[1].sta;pkmn_debug[10]=core.f[0].sp;pkmn_debug[11]=core.f[1].sp;pkmn_debug[12]=core.f[0].x;pkmn_debug[13]=core.f[1].x;pkmn_debug[14]=core.f[0].y;pkmn_debug[15]=core.f[1].y;pkmn_debug[16]=core.f[0].action;pkmn_debug[17]=core.f[1].action;pkmn_debug[18]=core.remaining;pkmn_debug[19]=core.hitstop;pkmn_debug[20]=core.outcome;pkmn_debug[21]=peak;pkmn_debug[22]=missed;pkmn_debug[23]=frame;pkmn_debug[24]=seed;
  bn::core::update();
 }
}
}
