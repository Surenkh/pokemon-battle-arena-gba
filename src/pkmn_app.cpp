#include "pkmn_app.h"
#include "pkmn_progress_store.h"
#include "pkmn_mastery.h"
#include "pkmn_combat_core.h"
#include "pkmn_animation.h"
#include "pkmn_effect_animation.h"
#include "pkmn_combat_feel.h"
#include "pkmn_combat_audio.h"
#include "pkmn_practice.h"
#include "pkmn_round_presentation.h"
#include "pkmn_presentation_audio.h"
#include "bn_sprite_palette_ptr.h"
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
#include "bn_sprite_items_pk_machop.h"
#include "bn_sprite_items_pk_jigglypuff.h"
#include "bn_sprite_items_pk_psyduck.h"
#include "bn_sprite_items_shot_0.h"
#include "bn_sprite_items_shot_1.h"
#include "bn_sprite_items_shot_2.h"
#include "bn_sprite_items_shot_3.h"
#include "bn_sprite_items_shot_4.h"
#include "bn_sprite_items_shot_5.h"
#include "bn_sprite_items_shot_6.h"
#include "bn_sprite_items_shot_7.h"
#include "bn_sprite_items_shot_8.h"
#include "bn_sprite_items_wave_0.h"
#include "bn_sprite_items_wave_1.h"
#include "bn_sprite_items_wave_2.h"
#include "bn_sprite_items_wave_3.h"
#include "bn_sprite_items_wave_4.h"
#include "bn_sprite_items_wave_5.h"
#include "bn_sprite_items_wave_6.h"
#include "bn_sprite_items_wave_7.h"
#include "bn_sprite_items_wave_8.h"
#include "bn_sprite_items_hit_0.h"
#include "bn_sprite_items_hit_1.h"
#include "bn_sprite_items_hit_2.h"
#include "bn_sprite_items_hit_3.h"
#include "bn_sprite_items_hit_4.h"
#include "bn_sprite_items_hit_5.h"
#include "bn_sprite_items_hit_6.h"
#include "bn_sprite_items_hit_7.h"
#include "bn_sprite_items_hit_8.h"
#include "bn_sprite_items_cast_0.h"
#include "bn_sprite_items_cast_1.h"
#include "bn_sprite_items_cast_2.h"
#include "bn_sprite_items_cast_3.h"
#include "bn_sprite_items_cast_4.h"
#include "bn_sprite_items_cast_5.h"
#include "bn_sprite_items_cast_6.h"
#include "bn_sprite_items_cast_7.h"
#include "bn_sprite_items_cast_8.h"
#include "bn_sprite_items_guard_0.h"
#include "bn_sprite_items_guard_1.h"
#include "bn_sprite_items_guard_2.h"
#include "bn_sprite_items_guard_3.h"
#include "bn_sprite_items_guard_4.h"
#include "bn_sprite_items_guard_5.h"
#include "bn_sprite_items_guard_6.h"
#include "bn_sprite_items_guard_7.h"
#include "bn_sprite_items_guard_8.h"
#include "bn_sprite_items_focus.h"
#include "bn_sprite_items_nav_markers.h"
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
#include "bn_regular_bg_items_arena_volcanic.h"
#include "bn_regular_bg_items_arena_shoreline.h"
#include "bn_regular_bg_items_practice_volcanic.h"
#include "bn_regular_bg_items_practice_shoreline.h"
#include "bn_sprite_items_mastery_badge.h"
#include "bn_regular_bg_items_practice_arena.h"
#include "bn_regular_bg_items_practice_setup.h"
#include "bn_regular_bg_items_fighter_guide.h"
#include "bn_regular_bg_position_hbe_ptr.h"
#include "bn_array.h"
#include "bn_regular_bg_items_victory.h"
#include "bn_regular_bg_items_defeat.h"
#include "bn_regular_bg_items_draw.h"
#include "bn_regular_bg_items_pause.h"
#include "bn_regular_bg_items_pause_no_retry.h"
#include "bn_regular_bg_items_help.h"
#include "bn_regular_bg_items_audio_options.h"
#include "bn_regular_bg_items_arcade_panel.h"
#include "bn_regular_bg_items_mastery_panel.h"
#include "bn_regular_bg_items_arcade_selector.h"
#include "bn_regular_bg_items_mode_menu.h"
#include "bn_sprite_items_recall_ball.h"
#include "bn_sprite_items_recall_beam.h"
#include "bn_sprite_items_recall_glint.h"
extern "C" { volatile int pkmn_practice_debug[16] = {}; volatile int pkmn_mastery_debug[32] = {}; volatile int pkmn_arcade_debug[32] = {}; volatile int pkmn_debug[32] = {}; volatile int pkmn_feel_debug[16] = {}; volatile int pkmn_depth_debug[32] = {}; volatile int pkmn_cpu_debug[32] = {}; volatile int pkmn_presentation_debug[32] = {}; }
namespace pkmn {
enum Screen { TITLE, SELECTOR, COUNTDOWN, FIGHTING, PAUSED, ROUND_END, RESULT, HELP, RESUME_COUNTDOWN, PRACTICE_SETUP, FIGHTER_GUIDE, AUDIO_OPTIONS, ARCADE_RESULT, CHAMPION, RECORDS, MODE_MENU, MASTERY };
static constexpr int mode_row_y[]={41,56,71,96,108,120,132};
static const bn::sprite_item* const bodies[]={&bn::sprite_items::pk_pikachu,&bn::sprite_items::pk_charmander,&bn::sprite_items::pk_squirtle,&bn::sprite_items::pk_bulbasaur,&bn::sprite_items::pk_eevee,&bn::sprite_items::pk_meowth,&bn::sprite_items::pk_psyduck,&bn::sprite_items::pk_jigglypuff,&bn::sprite_items::pk_machop};
static const bn::sprite_item* const shots[]={&bn::sprite_items::shot_0,&bn::sprite_items::shot_1,&bn::sprite_items::shot_2,&bn::sprite_items::shot_3,&bn::sprite_items::shot_4,&bn::sprite_items::shot_5,&bn::sprite_items::shot_6,&bn::sprite_items::shot_7,&bn::sprite_items::shot_8};
static const bn::sprite_item* const waves[]={&bn::sprite_items::wave_0,&bn::sprite_items::wave_1,&bn::sprite_items::wave_2,&bn::sprite_items::wave_3,&bn::sprite_items::wave_4,&bn::sprite_items::wave_5,&bn::sprite_items::wave_6,&bn::sprite_items::wave_7,&bn::sprite_items::wave_8};
static const bn::sprite_item* const hits[]={&bn::sprite_items::hit_0,&bn::sprite_items::hit_1,&bn::sprite_items::hit_2,&bn::sprite_items::hit_3,&bn::sprite_items::hit_4,&bn::sprite_items::hit_5,&bn::sprite_items::hit_6,&bn::sprite_items::hit_7,&bn::sprite_items::hit_8};
static const bn::sprite_item* const casts[]={&bn::sprite_items::cast_0,&bn::sprite_items::cast_1,&bn::sprite_items::cast_2,&bn::sprite_items::cast_3,&bn::sprite_items::cast_4,&bn::sprite_items::cast_5,&bn::sprite_items::cast_6,&bn::sprite_items::cast_7,&bn::sprite_items::cast_8};
static const bn::sprite_item* const guards[]={&bn::sprite_items::guard_0,&bn::sprite_items::guard_1,&bn::sprite_items::guard_2,&bn::sprite_items::guard_3,&bn::sprite_items::guard_4,&bn::sprite_items::guard_5,&bn::sprite_items::guard_6,&bn::sprite_items::guard_7,&bn::sprite_items::guard_8};
static Input input(){Input i;const bool held[]={bn::keypad::a_held(),bn::keypad::b_held(),bn::keypad::select_held(),bn::keypad::start_held(),bn::keypad::right_held(),bn::keypad::left_held(),bn::keypad::up_held(),bn::keypad::down_held(),bn::keypad::r_held(),bn::keypad::l_held()};const bool press[]={bn::keypad::a_pressed(),bn::keypad::b_pressed(),bn::keypad::select_pressed(),bn::keypad::start_pressed(),bn::keypad::right_pressed(),bn::keypad::left_pressed(),bn::keypad::up_pressed(),bn::keypad::down_pressed(),bn::keypad::r_pressed(),bn::keypad::l_pressed()};for(int n=0;n<10;n++){if(held[n])i.held|=1<<n;if(press[n])i.pressed|=1<<n;}return i;}
static ArcadeRun arcade;static Progress progress;static ProgressStore storage;static bool arcade_mode=false;static int mode_row=0;
static MasterySession mastery_tracker;static bool mastery_dirty=false,medal_earned=false;static int chosen_arena=WOODLAND,battle_arena=WOODLAND,match_medals=0;
static RoundPresentation round;static AudioSettings sound_settings;static PresentationAudio ambience;
static CombatCore core;static CpuController ai;static CombatFeel feel;static CombatAudio audio;static constexpr bn::sprite_font font(bn::sprite_items::pkfont);
class Renderer {
 int group=-1,key=-1;bn::optional<bn::regular_bg_ptr> bg,ensemble;
 bn::optional<bn::regular_bg_position_hbe_ptr> horizontal_shake,vertical_shake;
 bn::array<bn::fixed,160> shake_x{},shake_y{};int last_camera_x=99,last_camera_y=99,hbe_setup_frame=0,last_canopy_step=-1;
 bn::optional<bn::sprite_ptr> fighters[2],shield[2],bullet[4],bars[6],focus,focus_right,marker,sparks[5];
 bn::optional<bn::sprite_ptr> shadows[2],portraits[character_count],badges[character_count],charging[2];
 bn::optional<bn::sprite_ptr> recall_ball,recall_beam,recall_glint;int recall_tile=-1,glint_tile=-1;
 int charge_tile[2]={-1,-1},flash_state[2]={-1,-1};uint32_t presented_events=0;
 int last_practice_damage=-1,last_practice_chain=-1,last_practice_best=-1,last_practice_feedback=-1,active_draw_frames=0,scene_frames=0;
 enum Effect { DUST, IMPACT, CAST, GUARD };
 struct Particle{bn::optional<bn::sprite_ptr> sprite;int x=0,y=0,born=0,kind=0,character=0,frame=-1;};Particle particles[12];
 int bullet_tile[4]={-1,-1,-1,-1},bullet_key[4]={-1,-1,-1,-1};
 int tile[2]={-1,-1},portrait_tile[character_count]={},last_fx_tick=-1,result_started=0,ko_age=0;
 void pose(int i,int ch,Pose p){if(tile[i]!=p.frame){fighters[i]->set_tiles(bodies[ch]->tiles_item(),p.frame);tile[i]=p.frame;}pkmn_debug[25+i]=p.frame;pkmn_debug[27+i]=p.clip;}
 const bn::sprite_item& effect_item(int kind,int character)const{return kind==DUST?bn::sprite_items::dust:*(kind==IMPACT?hits[character]:kind==CAST?casts[character]:guards[character]);}
 void particle(int x,int y,int kind,int character=0,bool flip=false){
  Particle* slot=nullptr;
  for(auto&p:particles)if(!p.sprite||core.tick-p.born>=effect_lifetime_ticks){p.sprite.reset();slot=&p;break;}
  if(!slot&&kind!=DUST)for(auto&p:particles)if(p.kind==DUST){p.sprite.reset();slot=&p;break;}
  if(!slot)return;
  auto&p=*slot;p.x=x;p.y=y;p.kind=kind;p.character=character;p.born=core.tick;p.frame=0;
  p.sprite=effect_item(kind,character).create_sprite(0,0);p.sprite->set_z_order(-2);p.sprite->set_bg_priority(2);p.sprite->set_horizontal_flip(flip);
  if(kind!=DUST)pkmn_debug[31]|=1<<(character+(kind==CAST?8:kind==GUARD?16:0));
 }
 bn::sprite_text_generator text{font};bn::vector<bn::sprite_ptr,64> labels;bn::vector<bn::sprite_ptr,8> hud_names;bn::vector<bn::sprite_ptr,16> practice_labels;
 void line(int x,int y,bn::string_view value,bool center=false,int scale=1,bn::ivector<bn::sprite_ptr>* output=nullptr){auto& target=output?*output:static_cast<bn::ivector<bn::sprite_ptr>&>(labels);int first=target.size();if(center)text.set_center_alignment();else text.set_left_alignment();text.generate(x-120,y-80,value,target);for(int i=first;i<target.size();i++)target[i].set_bg_priority(0);if(scale>1){if(scale>2)scale=2;for(int i=first;i<target.size();i++){auto&sp=target[i];sp.set_position(x-120+(sp.x()-(x-120))*scale,y-80);sp.set_scale(scale);}}}
 void clear(){for(auto&v:fighters)if(v){auto palette=v->palette();palette.set_fade(bn::color(31,31,31),0);}recall_ball.reset();recall_beam.reset();recall_glint.reset();recall_tile=glint_tile=-1;labels.clear();hud_names.clear();practice_labels.clear();active_draw_frames=scene_frames=0;last_practice_damage=last_practice_chain=last_practice_best=last_practice_feedback=-1;horizontal_shake.reset();vertical_shake.reset();bg.reset();ensemble.reset();last_camera_x=last_camera_y=99;hbe_setup_frame=0;last_canopy_step=-1;for(auto&v:charging)v.reset();charge_tile[0]=charge_tile[1]=-1;flash_state[0]=flash_state[1]=-1;presented_events=core.event_frame;focus.reset();focus_right.reset();marker.reset();for(auto&v:fighters)v.reset();for(auto&v:shield)v.reset();for(auto&v:bullet)v.reset();for(auto&v:bars)v.reset();for(auto&v:sparks)v.reset();for(auto&v:shadows)v.reset();for(auto&v:portraits)v.reset();for(auto&v:badges)v.reset();for(auto&v:particles)v.sprite.reset();for(auto&v:tile)v=-1;for(auto&v:portrait_tile)v=-1;for(auto&v:bullet_tile)v=-1;for(auto&v:bullet_key)v=-1;last_fx_tick=-1;ko_age=0;key=-1;pkmn_debug[29]=pkmn_debug[30]=pkmn_debug[31]=0;}
 void focus_at(int x,int y,int width,int height){
  // A wide affine sprite clips at twice its source width. Join two halves.
  if(width>128){
   if(!focus_right){focus_right=bn::sprite_items::focus.create_sprite(0,0,2);focus->set_tiles(bn::sprite_items::focus.tiles_item(),1);}
   focus->set_scale(bn::fixed(width)/128,bn::fixed(height)/32);focus_right->set_scale(bn::fixed(width)/128,bn::fixed(height)/32);
   focus->set_position(x-width/4-120,y-80);focus_right->set_position(x+width/4-120,y-80);
  }else{
   if(focus_right){focus_right.reset();focus->set_tiles(bn::sprite_items::focus.tiles_item(),0);}
   focus->set_scale(bn::fixed(width)/64,bn::fixed(height)/32);focus->set_position(x-120,y-80);
  }
 }
 void backdrop(const bn::regular_bg_item&item){bg=item.create_bg(8,48);bg->set_priority(3);}
public:
 void reset_scene(){group=-1;}
 void draw(Screen screen,int selected,int cpu,int difficulty,int choice,int selector_row,int count,int frame,bool practice,int practice_row,const PracticeSession& trainer,int audio_row){
  bool level=selector_row==1;
  int next=screen==TITLE?0:screen==SELECTOR?1:(screen==COUNTDOWN||screen==FIGHTING||screen==ROUND_END||screen==RESUME_COUNTDOWN)?2:screen==PAUSED?3:screen==RESULT?4:screen==PRACTICE_SETUP?6:screen==FIGHTER_GUIDE?7:screen==AUDIO_OPTIONS?8:screen==ARCADE_RESULT?9:screen==CHAMPION?10:screen==RECORDS?11:screen==MODE_MENU?12:screen==MASTERY?13:5;
  if(next!=group){clear();group=next;
   if(group==0){backdrop(bn::regular_bg_items::title);ensemble=bn::regular_bg_items::ensemble.create_bg(8,48);ensemble->set_priority(2);for(int i=0;i<5;i++)sparks[i]=bn::sprite_items::spark.create_sprite(-104+i*48,-24+(i%2)*46);}
   if(group==1||group==6){backdrop(group==6?bn::regular_bg_items::practice_setup:arcade_mode?bn::regular_bg_items::arcade_selector:bn::regular_bg_items::selector);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0);}
   if(group==2){static const bn::regular_bg_item* const stages[]={&bn::regular_bg_items::arena,&bn::regular_bg_items::arena_volcanic,&bn::regular_bg_items::arena_shoreline};static const bn::regular_bg_item* const training_stages[]={&bn::regular_bg_items::practice_arena,&bn::regular_bg_items::practice_volcanic,&bn::regular_bg_items::practice_shoreline};backdrop(*(practice?training_stages[battle_arena]:stages[battle_arena]));shake_x.fill(0);shake_y.fill(0);}
   if(group==3){backdrop(arcade.active&&!arcade.continues?bn::regular_bg_items::pause_no_retry:bn::regular_bg_items::pause);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==4){result_started=frame;backdrop(core.outcome==1?bn::regular_bg_items::victory:core.outcome==2?bn::regular_bg_items::defeat:bn::regular_bg_items::draw);if(core.outcome==2)recall_ball=bn::sprite_items::recall_ball.create_sprite(0,30);else fighters[0]=bodies[selected]->create_sprite(0,0);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group>=9&&group<=11)backdrop(bn::regular_bg_items::arcade_panel);
   if(group==13)backdrop(bn::regular_bg_items::mastery_panel);
   if(group==12){backdrop(bn::regular_bg_items::mode_menu);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==5)backdrop(bn::regular_bg_items::help);
   if(group==7)backdrop(bn::regular_bg_items::fighter_guide);
   if(group==8){backdrop(bn::regular_bg_items::audio_options);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
  }
  if(group==0){int b=(frame/18)%8;ensemble->set_y(47+(b<4?b:7-b));for(int i=0;i<5;i++){sparks[i]->set_visible((frame/12+i)%4<2);sparks[i]->set_rotation_angle((frame*2+i*30)%360);}}
  if(group==1||group==6){
   for(int i=0;i<character_count;i++){
    // Two portraits per display frame keep music and uploads within budget.
    if(!portraits[i]&&scene_frames>=i/2){portraits[i]=bodies[i]->create_sprite(0,0);portraits[i]->set_scale(bn::fixed(.25));portraits[i]->set_position(44+(i%3)*76-120,bn::fixed(31)+(i/3)*22-26*bn::fixed(.25)-80);badges[i]=bn::sprite_items::mastery_badge.create_sprite(72+(i%3)*76-120,23+(i/3)*22-80,progress.medals(i));badges[i]->set_bg_priority(0);}
    if(!portraits[i])continue;auto p=loop_pose(i,CLIP_IDLE,frame+i*11);if(portrait_tile[i]!=p.frame){portraits[i]->set_tiles(bodies[i]->tiles_item(),p.frame);portrait_tile[i]=p.frame;}
   }
   int row=group==6?practice_row:selector_row;
   if(!row)focus_at(44+(selected%3)*76,28+(selected/3)*22,72,21);
   else focus_at(120,(group==1&&row==3)?131:79+row*13,224,12);
   marker->set_position(16+(selected%3)*76-120,24+(selected/3)*22-80);
  }
  if(group==2){
   // Separate roster teardown/background upload from actor/tile allocation.
   // The first countdown display frame shows the arena; actors follow next.
   ++hbe_setup_frame;
   if(hbe_setup_frame==2){for(int i=0;i<2;i++){fighters[i]=bodies[i?cpu:selected]->create_sprite(0,0);fighters[i]->set_bg_priority(2);shield[i]=bn::sprite_items::shield.create_sprite(0,0);shield[i]->set_visible(false);shield[i]->set_bg_priority(2);shadows[i]=bn::sprite_items::ground_shadow.create_sprite(0,0);shadows[i]->set_z_order(5);shadows[i]->set_bg_priority(2);for(int j=0;j<3;j++){bars[i*3+j]=bn::sprite_items::bars.create_sprite(0,0,j);bars[i*3+j]->set_bg_priority(0);}}}
   if(hbe_setup_frame<2)return;
   if(hbe_setup_frame==3)horizontal_shake=bn::regular_bg_position_hbe_ptr::create_horizontal(*bg,shake_x);
   if(hbe_setup_frame==4)vertical_shake=bn::regular_bg_position_hbe_ptr::create_vertical(*bg,shake_y);
   if(hbe_setup_frame==5){line(6,8,characters[selected].name,false,1,&hud_names);line(144,8,characters[cpu].name,false,1,&hud_names);}
   if(screen==FIGHTING||screen==ROUND_END)++active_draw_frames;
   const int camera_x=horizontal_shake&&vertical_shake?feel.dx:0,camera_y=horizontal_shake&&vertical_shake?feel.dy:0;
   // HBE deltas are hardware scroll values: negate the screen-space camera
   // displacement so the background and world sprites move together.
   const int canopy_step=screen==FIGHTING?frame/24:-1;
   if(horizontal_shake&&vertical_shake&&(last_camera_x!=camera_x||last_camera_y!=camera_y||last_canopy_step!=canopy_step)){for(int y=0;y<160;y++){bool world=y>=44&&y<(practice?123:142);int sway=arena_sway(battle_arena,y,canopy_step);shake_x[y]=world?-camera_x+sway:0;shake_y[y]=world?-camera_y:0;}horizontal_shake->reload_deltas_ref();vertical_shake->reload_deltas_ref();last_camera_x=camera_x;last_camera_y=camera_y;last_canopy_step=canopy_step;}
   ko_age=screen==ROUND_END?round.age:0;
   if(screen==ROUND_END&&ko_age==24)for(auto&p:particles)p.sprite.reset();
   bool new_tick=core.tick!=last_fx_tick;last_fx_tick=core.tick;
   for(int i=0;i<2;i++){auto&v=core.f[i];auto&sp=*fighters[i];int age=core.tick-v.began-1;
    Pose current=fighter_pose(core,i);
    if(screen==COUNTDOWN||screen==RESUME_COUNTDOWN)current=loop_pose(v.character,CLIP_IDLE,frame+i*19);
    if(screen==ROUND_END){
     if(i==round.loser||!v.hp)current=loop_pose(v.character,ko_age<24?CLIP_FAINT:CLIP_DOWN,ko_age,false);
     else current=loop_pose(v.character,core.outcome==3?CLIP_IDLE:CLIP_VICTORY,ko_age);
    }
    pose(i,v.character,current);bn::fixed sy=1,sx=1,offset=0;
    if(v.action==MELEE)offset=v.face*(age<characters[v.character].melee_start?-1:3);
    else if(v.action==FIRE&&age>=characters[v.character].fire_start)offset=-v.face*2;
    if(screen==ROUND_END)offset=0;
    if(current.clip==CLIP_LAND){sy=bn::fixed(.9);sx=bn::fixed(1.06);}
    int feet=v.y/256;if(screen==ROUND_END){feet+=ko_age*ko_age/8;if(feet>world_floor_y)feet=world_floor_y;}
    bool recalling=screen==ROUND_END&&round.recall&&i==round.loser;
    if(recalling)sx=sy=bn::fixed(round.body_scale_q8())/256;
    sp.set_scale(sx,sy);sp.set_rotation_angle(0);sp.set_horizontal_flip(v.face<0);
    bn::fixed center_y=recalling?bn::fixed(round.body_y_q8(feet-26))/256:bn::fixed(feet)-26*sy;
    sp.set_position(bn::fixed(v.x)/256-120+offset+camera_x,center_y-80+camera_y);
    sp.set_visible(screen==ROUND_END?round.body_visible(i):(!v.hp||feel.flash[i]||!(core.tick<v.invuln_until&&(core.tick/3)%2)));
    int tint=recalling?round.absorb_q8():(feel.flash[i]?77:0),tint_key=tint+(recalling?512:0);
    if(flash_state[i]!=tint_key){auto palette=sp.palette();palette.set_fade(recalling?bn::color(31,10,20):bn::color(31,31,31),bn::fixed(tint)/256);flash_state[i]=tint_key;}
    int height=world_floor_y-v.y/256;if(height<0)height=0;bn::fixed shadow_scale=bn::fixed(1)-bn::fixed(height)/80;if(shadow_scale<bn::fixed(.45))shadow_scale=bn::fixed(.45);
    if(recalling)shadow_scale*=round.age>=92?bn::fixed(.35):sx;
    shadows[i]->set_visible(!recalling||round.body_visible(i)||round.ball_visible());shadows[i]->set_scale(shadow_scale);shadows[i]->set_position(bn::fixed(v.x)/256-120+camera_x,world_floor_y-80+camera_y);
    bool charge=screen==FIGHTING&&v.action==SUPER&&age>=0&&age<super_startup_ticks;
    shield[i]->set_visible(screen==FIGHTING&&(v.action==BLOCK||v.action==BLOCKSTUN));shield[i]->set_scale(feel.blocked[i]?bn::fixed(1.08):bn::fixed(1));shield[i]->set_position(bn::fixed(v.x)/256+v.face*11-120+camera_x,bn::fixed(v.y)/256-17-80+camera_y);shield[i]->set_horizontal_flip(v.face<0);
    bool vine=screen==FIGHTING&&v.character==3&&v.action==MELEE&&age>=characters[3].melee_start-2&&age<characters[3].melee_start+characters[3].melee_active;
    if((charge||vine)&&!charging[i]){charging[i]=casts[v.character]->create_sprite(0,0);charging[i]->set_bg_priority(2);}
    if(charging[i])charging[i]->set_visible(charge||vine);
    if(charge||vine){int phase=charge?(age/3)%4:(age-characters[3].melee_start+2)%4;if(charge_tile[i]!=phase){charging[i]->set_tiles(casts[v.character]->tiles_item(),phase);charge_tile[i]=phase;}charging[i]->set_position(bn::fixed(v.x)/256+v.face*(vine?24:10)-120+camera_x,bn::fixed(v.y)/256-19-80+camera_y);charging[i]->set_horizontal_flip(v.face<0);charging[i]->set_scale(charge?bn::fixed(1)+bn::fixed(age/10)/8:bn::fixed(1));}

    if(new_tick&&screen==FIGHTING){if(v.landed_at==core.tick-1||v.took_off_at==core.tick-1)particle(v.x/256,world_floor_y-4,DUST);else if(v.action==NONE&&core.grounded(v)&&v.vx&&core.tick%12==0)particle(v.x/256-v.face*8,world_floor_y-3,DUST);}
    int values[]={v.hp,v.sta,v.sp},maxes[]={characters[v.character].hp,characters[v.character].sta*60,100};for(int j=0;j<3;j++){auto&bar=*bars[i*3+j];bn::fixed width=bn::fixed(71)*values[j]/maxes[j];bar.set_visible(values[j]>0);if(values[j]>0){if(width<1)width=1;bar.set_scale(width/64,bn::fixed(.125));bar.set_position((i?163:25)+width/2-120,18+j*9-80);}}}
   if(presented_events!=core.event_frame&&(screen==FIGHTING||screen==ROUND_END)){
    presented_events=core.event_frame;for(int n=0;n<core.event_count;n++){const auto&e=core.events[n];int ch=core.f[e.owner].character;
     if(e.kind==EV_HIT||e.kind==EV_BLOCK||e.kind==EV_BREAK)particle(e.x/256,e.y/256,e.kind==EV_BLOCK?GUARD:IMPACT,ch,core.f[e.owner].face<0);
     if(e.kind==EV_SHOT||e.kind==EV_RELEASE){particle(e.x/256,e.y/256,CAST,ch,core.f[e.owner].face<0);}
    }
   }
   for(int i=0;i<4;i++){
    const auto&b=core.p[i];
    if(!b.active){bullet[i].reset();bullet_tile[i]=bullet_key[i]=-1;continue;}
    const int character=core.f[b.owner].character,asset_key=character+(b.super?6:0),phase=projectile_effect_frame(core.tick,b.born);
    const auto&item=*(b.super?waves[character]:shots[character]);
    if(!bullet[i]||bullet_key[i]!=asset_key){bullet[i].reset();bullet[i]=item.create_sprite(0,0,phase);bullet_key[i]=asset_key;bullet_tile[i]=phase;bullet[i]->set_z_order(-3);bullet[i]->set_bg_priority(2);}
    else if(bullet_tile[i]!=phase){bullet[i]->set_tiles(item.tiles_item(),phase);bullet_tile[i]=phase;}
    bullet[i]->set_position(bn::fixed(b.x)/256-120+camera_x,bn::fixed(b.y)/256-80+camera_y);bullet[i]->set_horizontal_flip(b.vx<0);
    pkmn_debug[30]|=1<<(phase+(b.super?4:0));

   }
   if(screen==ROUND_END&&round.ball_visible()){
    if(!recall_ball){recall_ball=bn::sprite_items::recall_ball.create_sprite(0,0);recall_ball->set_bg_priority(2);recall_ball->set_z_order(-4);}
    int ball_frame=round.ball_frame();if(recall_tile!=ball_frame){recall_ball->set_tiles(bn::sprite_items::recall_ball.tiles_item(),ball_frame);recall_tile=ball_frame;}
    recall_ball->set_position(bn::fixed(round.ball_x_q8())/256-120,bn::fixed(round.ball_y_q8())/256-80);int angle=round.ball_angle()%360;if(angle<0)angle+=360;recall_ball->set_rotation_angle(angle);
    if(round.beam_visible()){
     if(!recall_beam){recall_beam=bn::sprite_items::recall_beam.create_sprite(0,0);recall_beam->set_bg_priority(2);recall_beam->set_z_order(-3);}
     int scale=round.body_scale_q8(),bottom=round.body_y_q8(94)+16*scale,height=(bottom-round.ball_y_q8())/256;if(height<4)height=4;
     recall_beam->set_scale(1,bn::fixed(height)/64);recall_beam->set_position(bn::fixed(round.ball_x_q8())/256-120,bn::fixed(round.ball_y_q8())/256+bn::fixed(height)/2-80);
    }else recall_beam.reset();
    if(round.glint_visible()){
     if(!recall_glint){recall_glint=bn::sprite_items::recall_glint.create_sprite(0,0);recall_glint->set_bg_priority(2);recall_glint->set_z_order(-5);}
     int f=(round.age-140)/4;if(f>3)f=3;if(glint_tile!=f){recall_glint->set_tiles(bn::sprite_items::recall_glint.tiles_item(),f);glint_tile=f;}
     recall_glint->set_position(bn::fixed(round.ball_x_q8())/256-120,bn::fixed(round.ball_y_q8())/256-80);
    }else recall_glint.reset();
   }
   int live=0;for(auto&p:particles)if(p.sprite){int age=core.tick-p.born;if(age>=effect_lifetime_ticks){p.sprite.reset();continue;}++live;int phase=age/effect_frame_ticks;if(p.frame!=phase){p.sprite->set_tiles(effect_item(p.kind,p.character).tiles_item(),phase);p.frame=phase;}p.sprite->set_position(p.x-120+camera_x,p.y-80+camera_y-(p.kind==DUST?age/4:0));}pkmn_debug[29]=live;
  }
  if(group==3){focus_at(120,52+choice*18,224,14);marker->set_position(-104,-28+choice*18);}
  if(group==12){focus_at(120,mode_row_y[mode_row],224,mode_row<3?13:11);marker->set_position(-104,mode_row_y[mode_row]-80);}
  if(group==8){focus_at(120,66+audio_row*24,224,18);marker->set_position(-96,-14+audio_row*24);}
  if(group==4){int age=frame-result_started;if(core.outcome==2){recall_ball->set_scale(bn::fixed(1.5));recall_ball->set_position(0,24);}else{auto&sp=*fighters[0];Pose current=loop_pose(selected,core.outcome==1?CLIP_VICTORY:core.outcome==2?(age<24?CLIP_FAINT:CLIP_DOWN):CLIP_IDLE,age,core.outcome!=2);pose(0,selected,current);bn::fixed scale=core.outcome==2?bn::fixed(1):bn::fixed(1.25);sp.set_scale(scale);sp.set_rotation_angle(0);sp.set_position(0,120-26*scale-80);}focus_at(choice?180:60,145,104,18);marker->set_position((choice?140:20)-120,145-80);}

  for(int i=0;i<2;++i)pkmn_presentation_debug[26+i]=fighters[i]&&fighters[i]->visible();
  pkmn_presentation_debug[28]=recall_ball&&recall_ball->visible();pkmn_presentation_debug[29]=recall_beam&&recall_beam->visible();pkmn_presentation_debug[30]=recall_glint&&recall_glint->visible();
  ++scene_frames;
  int seconds=(core.remaining+59)/60,charge=0;
  for(int i=0;i<2;i++)if((core.f[i].action==SUPER&&core.tick-core.f[i].began<=super_startup_ticks)||core.p[i+2].active)charge=1;
  int newkey=(group==2?feel.status(core,0)*10000000+feel.status(core,1)*100000000:0)+screen*100000+selected*10000+difficulty*1000+choice*100+(level?50:0)+(group==2?((screen==COUNTDOWN||screen==RESUME_COUNTDOWN)?(count+59)/60:seconds)+charge*200+(core.f[0].sp==100?400:0)+(core.f[1].sp==100?800:0):0)+(group==4?core.outcome*10000:0);
  if(group>=9&&group<=11)newkey=screen*100000+selected*10000+difficulty*1000+arcade.stage*10+arcade.continues;
  if(group==12)newkey=1200000+mode_row;
  if(group==1)newkey=screen*100000+selected*10000+difficulty*1000+selector_row*100+chosen_arena*10;
  if(group==13)newkey=13000000+selected*10000+int(progress.mastery[selected].followups)*100+int(progress.mastery[selected].evades)*10+int(progress.no_continue(selected));
  if(group==8)newkey=1100000+audio_row*100+int(sound_settings.music)*10+int(sound_settings.sound);
  if(group==6)newkey=screen*100000+selected*10000+practice_row*1000+int(trainer.mode)*100+trainer.opponent*10+chosen_arena;
  if(newkey!=key&&((group!=1&&group!=6&&group!=7)||scene_frames>=2)&&(group<9||scene_frames>=2)){key=newkey;labels.clear();
   if(group==1||group==6){
    int row=group==6?practice_row:selector_row;
    bool info=row==(group==6?4:3);
    line(120,151,!row?"A PLAY  SEL OPTIONS  B BACK":info?"A INFO  START PLAY  B GRID":"<> CHANGE  A PLAY  B GRID",true);
   }
   if(group==1){line(180,92,difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD",true);line(180,105,arcade_mode?"ROTATES":arena_names[chosen_arena],true);}
   if(group==6){
    line(180,92,practice_mode_names[trainer.mode],true);line(180,105,characters[trainer.opponent].name,true);line(180,118,arena_names[chosen_arena],true);
   }
   if(group==12){static const char* hints[]={"1 FIGHT","5 RIVALS","TRAIN","SCORES","ON/OFF","MOVES","MEDALS"};line(190,mode_row_y[mode_row],hints[mode_row],true);}
   if(group==3&&arcade.active)line(120,139,arcade.continues?"REMATCH COSTS 1 CONTINUE":"NO CONTINUES LEFT",true);
   if(group==8){line(182,66,sound_settings.music?"ON":"OFF",true);line(182,90,sound_settings.sound?"ON":"OFF",true);}
   if(group==7){const auto&d=characters[selected];line(120,36,d.name,true,2);line(120,54,d.role,true);line(120,72,d.trait,true);line(120,86,d.tip1,true);line(120,98,d.tip2,true);if(d.melee_followup)line(120,136,"EEVEE CAN ALSO FOLLOW WITH A",true);}
   if(group==2){
    if(practice)line(120,14,"TRN",true);else line(120,14,bn::to_string<8>(seconds),true,2);
    if(screen==COUNTDOWN)line(120,71,bn::to_string<8>((count+59)/60),true,3);
    if(screen==RESUME_COUNTDOWN)line(120,71,"READY",true,2);
    if(screen==FIGHTING||screen==ROUND_END){
     static const char* statuses[]={"","LOW STA","BLOCK","NO STA","GUARD BREAK","R READY","PUNISH","FOLLOW B","FOLLOW A/B"};
     for(int i=0;i<2;i++){int status=feel.status(core,i);if(status)line(i?192:48,47,statuses[status],true);}
     if(charge)line(120,57,"SUPER! JUMP OR SQUEEZE",true);
    }

   }
   if(group>=9&&group<=11){
    const char* diff=difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD";
    if(group==11){
     line(120,22,"ARCADE RECORDS",true,2);line(120,45,characters[selected].name,true);line(120,59,diff,true);
     const auto&r=progress.record(selected,difficulty);
     bn::string<40> v="CLEARS ";v+=bn::to_string<12>(r.clears);line(120,80,v,true);
     v="NO CONTINUE ";v+=bn::to_string<12>(r.perfect);line(120,94,v,true);
     v="BEST ";if(r.best_ticks){v+=bn::to_string<12>(r.best_ticks/60);v+=" SEC";}else v+="--";line(120,108,v,true);
     line(120,133,"< > FIGHTER  UP/DOWN LEVEL",true);line(120,145,"A MASTERY  B/START BACK",true);
    }else{
     line(120,22,group==10?"CHAMPION!":core.outcome==1?"RIVAL DEFEATED":core.outcome==3?"DRAW - TRY AGAIN":"DEFEATED",true,2);
     line(120,46,characters[selected].name,true);line(120,60,diff,true);
     bn::string<40> v="WINS ";v+=bn::to_string<4>(arcade.stage);v+=" / 5   CONTINUES ";v+=bn::to_string<4>(arcade.continues);line(120,79,v,true);
     v="BATTLE TIME ";v+=bn::to_string<12>(arcade.ticks/60);v+=" SEC";line(120,94,v,true);
     if(group==10){line(120,110,arcade.used?"ARCADE CLEAR!":"NO-CONTINUE CLEAR!",true);line(120,132,"A RECORDS  B MENU",true);}
     else {if(core.outcome==1){v="NEXT: ";v+=characters[arcade.opponent()].name;line(120,110,v,true);}line(120,132,core.outcome==1?"A NEXT RIVAL":core.outcome==3?"A RETRY":arcade.continues?"A USE CONTINUE":"A FINISH RUN",true);line(120,145,"B END RUN",true);}
    }
    if(storage.failed)line(120,120,"SAVE FAILED: MEMORY ONLY",true);else if(medal_earned&&group!=11)line(120,120,"MASTERY MEDAL EARNED!",true);
   }
   if(group==13){
    line(120,41,characters[selected].name,true);
    bn::string<16> v=bn::to_string<4>(progress.medals(selected));v+=" / 3";line(143,55,v,true);
    const auto&m=progress.mastery[selected];v=bn::to_string<4>(m.followups);v+=" / 10";line(201,75,v,true);
    v=bn::to_string<4>(m.evades);v+=" / 5";line(201,90,v,true);
    line(201,105,progress.no_continue(selected)?"1 / 1":"0 / 1",true);
    line(120,119,storage.failed?"SAVE FAILED: MEMORY ONLY":"QUICK BATTLE / ARCADE ONLY",true);
   }
   if(group==4){bn::string<40> result=characters[selected].name;result+=core.outcome==1?" WINS!":core.outcome==2?" WILL RETURN!":" - DRAW";line(120,54,result,true);if(storage.failed)line(120,124,"SAVE FAILED: MEMORY ONLY",true);else if(medal_earned)line(120,124,"MASTERY MEDAL EARNED!",true);}

  }
  if(arcade.active&&group==2&&hbe_setup_frame>=6&&practice_labels.empty()){bn::string<48> stage;if(arcade.active){stage=bn::to_string<4>(arcade.stage+1);stage+="/5 ";}stage+=arena_names[battle_arena];if(arcade.active){stage+="  CONT ";stage+=bn::to_string<4>(arcade.continues);}line(120,137,stage,true,1,&practice_labels);}
  if(practice&&group==2&&active_draw_frames>=3&&(practice_labels.empty()||last_practice_damage!=trainer.stats.last_damage||last_practice_chain!=trainer.stats.chain||last_practice_best!=trainer.stats.best_chain||last_practice_feedback!=trainer.feedback)){
   practice_labels.clear();
   if(trainer.drill())line(120,129,trainer.message(),true,1,&practice_labels);
   else{bn::string<48> score="LAST ";score+=bn::to_string<8>(trainer.stats.last_damage);score+=" CHAIN ";score+=bn::to_string<8>(trainer.stats.chain);score+=" BEST ";score+=bn::to_string<8>(trainer.stats.best_chain);line(120,129,score,true,1,&practice_labels);}
   line(120,138,trainer.drill()?"SELECT RETRY":"SELECT: RESET PRACTICE",true,1,&practice_labels);
   last_practice_damage=trainer.stats.last_damage;last_practice_chain=trainer.stats.chain;last_practice_best=trainer.stats.best_chain;
   last_practice_feedback=trainer.feedback;
  }
 }
};
[[noreturn]] void run_app(){
 static Renderer renderer;static PracticeSession trainer;
 progress=storage.load();sound_settings.music=progress.music;sound_settings.sound=progress.sound;
 Screen screen=TITLE,help_return=TITLE,guide_return=SELECTOR,audio_return=TITLE;
 int selected=0,cpu=1,difficulty=1,choice=0,count=0,frame=0,practice_row=0,practice_rounds=0,audio_row=0,selector_row=0;
 bool practice=false;uint32_t seed=0x504b4d4e;int peak=0,missed=0;
 auto save_progress=[&](){progress.music=sound_settings.music;progress.sound=sound_settings.sound;if(storage.save(progress))mastery_dirty=false;};
 auto avoid_mirror=[&](){if(trainer.opponent==selected)trainer.opponent=(selected+1)%character_count;};
 auto start_match=[&](bool keep_stats=false,bool quick_practice=false){
  feel.reset();audio.reset();round.reset();renderer.reset_scene();seed=seed*1664525u+1013904223u;
  if(arcade.active){selected=arcade.fighter;difficulty=arcade.difficulty;cpu=arcade.opponent();}
  if(practice){avoid_mirror();cpu=trainer.opponent;trainer.begin_round(keep_stats);practice_rounds=keep_stats?practice_rounds+1:1;}
  if(mastery_dirty)save_progress();mastery_tracker.reset();match_medals=progress.medals(selected);medal_earned=false;
  battle_arena=arcade.active?arcade.arena():chosen_arena;
  core.reset(selected,cpu,seed);core.training=practice;
  if(practice)trainer.prepare(core);
  ai.reset(difficulty,seed^0xa5a5a5a5u);count=quick_practice?practice_retry_frames:match_countdown_display_ticks;screen=quick_practice?RESUME_COUNTDOWN:COUNTDOWN;choice=0;
 };
 while(true){
  Input keys=input();++frame;Screen previous_screen=screen;int previous_options=selector_row*3+chosen_arena;int previous_menu=selected+choice*10+difficulty*100+practice_row*1000+int(trainer.mode)*10000+trainer.opponent*100000+audio_row*1000000+mode_row*10000000;AudioCue cue=AUDIO_NONE;bool confirm=keys.pressed&(A|START);
  if((keys.pressed&L)&&(screen==TITLE||screen==SELECTOR||screen==PRACTICE_SETUP||screen==PAUSED||screen==RESULT)){audio_return=screen;audio_row=0;screen=AUDIO_OPTIONS;}
  else if(screen==AUDIO_OPTIONS){
   if(keys.pressed&(B|START|L)){screen=audio_return;if(progress.music!=sound_settings.music||progress.sound!=sound_settings.sound)save_progress();}
   else if(keys.pressed&(UP|DOWN))audio_row=1-audio_row;
   else if(keys.pressed&(A|LEFT|RIGHT)){if(audio_row==0)sound_settings.music=!sound_settings.music;else sound_settings.sound=!sound_settings.sound;cue=UI_CONFIRM;}
  }
  else if(screen==TITLE){
   if(confirm){practice=false;arcade.active=false;mode_row=0;screen=MODE_MENU;}
   else if(keys.pressed&R){practice=false;arcade_mode=true;arcade.active=false;screen=SELECTOR;}
   else if(keys.pressed&DOWN){mode_row=3;screen=RECORDS;}
   else if(keys.pressed&B){arcade.active=false;arcade_mode=false;practice=true;practice_row=0;avoid_mirror();screen=PRACTICE_SETUP;}
   else if(keys.pressed&SELECT){help_return=TITLE;screen=HELP;}
  }
  else if(screen==MODE_MENU){
   if(keys.pressed&(UP|LEFT))mode_row=(mode_row+6)%7;
   else if(keys.pressed&(DOWN|RIGHT))mode_row=(mode_row+1)%7;
   if(keys.pressed&B)screen=TITLE;
   else if(confirm){
    arcade.active=false;practice=false;selector_row=0;
    if(mode_row<2){arcade_mode=mode_row==1;screen=SELECTOR;}
    else if(mode_row==2){arcade_mode=false;practice=true;practice_row=0;avoid_mirror();screen=PRACTICE_SETUP;}
    else if(mode_row==3)screen=RECORDS;
    else if(mode_row==4){audio_return=MODE_MENU;audio_row=0;screen=AUDIO_OPTIONS;}
    else if(mode_row==6)screen=MASTERY;
    else{help_return=MODE_MENU;screen=HELP;}
   }
  }
  else if(screen==HELP){if(keys.pressed&(A|B|START|SELECT))screen=help_return;}
  else if(screen==FIGHTER_GUIDE){if(keys.pressed&(A|B|START|R))screen=guide_return;}
  else if(screen==SELECTOR){
   if(keys.pressed&B){if(selector_row)selector_row=0;else{screen=MODE_MENU;mode_row=arcade_mode?1:0;}}
   else if(keys.pressed&SELECT)selector_row=selector_row?0:1;
   else if((keys.pressed&R)||((keys.pressed&A)&&!(keys.pressed&START)&&selector_row==3)){guide_return=SELECTOR;screen=FIGHTER_GUIDE;}
   else if(confirm){seed^=uint32_t(frame);if(arcade_mode)arcade.begin(selected,difficulty,seed);cpu=(selected+1+int(seed%(character_count-1)))%character_count;start_match();}
   else if(selector_row==1){if(keys.pressed&UP)selector_row=0;else if(keys.pressed&DOWN)selector_row=arcade_mode?3:2;else if(keys.pressed&LEFT)difficulty=(difficulty+2)%3;else if(keys.pressed&RIGHT)difficulty=(difficulty+1)%3;}
   else if(selector_row==2){if(keys.pressed&UP)selector_row=1;else if(keys.pressed&DOWN)selector_row=3;else if(keys.pressed&LEFT)chosen_arena=(chosen_arena+arena_count-1)%arena_count;else if(keys.pressed&RIGHT)chosen_arena=(chosen_arena+1)%arena_count;}
   else if(selector_row==3){if(keys.pressed&UP)selector_row=arcade_mode?1:2;}
   else{if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;if(keys.pressed&RIGHT)selected=(selected+1)%character_count;if(keys.pressed&UP)selected=(selected+character_count-3)%character_count;if(keys.pressed&DOWN){if(selected>=character_count-3)selector_row=1;else selected+=3;}}
  }
  else if(screen==PRACTICE_SETUP){
   if(keys.pressed&B){if(practice_row)practice_row=0;else{screen=MODE_MENU;mode_row=2;practice=false;}}
   else if(keys.pressed&SELECT)practice_row=practice_row?0:1;
   else if((keys.pressed&R)||((keys.pressed&A)&&!(keys.pressed&START)&&practice_row==4)){guide_return=PRACTICE_SETUP;screen=FIGHTER_GUIDE;}
   else if(confirm){start_match();}
   else if(practice_row==0){
    if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;if(keys.pressed&RIGHT)selected=(selected+1)%character_count;
    if(keys.pressed&UP)selected=(selected+character_count-3)%character_count;
    if(keys.pressed&DOWN){if(selected>=character_count-3)practice_row=1;else selected+=3;}
    avoid_mirror();
   }
   else{
    if(keys.pressed&UP)--practice_row;
    else if((keys.pressed&DOWN)&&practice_row<4)++practice_row;
    else if(practice_row<4&&(keys.pressed&(LEFT|RIGHT))){
     int direction=keys.pressed&RIGHT?1:-1;
     if(practice_row==1)trainer.mode=DummyMode((int(trainer.mode)+DUMMY_MODE_COUNT+direction)%DUMMY_MODE_COUNT);
     else if(practice_row==3)chosen_arena=(chosen_arena+arena_count+direction)%arena_count;
     else{trainer.opponent=(trainer.opponent+character_count+direction)%character_count;if(trainer.opponent==selected)trainer.opponent=(trainer.opponent+character_count+direction)%character_count;}
    }
   }
  }
  else if(screen==COUNTDOWN||screen==RESUME_COUNTDOWN){core.f[0].pending=core.f[1].pending=0;if(--count<=0){screen=FIGHTING;}}
  else if(screen==FIGHTING){
   if(keys.pressed&START){screen=PAUSED;choice=0;core.f[0].pending=core.f[1].pending=0;}
   else if(practice&&(keys.pressed&SELECT))start_match(false,true);
   else{if(arcade.active)++arcade.ticks;core.frame(keys,practice?trainer.input(core):ai.input(core));if(practice)trainer.observe(core);else{mastery_dirty=mastery_tracker.observe(core,progress)||mastery_dirty;medal_earned=progress.medals(selected)>match_medals;}if(core.outcome){round.begin(core,practice);screen=ROUND_END;count=round.duration();}}
  }
  else if(screen==PAUSED){
   if(keys.pressed&(UP|LEFT)){choice=(choice+4)%5;if(arcade.active&&!arcade.continues&&choice==1)choice=0;}
   else if(keys.pressed&(DOWN|RIGHT)){choice=(choice+1)%5;if(arcade.active&&!arcade.continues&&choice==1)choice=2;}
   if((keys.pressed&B)||(keys.pressed&START)){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}
   else if(confirm){
    if(choice==0){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}
    else if(choice==1){if(!arcade.active||arcade.retry(2))start_match(false,practice);}
    else if(choice==2){if(mastery_dirty)save_progress();arcade.active=false;screen=practice?PRACTICE_SETUP:SELECTOR;selector_row=0;practice_row=0;}
    else if(choice==3){help_return=PAUSED;screen=HELP;}else{audio_return=PAUSED;audio_row=0;screen=AUDIO_OPTIONS;}
   }
  }
  else if(screen==ROUND_END){round.advance();if(round.age==8&&mastery_dirty)save_progress();if(keys.pressed&(A|START))round.skip();count=round.duration()-round.age;if(round.done()){if(practice)start_match(true,true);else{screen=RESULT;choice=0;if(arcade.active){if(core.outcome==1)arcade.advance();if(arcade.finished){progress.complete(arcade);medal_earned=progress.medals(selected)>match_medals;save_progress();screen=CHAMPION;}else screen=ARCADE_RESULT;}}}}
  else if(screen==RECORDS){
   if(keys.pressed&(B|START)){arcade.active=false;screen=MODE_MENU;mode_row=3;}
   else if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;
   else if(keys.pressed&RIGHT)selected=(selected+1)%character_count;
   else if(keys.pressed&UP)difficulty=(difficulty+2)%3;
   else if(keys.pressed&DOWN)difficulty=(difficulty+1)%3;
   else if(keys.pressed&A)screen=MASTERY;
  }
  else if(screen==MASTERY){
   if(keys.pressed&(B|START)){screen=MODE_MENU;mode_row=6;}
   else if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;
   else if(keys.pressed&RIGHT)selected=(selected+1)%character_count;
   else if(keys.pressed&A)screen=RECORDS;
  }
  else if(screen==CHAMPION){if(keys.pressed&B){arcade.active=false;screen=MODE_MENU;mode_row=1;}else if(confirm){arcade.active=false;screen=RECORDS;}}
  else if(screen==ARCADE_RESULT){
   if(keys.pressed&B){arcade.active=false;screen=MODE_MENU;mode_row=1;}
   else if(confirm){if(core.outcome==1||arcade.retry(core.outcome))start_match();else{arcade.active=false;screen=MODE_MENU;mode_row=1;}}
  }
  else if(screen==RESULT){if(keys.pressed&(LEFT|RIGHT|UP|DOWN))choice=1-choice;if(keys.pressed&B){screen=SELECTOR;selector_row=0;}else if(confirm){if(choice==0)start_match();else{screen=SELECTOR;selector_row=0;}}}
  int current_menu=selected+choice*10+difficulty*100+practice_row*1000+int(trainer.mode)*10000+trainer.opponent*100000+audio_row*1000000+mode_row*10000000;
  if(previous_screen!=screen){
   if(previous_screen==AUDIO_OPTIONS)cue=UI_BACK;else if(screen==PAUSED)cue=UI_PAUSE;else if(screen==RESUME_COUNTDOWN)cue=UI_RESUME;
   else if(screen==COUNTDOWN)cue=ROUND_COUNT;else if(screen==FIGHTING)cue=ROUND_FIGHT;
   else if(screen==CHAMPION)cue=RESULT_WIN;else if(screen==ARCADE_RESULT)cue=core.outcome==1?RESULT_WIN:core.outcome==2?RESULT_LOSE:RESULT_DRAW;
   else if(screen==RESULT)cue=core.outcome==1?RESULT_WIN:core.outcome==2?RESULT_LOSE:RESULT_DRAW;
   else if(screen!=ROUND_END)cue=(previous_screen==AUDIO_OPTIONS||previous_screen==HELP||previous_screen==FIGHTER_GUIDE||screen==TITLE)?UI_BACK:UI_CONFIRM;
  }else if(cue==AUDIO_NONE&&(previous_menu!=current_menu||previous_options!=selector_row*3+chosen_arena)&&screen!=FIGHTING&&screen!=ROUND_END)cue=UI_MOVE;
  if(screen==COUNTDOWN&&(count==120||count==60))cue=ROUND_COUNT;
  if(screen==ROUND_END){if(round.age==18)cue=ROUND_KO;if(round.recall){if(round.age==44)cue=BALL_OPEN;else if(round.age==56)cue=BALL_ABSORB;else if(round.age==140)cue=BALL_SEAL;}}
  bool active=screen==FIGHTING||screen==ROUND_END;feel.update(core,active);
  audio.update(core,sound_settings.sound&&(screen==FIGHTING||(screen==ROUND_END&&round.age<18)));
  bool menu_music=screen==MASTERY||screen==MODE_MENU||screen==ARCADE_RESULT||screen==CHAMPION||screen==RECORDS||screen==TITLE||screen==SELECTOR||screen==PRACTICE_SETUP||screen==FIGHTER_GUIDE||screen==AUDIO_OPTIONS||(screen==HELP&&help_return!=PAUSED);
  ambience.update(menu_music,sound_settings);if(cue!=AUDIO_NONE)ambience.cue(cue,sound_settings);
  pkmn_arcade_debug[0]=0x41524338;pkmn_arcade_debug[1]=arcade_mode;pkmn_arcade_debug[2]=arcade.active;pkmn_arcade_debug[3]=arcade.stage;pkmn_arcade_debug[4]=arcade.continues;pkmn_arcade_debug[5]=arcade.used;pkmn_arcade_debug[6]=arcade.ticks;pkmn_arcade_debug[7]=arcade.finished;pkmn_arcade_debug[8]=progress.record(selected,difficulty).clears;pkmn_arcade_debug[9]=progress.record(selected,difficulty).perfect;pkmn_arcade_debug[10]=progress.record(selected,difficulty).best_ticks;pkmn_arcade_debug[11]=storage.failed;
  for(int i=0;i<arcade_rivals;++i)pkmn_arcade_debug[12+i]=arcade.roster[i];pkmn_arcade_debug[17]=mode_row;pkmn_arcade_debug[18]=character_count;pkmn_arcade_debug[19]=save_words;pkmn_arcade_debug[20]=choice;
  pkmn_mastery_debug[0]=0x4d415339;pkmn_mastery_debug[1]=chosen_arena;pkmn_mastery_debug[2]=battle_arena;pkmn_mastery_debug[3]=selector_row;pkmn_mastery_debug[4]=progress.mastery[selected].followups;pkmn_mastery_debug[5]=progress.mastery[selected].evades;pkmn_mastery_debug[6]=progress.no_continue(selected);pkmn_mastery_debug[7]=progress.medals(selected);pkmn_mastery_debug[8]=mastery_dirty;pkmn_mastery_debug[9]=medal_earned;pkmn_mastery_debug[10]=3;pkmn_mastery_debug[11]=sizeof(SaveSlot);pkmn_mastery_debug[12]=frame/24;pkmn_mastery_debug[13]=arena_sway(battle_arena,80,screen==FIGHTING?frame/24:-1);
  renderer.draw(screen,selected,cpu,difficulty,choice,selector_row,count,frame,practice,practice_row,trainer,audio_row);
  pkmn_presentation_debug[0]=0x50524537;pkmn_presentation_debug[1]=sound_settings.music;pkmn_presentation_debug[2]=sound_settings.sound;pkmn_presentation_debug[3]=ambience.music_playing();pkmn_presentation_debug[4]=ambience.music_gain_q8();pkmn_presentation_debug[5]=ambience.active_voices();pkmn_presentation_debug[6]=ambience.played_mask;pkmn_presentation_debug[7]=ambience.played;
  pkmn_presentation_debug[8]=audio_row;pkmn_presentation_debug[9]=audio_return;pkmn_presentation_debug[10]=round.age;pkmn_presentation_debug[11]=round.phase();pkmn_presentation_debug[12]=round.loser;pkmn_presentation_debug[13]=round.recall;pkmn_presentation_debug[14]=round.ball_x_q8();pkmn_presentation_debug[15]=round.ball_y_q8();pkmn_presentation_debug[16]=round.body_scale_q8();pkmn_presentation_debug[17]=round.ball_frame();pkmn_presentation_debug[18]=round.ball_visible();pkmn_presentation_debug[19]=round.beam_visible();pkmn_presentation_debug[20]=round.loser<0||round.body_visible(round.loser);pkmn_presentation_debug[21]=bn::core::last_cpu_usage().data();pkmn_presentation_debug[22]=ambience.music_position();pkmn_presentation_debug[23]=audio.active_voices()+ambience.active_voices();pkmn_presentation_debug[24]=round.duration();
  const auto& tactics=ai.diagnostics;
  pkmn_cpu_debug[0]=0x43505536;pkmn_cpu_debug[1]=tactics.intent;pkmn_cpu_debug[2]=tactics.target;pkmn_cpu_debug[3]=tactics.observed_tick;
  pkmn_cpu_debug[4]=tactics.melee_memory;pkmn_cpu_debug[5]=tactics.fire_memory;pkmn_cpu_debug[6]=tactics.super_memory;
  pkmn_cpu_debug[7]=tactics.decisions;pkmn_cpu_debug[8]=tactics.defenses;pkmn_cpu_debug[9]=tactics.punishes;pkmn_cpu_debug[10]=tactics.followups;
  pkmn_cpu_debug[11]=tactics.hesitations;pkmn_cpu_debug[12]=tactics.plan;pkmn_cpu_debug[13]=tactics.resting;
  pkmn_cpu_debug[14]=cpu_difficulties[difficulty].reaction_ticks;pkmn_cpu_debug[15]=cpu_difficulties[difficulty].think_ticks;
  pkmn_cpu_debug[16]=tactics.closing;
  pkmn_feel_debug[0]=0x4645454c;pkmn_feel_debug[1]=feel.seen_events;pkmn_feel_debug[2]=feel.dx;pkmn_feel_debug[3]=feel.dy;pkmn_feel_debug[4]=audio.active_voices();pkmn_feel_debug[5]=audio.character_mask;pkmn_feel_debug[6]=audio.charge_mask;pkmn_feel_debug[7]=audio.release_mask;pkmn_feel_debug[8]=feel.guard_breaks;pkmn_feel_debug[9]=feel.rejected_actions;pkmn_feel_debug[10]=feel.landing_jumps;pkmn_feel_debug[11]=feel.shake_frames;pkmn_feel_debug[12]=audio.active_charges();pkmn_feel_debug[13]=bn::core::last_cpu_usage().data();pkmn_feel_debug[14]=feel.status(core,0);pkmn_feel_debug[15]=feel.status(core,1);
  pkmn_practice_debug[0]=0x50524143;pkmn_practice_debug[1]=trainer.feedback;pkmn_practice_debug[2]=trainer.completed();pkmn_practice_debug[3]=trainer.super_started;pkmn_practice_debug[4]=trainer.super_released;pkmn_practice_debug[5]=practice&&screen==RESUME_COUNTDOWN?count:0;pkmn_practice_debug[6]=core.p[3].active;pkmn_practice_debug[7]=core.p[3].x;pkmn_practice_debug[8]=core.p[3].id;pkmn_practice_debug[9]=practice&&trainer.drill();
  int training_projectiles=0;if(practice)for(int i=0;i<4;++i)if(core.p[i].active||core.p[i].reserved)training_projectiles|=1<<i;
  pkmn_practice_debug[10]=training_projectiles;pkmn_practice_debug[11]=core.f[0].pending;pkmn_practice_debug[12]=core.f[1].pending;
  pkmn_depth_debug[0]=0x44455054;pkmn_depth_debug[1]=practice;pkmn_depth_debug[2]=practice_row;pkmn_depth_debug[3]=trainer.mode;pkmn_depth_debug[4]=trainer.opponent;pkmn_depth_debug[5]=trainer.stats.last_damage;pkmn_depth_debug[6]=trainer.stats.chain;pkmn_depth_debug[7]=trainer.stats.best_chain;pkmn_depth_debug[8]=core.f[0].chain_depth;pkmn_depth_debug[9]=core.f[1].chain_depth;pkmn_depth_debug[10]=core.f[0].confirmed_at;pkmn_depth_debug[11]=core.f[1].confirmed_at;pkmn_depth_debug[12]=characters[selected].melee_reach;pkmn_depth_debug[13]=characters[cpu].melee_reach;pkmn_depth_debug[14]=trainer.stats.followups;pkmn_depth_debug[15]=trainer.stats.punishes;pkmn_depth_debug[16]=feel.followups;pkmn_depth_debug[17]=feel.punishes;pkmn_depth_debug[18]=trainer.stats.hits;pkmn_depth_debug[19]=core.training;pkmn_depth_debug[20]=bn::core::last_cpu_usage().data();pkmn_depth_debug[21]=core.f[0].vx;pkmn_depth_debug[22]=core.f[1].vx;pkmn_depth_debug[23]=core.f[0].kb;pkmn_depth_debug[24]=core.f[1].kb;pkmn_depth_debug[25]=core.f[0].ends;pkmn_depth_debug[26]=core.f[1].ends;pkmn_depth_debug[27]=core.f[0].began;pkmn_depth_debug[28]=core.f[1].began;pkmn_depth_debug[29]=core.event_count;pkmn_depth_debug[30]=feel.status(core,0);pkmn_depth_debug[31]=practice_rounds;
  int usage=bn::core::last_cpu_usage().data();if(usage>peak)peak=usage;missed+=bn::core::last_missed_frames();
  pkmn_debug[0]=0x504b4d4e;pkmn_debug[1]=screen;pkmn_debug[2]=selected;pkmn_debug[3]=cpu;pkmn_debug[4]=difficulty;pkmn_debug[5]=core.tick;pkmn_debug[6]=core.f[0].hp;pkmn_debug[7]=core.f[1].hp;pkmn_debug[8]=core.f[0].sta;pkmn_debug[9]=core.f[1].sta;pkmn_debug[10]=core.f[0].sp;pkmn_debug[11]=core.f[1].sp;pkmn_debug[12]=core.f[0].x;pkmn_debug[13]=core.f[1].x;pkmn_debug[14]=core.f[0].y;pkmn_debug[15]=core.f[1].y;pkmn_debug[16]=core.f[0].action;pkmn_debug[17]=core.f[1].action;pkmn_debug[18]=core.remaining;pkmn_debug[19]=core.hitstop;pkmn_debug[20]=core.outcome;pkmn_debug[21]=peak;pkmn_debug[22]=missed;pkmn_debug[23]=frame;pkmn_debug[24]=seed;
  bn::core::update();
 }
}
}
