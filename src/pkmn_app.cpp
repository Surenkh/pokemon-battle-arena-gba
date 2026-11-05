#include "pkmn_app.h"
#include "pkmn_cup_store.h"
#include "bn_sprite_items_cup_cue_panel.h"
#include "bn_regular_bg_items_cup_selector.h"
#include "bn_regular_bg_items_cup_board_0.h"
#include "bn_regular_bg_items_cup_board_1.h"
#include "bn_regular_bg_items_cup_board_2.h"
#include "bn_regular_bg_items_cup_result.h"
#include "bn_regular_bg_items_cup_picker.h"
#include "bn_sprite_items_cup_crest.h"
extern "C" { volatile int pkmn_cup_debug[32] = {}; }
#include "bn_sprite_items_shadow_burst.h"
extern "C" { volatile int pkmn_technique_debug[32] = {}; volatile int pkmn_starter_debug[32] = {}; }
#include "pkmn_local_versus.h"
#include "bn_regular_bg_items_versus_selector.h"
#include "bn_regular_bg_items_versus_pause.h"
#include "bn_regular_bg_items_versus_result.h"
#include "bn_sprite_items_versus_cursor.h"
#include "bn_sprite_items_versus_marker.h"
#include "bn_sprite_items_selected_card.h"
#include "bn_sprite_items_arena_previews.h"
#include "bn_sprite_items_aura_0.h"
#include "bn_sprite_items_aura_1.h"
#include "bn_sprite_items_aura_2.h"
#include "bn_sprite_items_aura_3.h"
#include "bn_sprite_items_aura_4.h"
#include "bn_sprite_items_aura_5.h"
#include "bn_sprite_items_aura_6.h"
#include "bn_sprite_items_aura_7.h"
#include "bn_sprite_items_aura_8.h"
#include "bn_sprite_items_aura_9.h"
#include "bn_sprite_items_aura_10.h"
#include "bn_sprite_items_aura_11.h"
#include "bn_sprite_items_aura_12.h"
#include "bn_sprite_items_punch_0.h"
#include "bn_sprite_items_punch_1.h"
#include "bn_sprite_items_punch_2.h"
#include "bn_sprite_items_punch_3.h"
#include "bn_sprite_items_punch_4.h"
#include "bn_sprite_items_punch_5.h"
#include "bn_sprite_items_punch_6.h"
#include "bn_sprite_items_punch_7.h"
#include "bn_sprite_items_punch_8.h"
#include "bn_sprite_items_punch_9.h"
#include "bn_sprite_items_punch_10.h"
#include "bn_sprite_items_punch_11.h"
#include "bn_sprite_items_punch_12.h"
#include "bn_sprite_items_burst_0.h"
#include "bn_sprite_items_burst_1.h"
#include "bn_sprite_items_burst_2.h"
#include "bn_sprite_items_burst_3.h"
#include "bn_sprite_items_burst_4.h"
#include "bn_sprite_items_burst_5.h"
#include "bn_sprite_items_burst_6.h"
#include "bn_sprite_items_burst_7.h"
#include "bn_sprite_items_burst_8.h"
#include "bn_sprite_items_burst_9.h"
#include "bn_sprite_items_burst_10.h"
#include "bn_sprite_items_burst_11.h"
#include "bn_sprite_items_burst_12.h"
#include "pkmn_roster_navigation.h"
#include "bn_sprite_items_pk_vulpix.h"
#include "bn_sprite_items_pk_gastly.h"
#include "bn_sprite_items_pk_riolu.h"
#include "bn_sprite_items_pk_totodile.h"
#include "bn_sprite_items_shot_9.h"
#include "bn_sprite_items_wave_9.h"
#include "bn_sprite_items_hit_9.h"
#include "bn_sprite_items_cast_9.h"
#include "bn_sprite_items_guard_9.h"
#include "bn_sprite_items_shot_10.h"
#include "bn_sprite_items_wave_10.h"
#include "bn_sprite_items_hit_10.h"
#include "bn_sprite_items_cast_10.h"
#include "bn_sprite_items_guard_10.h"
#include "bn_sprite_items_shot_11.h"
#include "bn_sprite_items_wave_11.h"
#include "bn_sprite_items_hit_11.h"
#include "bn_sprite_items_cast_11.h"
#include "bn_sprite_items_guard_11.h"
#include "bn_sprite_items_shot_12.h"
#include "bn_sprite_items_wave_12.h"
#include "bn_sprite_items_hit_12.h"
#include "bn_sprite_items_cast_12.h"
#include "bn_sprite_items_guard_12.h"
#include "bn_regular_bg_items_arena_frostpeak.h"
#include "bn_regular_bg_items_arena_ruins.h"
#include "bn_regular_bg_items_practice_frostpeak.h"
#include "bn_regular_bg_items_practice_ruins.h"
#include "pkmn_progress_store.h"
#include "pkmn_mastery.h"
#include "pkmn_match_goals.h"
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
#include "bn_regular_bg_items_records_panel.h"
#include "bn_regular_bg_items_mastery_panel.h"
#include "bn_regular_bg_items_arcade_selector.h"
#include "bn_regular_bg_items_mode_menu.h"
#include "bn_sprite_items_recall_ball.h"
#include "bn_sprite_items_recall_beam.h"
#include "bn_sprite_items_recall_glint.h"
extern "C" { volatile int pkmn_power_debug[32] = {}; volatile int pkmn_expansion_debug[32] = {}; volatile int pkmn_goals_debug[32] = {}; volatile int pkmn_practice_debug[16] = {}; volatile int pkmn_mastery_debug[32] = {}; volatile int pkmn_arcade_debug[32] = {}; volatile int pkmn_debug[32] = {}; volatile int pkmn_feel_debug[16] = {}; volatile int pkmn_depth_debug[32] = {}; volatile int pkmn_cpu_debug[32] = {}; volatile int pkmn_presentation_debug[32] = {}; }
namespace pkmn {
enum Screen { TITLE, SELECTOR, COUNTDOWN, FIGHTING, PAUSED, ROUND_END, RESULT, HELP, RESUME_COUNTDOWN, PRACTICE_SETUP, FIGHTER_GUIDE, AUDIO_OPTIONS, ARCADE_RESULT, CHAMPION, RECORDS, MODE_MENU, MASTERY, VERSUS_SELECT, CUP_BOARD, CUP_RESULT, CUP_PICKER };
static constexpr int mode_row_y[]={34,46,70,104,116,128,140,82,58};
static LocalVersus versus;
static VersusInput versus_input;
static const bn::sprite_item* const bodies[]={&bn::sprite_items::pk_pikachu,&bn::sprite_items::pk_charmander,&bn::sprite_items::pk_squirtle,&bn::sprite_items::pk_bulbasaur,&bn::sprite_items::pk_eevee,&bn::sprite_items::pk_meowth,&bn::sprite_items::pk_psyduck,&bn::sprite_items::pk_jigglypuff,&bn::sprite_items::pk_machop,&bn::sprite_items::pk_vulpix,&bn::sprite_items::pk_gastly,&bn::sprite_items::pk_riolu,&bn::sprite_items::pk_totodile};
static_assert(sizeof(bodies)/sizeof(bodies[0])==character_count);
static const bn::sprite_item* const shots[]={&bn::sprite_items::shot_0,&bn::sprite_items::shot_1,&bn::sprite_items::shot_2,&bn::sprite_items::shot_3,&bn::sprite_items::shot_4,&bn::sprite_items::shot_5,&bn::sprite_items::shot_6,&bn::sprite_items::shot_7,&bn::sprite_items::shot_8,&bn::sprite_items::shot_9,&bn::sprite_items::shot_10,&bn::sprite_items::shot_11,&bn::sprite_items::shot_12};
static_assert(sizeof(shots)/sizeof(shots[0])==character_count);
static const bn::sprite_item* const waves[]={&bn::sprite_items::wave_0,&bn::sprite_items::wave_1,&bn::sprite_items::wave_2,&bn::sprite_items::wave_3,&bn::sprite_items::wave_4,&bn::sprite_items::wave_5,&bn::sprite_items::wave_6,&bn::sprite_items::wave_7,&bn::sprite_items::wave_8,&bn::sprite_items::wave_9,&bn::sprite_items::wave_10,&bn::sprite_items::wave_11,&bn::sprite_items::wave_12};
static_assert(sizeof(waves)/sizeof(waves[0])==character_count);
static const bn::sprite_item* const hits[]={&bn::sprite_items::hit_0,&bn::sprite_items::hit_1,&bn::sprite_items::hit_2,&bn::sprite_items::hit_3,&bn::sprite_items::hit_4,&bn::sprite_items::hit_5,&bn::sprite_items::hit_6,&bn::sprite_items::hit_7,&bn::sprite_items::hit_8,&bn::sprite_items::hit_9,&bn::sprite_items::hit_10,&bn::sprite_items::hit_11,&bn::sprite_items::hit_12};
static_assert(sizeof(hits)/sizeof(hits[0])==character_count);
static const bn::sprite_item* const casts[]={&bn::sprite_items::cast_0,&bn::sprite_items::cast_1,&bn::sprite_items::cast_2,&bn::sprite_items::cast_3,&bn::sprite_items::cast_4,&bn::sprite_items::cast_5,&bn::sprite_items::cast_6,&bn::sprite_items::cast_7,&bn::sprite_items::cast_8,&bn::sprite_items::cast_9,&bn::sprite_items::cast_10,&bn::sprite_items::cast_11,&bn::sprite_items::cast_12};
static_assert(sizeof(casts)/sizeof(casts[0])==character_count);
static const bn::sprite_item* const guards[]={&bn::sprite_items::guard_0,&bn::sprite_items::guard_1,&bn::sprite_items::guard_2,&bn::sprite_items::guard_3,&bn::sprite_items::guard_4,&bn::sprite_items::guard_5,&bn::sprite_items::guard_6,&bn::sprite_items::guard_7,&bn::sprite_items::guard_8,&bn::sprite_items::guard_9,&bn::sprite_items::guard_10,&bn::sprite_items::guard_11,&bn::sprite_items::guard_12};
static_assert(sizeof(guards)/sizeof(guards[0])==character_count);
static const bn::sprite_item* const auras[]={&bn::sprite_items::aura_0,&bn::sprite_items::aura_1,&bn::sprite_items::aura_2,&bn::sprite_items::aura_3,&bn::sprite_items::aura_4,&bn::sprite_items::aura_5,&bn::sprite_items::aura_6,&bn::sprite_items::aura_7,&bn::sprite_items::aura_8,&bn::sprite_items::aura_9,&bn::sprite_items::aura_10,&bn::sprite_items::aura_11,&bn::sprite_items::aura_12};
static const bn::sprite_item* const punch_items[]={&bn::sprite_items::punch_0,&bn::sprite_items::punch_1,&bn::sprite_items::punch_2,&bn::sprite_items::punch_3,&bn::sprite_items::punch_4,&bn::sprite_items::punch_5,&bn::sprite_items::punch_6,&bn::sprite_items::punch_7,&bn::sprite_items::punch_8,&bn::sprite_items::punch_9,&bn::sprite_items::punch_10,&bn::sprite_items::punch_11,&bn::sprite_items::punch_12};
static const bn::sprite_item* const bursts[]={&bn::sprite_items::burst_0,&bn::sprite_items::burst_1,&bn::sprite_items::burst_2,&bn::sprite_items::burst_3,&bn::sprite_items::burst_4,&bn::sprite_items::burst_5,&bn::sprite_items::burst_6,&bn::sprite_items::burst_7,&bn::sprite_items::burst_8,&bn::sprite_items::burst_9,&bn::sprite_items::burst_10,&bn::sprite_items::burst_11,&bn::sprite_items::burst_12};
static Input input(){Input i;const bool held[]={bn::keypad::a_held(),bn::keypad::b_held(),bn::keypad::select_held(),bn::keypad::start_held(),bn::keypad::right_held(),bn::keypad::left_held(),bn::keypad::up_held(),bn::keypad::down_held(),bn::keypad::r_held(),bn::keypad::l_held()};const bool press[]={bn::keypad::a_pressed(),bn::keypad::b_pressed(),bn::keypad::select_pressed(),bn::keypad::start_pressed(),bn::keypad::right_pressed(),bn::keypad::left_pressed(),bn::keypad::up_pressed(),bn::keypad::down_pressed(),bn::keypad::r_pressed(),bn::keypad::l_pressed()};for(int n=0;n<10;n++){if(held[n])i.held|=1<<n;if(press[n])i.pressed|=1<<n;}return i;}
static ArcadeRun arcade;static Progress progress;static ProgressStore storage;static bool arcade_mode=false;static int mode_row=0;
static MasterySession mastery_tracker;static bool mastery_dirty=false,medal_earned=false;static int chosen_arena=WOODLAND,battle_arena=WOODLAND,match_medals=0;
static bn::string<16> cup_time(uint32_t ticks){
 const uint32_t seconds=ticks/60+uint32_t(ticks%60!=0);
 bn::string<16> value=bn::to_string<10>(seconds>9999?seconds/3600:seconds);value+=seconds>9999?"H":"S";return value;
}
static MatchGoals match_goals;
static RivalCupRun cup_run;static CupProgress cup_progress;static CupStore cup_storage;
static bool cup_mode=false;static int selected_cup=0;
static RoundPresentation round;static AudioSettings sound_settings;static PresentationAudio ambience;
static CombatCore core;static CpuController ai;static CombatFeel feel;static CombatAudio audio;static constexpr bn::sprite_font font(bn::sprite_items::pkfont);
class Renderer {
 int group=-1,key=-1,roster_selected=-1,roster_frames=0;bn::optional<bn::regular_bg_ptr> bg,ensemble;
 bn::optional<bn::regular_bg_position_hbe_ptr> horizontal_shake,vertical_shake;
 bn::array<bn::fixed,160> shake_x{},shake_y{};int last_camera_x=99,last_camera_y=99,hbe_setup_frame=0,last_canopy_step=-1;
 bn::optional<bn::sprite_ptr> fighters[2],shield[2],bullet[4],bars[6],focus,focus_right,marker,sparks[5];
 bn::optional<bn::sprite_ptr> cup_panels[2],cup_crest;
 bn::optional<bn::sprite_ptr> selected_card,arena_preview,punches[2];int preview_tile=-1,punch_tile[2]={-1,-1},charge_asset[2]={-1,-1};
 bn::optional<bn::sprite_ptr> versus_cursors[2],versus_cards[2],player_marks[2];
 int versus_selected[2]={-1,-1};
 bn::optional<bn::sprite_ptr> shadows[2],portraits[character_count],badges[character_count],charging[2];
 bn::optional<bn::sprite_ptr> recall_ball,recall_beam,recall_glint;int recall_tile=-1,glint_tile=-1;
 int charge_tile[2]={-1,-1},flash_state[2]={-1,-1};uint32_t presented_events=0;
 int last_practice_damage=-1,last_practice_chain=-1,last_practice_best=-1,last_practice_feedback=-1,active_draw_frames=0,scene_frames=0;
 int result_detail_phase=0,result_detail_bits=0;
 enum Effect { DUST, IMPACT, CAST, GUARD, SUPER_IMPACT };
 int lifetime(int kind)const{return kind==SUPER_IMPACT?super_impact_ticks:effect_lifetime_ticks;}
 struct Particle{bn::optional<bn::sprite_ptr> sprite;int x=0,y=0,born=0,kind=0,character=0,frame=-1;};Particle particles[12];
 int bullet_tile[4]={-1,-1,-1,-1},bullet_key[4]={-1,-1,-1,-1};
 int tile[2]={-1,-1},portrait_tile[character_count]={},last_fx_tick=-1,result_started=0,ko_age=0;
 void pose(int i,int ch,Pose p){if(tile[i]!=p.frame){fighters[i]->set_tiles(bodies[ch]->tiles_item(),p.frame);tile[i]=p.frame;}pkmn_debug[25+i]=p.frame;pkmn_debug[27+i]=p.clip;}
 const bn::sprite_item& effect_item(int kind,int character)const{return kind==DUST?bn::sprite_items::dust:*(kind==SUPER_IMPACT?bursts[character]:kind==IMPACT?hits[character]:kind==CAST?casts[character]:guards[character]);}
 void particle(int x,int y,int kind,int character=0,bool flip=false){
  Particle* slot=nullptr;
  for(auto&p:particles)if(!p.sprite||core.tick-p.born>=lifetime(p.kind)){p.sprite.reset();slot=&p;break;}
  if(!slot&&kind!=DUST)for(auto&p:particles)if(p.kind==DUST){p.sprite.reset();slot=&p;break;}
  if(!slot)return;
  auto&p=*slot;p.x=x;p.y=y;p.kind=kind;p.character=character;p.born=core.tick;p.frame=0;
  p.sprite=effect_item(kind,character).create_sprite(0,0);p.sprite->set_z_order(-2);p.sprite->set_bg_priority(2);p.sprite->set_horizontal_flip(flip);
  if(kind==SUPER_IMPACT)pkmn_expansion_debug[10]|=1<<character;
  else if(kind!=DUST){pkmn_debug[31]|=1<<(character+(kind==CAST?8:kind==GUARD?16:0));pkmn_expansion_debug[9+kind]|=1<<character;}
 }
 bn::sprite_text_generator text{font};bn::vector<bn::sprite_ptr,64> labels;bn::vector<bn::sprite_ptr,8> hud_names;bn::vector<bn::sprite_ptr,16> practice_labels;
 bn::vector<bn::sprite_ptr,24> result_labels;
 bn::sprite_text_generator cup_card_text{bn::sprite_font(bn::sprite_items::pkfont,{},{},-2)};
 void line(int x,int y,bn::string_view value,bool center=false,int scale=1,bn::ivector<bn::sprite_ptr>* output=nullptr){auto& target=output?*output:static_cast<bn::ivector<bn::sprite_ptr>&>(labels);int first=target.size();if(center)text.set_center_alignment();else text.set_left_alignment();text.generate(x-120,y-80,value,target);for(int i=first;i<target.size();i++)target[i].set_bg_priority(0);if(scale>1){if(scale>2)scale=2;for(int i=first;i<target.size();i++){auto&sp=target[i];sp.set_position(x-120+(sp.x()-(x-120))*scale,y-80);sp.set_scale(scale);}}}
 void right_line(int x,int y,bn::string_view value){int first=labels.size();text.set_right_alignment();text.generate(x-120,y-80,value,labels);for(int i=first;i<labels.size();++i)labels[i].set_bg_priority(0);}
 void clear(){cup_crest.reset();for(auto& panel:cup_panels)panel.reset();for(int who=0;who<2;++who){versus_cursors[who].reset();versus_cards[who].reset();player_marks[who].reset();versus_selected[who]=-1;}for(auto&v:fighters)if(v){auto palette=v->palette();palette.set_fade(bn::color(31,31,31),0);}recall_ball.reset();recall_beam.reset();recall_glint.reset();recall_tile=glint_tile=-1;labels.clear();hud_names.clear();practice_labels.clear();result_labels.clear();result_detail_phase=result_detail_bits=0;active_draw_frames=scene_frames=0;roster_selected=-1;roster_frames=0;selected_card.reset();arena_preview.reset();preview_tile=-1;for(auto& v:punches)v.reset();punch_tile[0]=punch_tile[1]=-1;charge_asset[0]=charge_asset[1]=-1;last_practice_damage=last_practice_chain=last_practice_best=last_practice_feedback=-1;horizontal_shake.reset();vertical_shake.reset();bg.reset();ensemble.reset();last_camera_x=last_camera_y=99;hbe_setup_frame=0;last_canopy_step=-1;for(auto&v:charging)v.reset();charge_tile[0]=charge_tile[1]=-1;flash_state[0]=flash_state[1]=-1;presented_events=core.event_frame;focus.reset();focus_right.reset();marker.reset();for(auto&v:fighters)v.reset();for(auto&v:shield)v.reset();for(auto&v:bullet)v.reset();for(auto&v:bars)v.reset();for(auto&v:sparks)v.reset();for(auto&v:shadows)v.reset();for(auto&v:portraits)v.reset();for(auto&v:badges)v.reset();for(auto&v:particles)v.sprite.reset();for(auto&v:tile)v=-1;for(auto&v:portrait_tile)v=-1;for(auto&v:bullet_tile)v=-1;for(auto&v:bullet_key)v=-1;last_fx_tick=-1;ko_age=0;key=-1;pkmn_debug[29]=pkmn_debug[30]=pkmn_debug[31]=0;}
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
 void roster_backdrop(bool training){
  backdrop(training?bn::regular_bg_items::practice_setup:cup_mode?bn::regular_bg_items::cup_selector:arcade_mode?bn::regular_bg_items::arcade_selector:bn::regular_bg_items::selector);
 }
 void cup_portrait(int who,int x,int y,int frame) {
  if(!portraits[who]){
   portraits[who]=bodies[who]->create_sprite(x-120,y-80);
   portraits[who]->set_scale(bn::fixed(.5));
   portraits[who]->set_bg_priority(2);
  }
  const auto pose=loop_pose(who,CLIP_IDLE,frame);
  if(portrait_tile[who]!=pose.frame){
   portraits[who]->set_tiles(bodies[who]->tiles_item(),pose.frame);
   portrait_tile[who]=pose.frame;
  }
 }
 void cup_picker(int choice,int frame) {
  const auto& cup=cup_run.definition();
  focus_at(44+choice*76,70,72,62);
  if(scene_frames==1){
   bn::string<28> heading=cup.name;
   heading+="  FIGHT ";heading+=bn::to_string<4>(cup_run.stage+1);heading+="/3";
   line(120,30,heading,true);
  }
  for(int i=0;i<cup_challengers;++i){
   const auto& rival=cup.challengers[i];
   const bool available=cup_run.can_choose(i);
   const int x=44+i*76;
   if(scene_frames>=2+i&&available)cup_portrait(rival.fighter,x,66,frame+i*11);
   if(scene_frames==2+i){
    line(x,48,rival.name,true);
    cup_card_text.generate(x-120,91-80,characters[rival.fighter].name,labels);
    if(!available)line(x,67,"DEFEATED",true);
   }
  }
  if(scene_frames==5)line(120,148,cup_run.stage?"<> RIVAL  A FIGHT  B END RUN":"<> RIVAL  A FIGHT  B BACK",true);
  if(scene_frames>=6&&key!=choice){
   key=choice;practice_labels.clear();
   const auto& rival=cup.challengers[choice];
   line(120,115,arena_names[rival.arena],true,1,&practice_labels);
   line(120,129,rival.cue,true,1,&practice_labels);
  }
 }
 void local_selector(int frame) {
    if (scene_frames < 1) return;
    for (int character = 0; character < character_count; ++character) {
        if (character != versus.fighters[0] && character != versus.fighters[1]) {
            portraits[character].reset();
            portrait_tile[character] = -1;
        }
    }
    for (int who = 0; who < 2; ++who) {
        const int character = versus.fighters[who];
        const int x = roster_x(character) - 120;
        const int y = roster_y(character) - 80;
        if (!versus_cursors[who]) {
            versus_cursors[who] = bn::sprite_items::versus_cursor.create_sprite(0, 0, who);
            versus_cursors[who]->set_bg_priority(1);
            versus_cursors[who]->set_z_order(-3);
        }
        versus_cursors[who]->set_position(x, y + selection_card_height / 2);
        if (!versus_cards[who]) {
            versus_cards[who] = bn::sprite_items::selected_card.create_sprite(0, 0, character);
            versus_cards[who]->set_bg_priority(2);
            versus_cards[who]->set_z_order(3);
        }
        if (versus_selected[who] != character) {
            versus_cards[who]->set_tiles(bn::sprite_items::selected_card.tiles_item(), character);
            versus_selected[who] = character;
        }
        versus_cards[who]->set_position(x, y + selection_card_height / 2);
        if (!portraits[character]) {
            portraits[character] = bodies[character]->create_sprite(0, 0);
            portraits[character]->set_scale(bn::fixed(.25));
            portraits[character]->set_bg_priority(2);
            portraits[character]->set_z_order(1);
        }
        const auto current = loop_pose(character, CLIP_IDLE, frame + character * 11);
        if (portrait_tile[character] != current.frame) {
            portraits[character]->set_tiles(bodies[character]->tiles_item(), current.frame);
            portrait_tile[character] = current.frame;
        }
        portraits[character]->set_position(x, y + selection_portrait_y);
    }
    if (!arena_preview) {
        arena_preview = bn::sprite_items::arena_previews.create_sprite(selection_preview_x - 120, selection_preview_y - 80, versus.arena);
        arena_preview->set_vertical_scale(bn::fixed(.75));
        arena_preview->set_bg_priority(2);
    }
    if (preview_tile != versus.arena) {
        arena_preview->set_tiles(bn::sprite_items::arena_previews.tiles_item(), versus.arena);
        preview_tile = versus.arena;
    }
 }
public:
 void reset_scene(){group=-1;}
 void draw(Screen screen,int selected,int cpu,int difficulty,int choice,int selector_row,int count,int frame,bool practice,int practice_row,const PracticeSession& trainer,int audio_row){
  pkmn_technique_debug[30]=pkmn_technique_debug[31]=0;
  pkmn_goals_debug[10]=0;pkmn_expansion_debug[9]=0;pkmn_expansion_debug[15]=0;pkmn_power_debug[6]=0;pkmn_power_debug[7]=-1;pkmn_power_debug[8]=pkmn_power_debug[9]=0;for(int i=16;i<=22;++i)pkmn_power_debug[i]=0;
  bool level=selector_row==1;
  int next=screen==TITLE?0:screen==SELECTOR?1:(screen==COUNTDOWN||screen==FIGHTING||screen==ROUND_END||screen==RESUME_COUNTDOWN)?2:screen==PAUSED?(versus.active?15:3):screen==RESULT?(versus.active?16:4):screen==PRACTICE_SETUP?6:screen==FIGHTER_GUIDE?7:screen==AUDIO_OPTIONS?8:screen==ARCADE_RESULT?9:screen==CHAMPION?10:screen==RECORDS?11:screen==MODE_MENU?12:screen==MASTERY?13:screen==VERSUS_SELECT?14:screen==CUP_BOARD?17:screen==CUP_RESULT?18:screen==CUP_PICKER?19:5;
  if(next!=group){clear();group=next;
   if(group==0){backdrop(bn::regular_bg_items::title);ensemble=bn::regular_bg_items::ensemble.create_bg(8,48);ensemble->set_priority(2);for(int i=0;i<5;i++)sparks[i]=bn::sprite_items::spark.create_sprite(-104+i*48,-24+(i%2)*46);}
   if(group==1||group==6){roster_backdrop(group==6);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0);}
   if(group==2){static const bn::regular_bg_item* const stages[]={&bn::regular_bg_items::arena,&bn::regular_bg_items::arena_volcanic,&bn::regular_bg_items::arena_shoreline,&bn::regular_bg_items::arena_frostpeak,&bn::regular_bg_items::arena_ruins};static const bn::regular_bg_item* const training_stages[]={&bn::regular_bg_items::practice_arena,&bn::regular_bg_items::practice_volcanic,&bn::regular_bg_items::practice_shoreline,&bn::regular_bg_items::practice_frostpeak,&bn::regular_bg_items::practice_ruins};backdrop(*(practice?training_stages[battle_arena]:stages[battle_arena]));shake_x.fill(0);shake_y.fill(0);}
   if(group==3){backdrop(((arcade.active&&!arcade.continues)||(cup_run.active&&!cup_run.continues))?bn::regular_bg_items::pause_no_retry:bn::regular_bg_items::pause);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==4){result_started=frame;backdrop(core.outcome==1?bn::regular_bg_items::victory:core.outcome==2?bn::regular_bg_items::defeat:bn::regular_bg_items::draw);if(core.outcome==2)recall_ball=bn::sprite_items::recall_ball.create_sprite(0,30);else fighters[0]=bodies[selected]->create_sprite(0,0);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==9||group==10)backdrop(bn::regular_bg_items::arcade_panel);
   if(group==11)backdrop(bn::regular_bg_items::records_panel);
   if(group==13)backdrop(bn::regular_bg_items::mastery_panel);
   if(group==12){backdrop(bn::regular_bg_items::mode_menu);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==14)backdrop(bn::regular_bg_items::versus_selector);
   if(group==15){backdrop(bn::regular_bg_items::versus_pause);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
   if(group==16){
    backdrop(bn::regular_bg_items::versus_result);
    if(core.outcome!=3)fighters[0]=bodies[versus.fighters[core.outcome-1]]->create_sprite(0,0);
    focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);
   }
   if(group==17){static const bn::regular_bg_item* const boards[]={&bn::regular_bg_items::cup_board_0,&bn::regular_bg_items::cup_board_1,&bn::regular_bg_items::cup_board_2};backdrop(*boards[selected_cup]);}
   if(group==18)backdrop(bn::regular_bg_items::cup_result);
   if(group==19){
    backdrop(bn::regular_bg_items::cup_picker);
    focus=bn::sprite_items::focus.create_sprite(0,0);
    cup_card_text.set_center_alignment();
    cup_card_text.set_bg_priority(0);
   }
   if(group==5)backdrop(bn::regular_bg_items::help);
   if(group==7)backdrop(bn::regular_bg_items::fighter_guide);
   if(group==8){backdrop(bn::regular_bg_items::audio_options);focus=bn::sprite_items::focus.create_sprite(0,0);marker=bn::sprite_items::nav_markers.create_sprite(0,0,1);}
  }
  if(group==0){int b=(frame/18)%8;ensemble->set_y(47+(b<4?b:7-b));for(int i=0;i<5;i++){sparks[i]->set_visible((frame/12+i)%4<2);sparks[i]->set_rotation_angle((frame*2+i*30)%360);}}
  if(group==14)local_selector(frame);
  if(group==17&&scene_frames>=1)cup_portrait(cups[selected_cup].captain.fighter,42,62,frame);
  if(group==19){cup_picker(choice,frame);++scene_frames;return;}
  if(group==1||group==6){
   if(roster_selected!=selected){for(auto& portrait:portraits)portrait.reset();for(auto& value:portrait_tile)value=-1;roster_selected=selected;}
   if(scene_frames>=1){
    if(!selected_card){selected_card=bn::sprite_items::selected_card.create_sprite(0,0,selected);selected_card->set_bg_priority(2);selected_card->set_z_order(3);}
    selected_card->set_tiles(bn::sprite_items::selected_card.tiles_item(),selected);selected_card->set_position(roster_x(selected)-120,roster_y(selected)+selection_card_height/2-80);
    if(!portraits[selected]){portraits[selected]=bodies[selected]->create_sprite(0,0);portraits[selected]->set_scale(bn::fixed(.25));portraits[selected]->set_bg_priority(2);portraits[selected]->set_z_order(1);}
    auto pose=loop_pose(selected,CLIP_IDLE,frame+selected*11);if(portrait_tile[selected]!=pose.frame){portraits[selected]->set_tiles(bodies[selected]->tiles_item(),pose.frame);portrait_tile[selected]=pose.frame;}
    portraits[selected]->set_position(roster_x(selected)-120,roster_y(selected)+selection_portrait_y-80);
    pkmn_power_debug[9]=1<<selected;
    for(int i=0;i<character_count;++i)if(!badges[i]&&scene_frames>=1+i/4){badges[i]=bn::sprite_items::mastery_badge.create_sprite(roster_x(i)+16-120,roster_y(i)+5-80,progress.medals(i));badges[i]->set_bg_priority(2);badges[i]->set_z_order(-1);}
    if(group==6||(!arcade_mode&&!cup_mode)){
     if(!arena_preview){arena_preview=bn::sprite_items::arena_previews.create_sprite(selection_preview_x-120,selection_preview_y-80,chosen_arena);arena_preview->set_bg_priority(2);arena_preview->set_vertical_scale(bn::fixed(.75));}
     if(preview_tile!=chosen_arena){arena_preview->set_tiles(bn::sprite_items::arena_previews.tiles_item(),chosen_arena);preview_tile=chosen_arena;}
     pkmn_power_debug[7]=chosen_arena;
    }
   }
   ++roster_frames;pkmn_expansion_debug[15]=roster_frames;pkmn_expansion_debug[9]=(1<<character_count)-1;pkmn_power_debug[8]=(1<<character_count)-1;
   int row=group==6?practice_row:selector_row;
   if(!row)focus_at(roster_x(selected),roster_y(selected)+selection_card_height/2,selection_card_width,selection_card_height);
   else focus_at(120,selection_row_y(group==6,row),224,11);
   marker->set_position(roster_x(selected)-18-120,roster_y(selected)+5-80);
  }
  if(group==2){
   // Separate roster teardown/background upload from actor/tile allocation.
   // The first countdown display frame shows the arena; actors follow next.
   ++hbe_setup_frame;
   if(hbe_setup_frame==2){for(int i=0;i<2;i++){fighters[i]=bodies[i?cpu:selected]->create_sprite(0,0);fighters[i]->set_bg_priority(2);if(selected==cpu)fighters[i]->set_palette(bn::sprite_palette_ptr::create_new(bodies[selected]->palette_item()));shield[i]=bn::sprite_items::shield.create_sprite(0,0);shield[i]->set_visible(false);shield[i]->set_bg_priority(2);shadows[i]=bn::sprite_items::ground_shadow.create_sprite(0,0);shadows[i]->set_z_order(5);shadows[i]->set_bg_priority(2);for(int j=0;j<3;j++){bars[i*3+j]=bn::sprite_items::bars.create_sprite(0,0,j);bars[i*3+j]->set_bg_priority(0);}}}
   if(cup_run.active&&screen==COUNTDOWN){
    for(int i=0;i<2;++i)if(hbe_setup_frame>=6+i&&!cup_panels[i]){
     cup_panels[i]=bn::sprite_items::cup_cue_panel.create_sprite(64+i*112-120,112-80);cup_panels[i]->set_horizontal_scale(bn::fixed(1.75));cup_panels[i]->set_bg_priority(1);cup_panels[i]->set_z_order(3);
    }
   }else for(auto& panel:cup_panels)panel.reset();
   if(hbe_setup_frame<2)return;
   if(hbe_setup_frame==3)horizontal_shake=bn::regular_bg_position_hbe_ptr::create_horizontal(*bg,shake_x);
   if(hbe_setup_frame==4)vertical_shake=bn::regular_bg_position_hbe_ptr::create_vertical(*bg,shake_y);
   if(hbe_setup_frame==5){
    line(6,8,characters[selected].name,false,1,&hud_names);line(144,8,characters[cpu].name,false,1,&hud_names);
    if(versus.active)for(int who=0;who<2;++who){player_marks[who]=bn::sprite_items::versus_marker.create_sprite(0,0,who);player_marks[who]->set_bg_priority(1);}
   }
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
    if(player_marks[i]){
     int marker_y=v.y/256-37;if(marker_y<50)marker_y=50;
     player_marks[i]->set_position(bn::fixed(v.x)/256-120+camera_x,marker_y-80+camera_y);
     player_marks[i]->set_visible(sp.visible());
    }
    int tint=recalling?round.absorb_q8():(feel.flash[i]?77:0),tint_key=tint+(recalling?512:0);
    if(flash_state[i]!=tint_key){auto palette=sp.palette();palette.set_fade(recalling?bn::color(31,10,20):bn::color(31,31,31),bn::fixed(tint)/256);flash_state[i]=tint_key;}
    int height=world_floor_y-v.y/256;if(height<0)height=0;bn::fixed shadow_scale=bn::fixed(1)-bn::fixed(height)/80;if(shadow_scale<bn::fixed(.45))shadow_scale=bn::fixed(.45);
    if(recalling)shadow_scale*=round.age>=92?bn::fixed(.35):sx;
    shadows[i]->set_visible(!recalling||round.body_visible(i)||round.ball_visible());shadows[i]->set_scale(shadow_scale);shadows[i]->set_position(bn::fixed(v.x)/256-120+camera_x,world_floor_y-80+camera_y);
    int punch_frame=screen==FIGHTING&&v.action==MELEE?power_punch_frame(age,characters[v.character].melee_start,characters[v.character].melee_active):-1;
    if(punch_frame>=0){
     if(!punches[i]){punches[i]=punch_items[v.character]->create_sprite(0,0,punch_frame);punches[i]->set_bg_priority(2);punches[i]->set_z_order(-2);}
     if(punch_tile[i]!=punch_frame){punches[i]->set_tiles(punch_items[v.character]->tiles_item(),punch_frame);punch_tile[i]=punch_frame;}
     punches[i]->set_visible(true);punches[i]->set_horizontal_flip(v.face<0);punches[i]->set_position(bn::fixed(v.x)/256+v.face*(characters[v.character].melee_reach-4)-120+camera_x,bn::fixed(v.y)/256-19-80+camera_y);
     pkmn_power_debug[18+i]=punch_frame+1;pkmn_power_debug[2]|=1<<punch_frame;pkmn_power_debug[3]|=1<<v.character;pkmn_power_debug[13]|=v.face<0?2:1;++pkmn_power_debug[6];
    }else if(punches[i])punches[i]->set_visible(false);
    bool charge=screen==FIGHTING&&v.action==SUPER&&age>=0&&age<super_startup_ticks;
    bool floating=screen==FIGHTING&&core.air_braking(i);
    if(floating)pkmn_technique_debug[30]|=1<<i;
    if(floating)sp.set_scale(bn::fixed(1.04),bn::fixed(.96));
    const bool stepping=screen==FIGHTING&&v.action==VOLT_STEP,rolling=screen==FIGHTING&&core.rolling(i);
    if(stepping){sp.set_scale(bn::fixed(1.08),bn::fixed(.92));pkmn_starter_debug[21]|=1<<i;}
    if(rolling){sp.set_scale(bn::fixed(.8));sp.set_rotation_angle((age*36)%360);sp.set_y(bn::fixed(v.y)/256-12-80+camera_y);pkmn_starter_debug[21]|=4<<i;}
    shield[i]->set_visible(screen==FIGHTING&&(v.action==BLOCK||v.action==BLOCKSTUN));shield[i]->set_scale(feel.blocked[i]?bn::fixed(1.08):bn::fixed(1));shield[i]->set_position(bn::fixed(v.x)/256+v.face*11-120+camera_x,bn::fixed(v.y)/256-17-80+camera_y);shield[i]->set_horizontal_flip(v.face<0);
    bool vine=screen==FIGHTING&&v.character==3&&v.action==MELEE&&age>=characters[3].melee_start-2&&age<characters[3].melee_start+characters[3].melee_active;
    int charge_kind=charge?1:floating?2:stepping?3:rolling?4:0;
    const auto& charge_item=*(charge?auras[v.character]:casts[v.character]);
    if((charge||vine||floating||stepping||rolling)&&(!charging[i]||charge_asset[i]!=charge_kind)){charging[i].reset();charging[i]=charge_item.create_sprite(0,0);charging[i]->set_bg_priority(2);charging[i]->set_z_order(-1);charge_asset[i]=charge_kind;charge_tile[i]=-1;}
    if(charging[i])charging[i]->set_visible(charge||vine||floating||stepping||rolling);
    if(charge||vine||floating||stepping||rolling){int phase=charge?(age/3)%8:floating?(core.tick/3)%4:(stepping||rolling)?(age/2)%4:(age-characters[3].melee_start+2)%4;if(charge_tile[i]!=phase){charging[i]->set_tiles(charge_item.tiles_item(),phase);charge_tile[i]=phase;}charging[i]->set_position(bn::fixed(v.x)/256+(floating?0:stepping?-v.mobility_direction*12:v.face*(vine?24:10))-120+camera_x,bn::fixed(v.y)/256-(floating?3:rolling?5:19)-80+camera_y);charging[i]->set_horizontal_flip(v.face<0);charging[i]->set_scale(charge?bn::fixed(1.2)+bn::fixed(age)/100:bn::fixed(1));if(charge){pkmn_power_debug[20+i]=phase+1;pkmn_power_debug[5]|=1<<phase;pkmn_power_debug[12]|=1<<v.character;}}

    if(new_tick&&screen==FIGHTING){if(v.landed_at==core.tick-1||v.took_off_at==core.tick-1)particle(v.x/256,world_floor_y-4,DUST);else if(v.action==NONE&&core.grounded(v)&&v.vx&&core.tick%12==0)particle(v.x/256-v.face*8,world_floor_y-3,DUST);}
    int values[]={v.hp,v.sta,v.sp},maxes[]={characters[v.character].hp,characters[v.character].sta*60,100};for(int j=0;j<3;j++){auto&bar=*bars[i*3+j];bn::fixed width=bn::fixed(71)*values[j]/maxes[j];bar.set_visible(values[j]>0);if(values[j]>0){if(width<1)width=1;bar.set_scale(width/64,bn::fixed(.125));bar.set_position((i?163:25)+width/2-120,18+j*9-80);}}}
   if(presented_events!=core.event_frame&&(screen==FIGHTING||screen==ROUND_END)){
    presented_events=core.event_frame;for(int n=0;n<core.event_count;n++){const auto&e=core.events[n];int ch=core.f[e.owner].character;
     if(e.kind==EV_HIT||e.kind==EV_BLOCK||e.kind==EV_BREAK){const bool orb_burst=e.kind==EV_HIT&&!e.super&&core.p[e.owner].burst_at>=0&&core.p[e.owner].id==e.attack;particle(e.x/256,e.y/256,e.kind==EV_BLOCK?GUARD:e.super||orb_burst?SUPER_IMPACT:IMPACT,ch,core.f[e.owner].face<0);}
     if(e.kind==EV_BREAKAWAY)particle(e.x/256,e.y/256,GUARD,ch,core.f[e.owner].face<0);
     if(e.kind==EV_SHOT||e.kind==EV_RELEASE){particle(e.x/256,e.y/256,CAST,ch,core.f[e.owner].face<0);}
    }
   }
   for(int i=0;i<4;i++){
    const auto&b=core.p[i];
    if(!b.active){bullet[i].reset();bullet_tile[i]=bullet_key[i]=-1;continue;}
    const int character=core.f[b.owner].character,asset_key=b.burst_at>=0?100:character+(b.super?6:0),phase=b.burst_at>=0?core.burst_phase(b):b.super?super_effect_frame(core.tick,b.born):projectile_effect_frame(core.tick,b.born);
    const auto&item=b.burst_at>=0?bn::sprite_items::shadow_burst:*(b.super?waves[character]:shots[character]);
    if(b.burst_at>=0)pkmn_technique_debug[31]|=(phase+1)<<(b.owner*4);
    if(!bullet[i]||bullet_key[i]!=asset_key){bullet[i].reset();bullet[i]=item.create_sprite(0,0,phase);bullet_key[i]=asset_key;bullet_tile[i]=phase;bullet[i]->set_z_order(-3);bullet[i]->set_bg_priority(2);}
    else if(bullet_tile[i]!=phase){bullet[i]->set_tiles(item.tiles_item(),phase);bullet_tile[i]=phase;}
    bullet[i]->set_position(bn::fixed(b.x)/256-120+camera_x,bn::fixed(b.y)/256-80+camera_y);bullet[i]->set_horizontal_flip(b.vx<0);
    pkmn_debug[30]|=1<<(phase+(b.super?4:0));if(b.super){pkmn_power_debug[16+b.owner]=phase+1;pkmn_power_debug[1]|=1<<phase;pkmn_power_debug[4]|=1<<character;}

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
   int live=0;for(auto&p:particles)if(p.sprite){int age=core.tick-p.born;if(age>=lifetime(p.kind)){p.sprite.reset();continue;}++live;int phase=p.kind==SUPER_IMPACT?age/2:age/effect_frame_ticks;if(p.frame!=phase){p.sprite->set_tiles(effect_item(p.kind,p.character).tiles_item(),phase);p.frame=phase;}if(p.kind==SUPER_IMPACT){pkmn_power_debug[11]|=1<<phase;pkmn_power_debug[22]|=1<<phase;}p.sprite->set_position(p.x-120+camera_x,p.y-80+camera_y-(p.kind==DUST?age/4:0));}pkmn_debug[29]=live;
  }
  if(group==3){focus_at(120,52+choice*18,224,14);marker->set_position(-104,-28+choice*18);}
  if(group==12){focus_at(120,mode_row_y[mode_row],224,11);marker->set_position(-104,mode_row_y[mode_row]-80);}
  if(group==15){focus_at(120,66+choice*18,216,14);marker->set_position(-104,66+choice*18-80);}
  if(group==16){
   focus_at(120,choice?148:133,216,12);marker->set_position(-100,(choice?148:133)-80);
   if(fighters[0]){
    const int character=versus.fighters[core.outcome-1];
    pose(0,character,loop_pose(character,CLIP_VICTORY,frame));
    fighters[0]->set_position(0,77-80);
   }
  }
  if(group==8){focus_at(120,66+audio_row*24,224,18);marker->set_position(-96,-14+audio_row*24);}
  if(group==4){int age=frame-result_started;if(core.outcome==2){recall_ball->set_scale(bn::fixed(1.5));recall_ball->set_position(0,6);}else{auto&sp=*fighters[0];Pose current=loop_pose(selected,core.outcome==1?CLIP_VICTORY:CLIP_IDLE,age);pose(0,selected,current);sp.set_scale(1);sp.set_rotation_angle(0);sp.set_position(0,103-26-80);}focus_at(choice?180:60,145,104,18);marker->set_position((choice?140:20)-120,145-80);}

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
  if(group==14)newkey=14000000+versus.fighters[0]+13*versus.fighters[1]+169*int(versus.ready[0])+338*int(versus.ready[1])+676*versus.arena+3380*int(versus_input.connected());
  if(group==17)newkey=17000000+selected_cup*10+difficulty;
  if(group==18)newkey=18000000+cup_run.stage*10+cup_run.result;
  if(group==15)newkey=15000000+choice+10*int(versus_input.connected());
  if(group==16)newkey=16000000+versus.result+10*versus.wins[0]+30*versus.wins[1]+100*choice;
  if(newkey!=key&&((group!=17&&group!=18)||scene_frames>=4)&&((group!=1&&group!=6&&group!=7)||scene_frames>=2)&&(group<9||scene_frames>=2)){key=newkey;labels.clear();
   if(group==1||group==6){
    int row=group==6?practice_row:selector_row;
    bool info=row==(group==6?4:3);
    right_line(230,9,characters[selected].name);
    line(120,selection_footer_y,!row?"A PLAY  SEL OPTIONS  B BACK":info?"A INFO  START PLAY  B GRID":"<> CHANGE  A PLAY  B GRID",true);
   }
   if(group==1){line(180,selection_quick_rows[1],difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD",true);right_line(220,selection_quick_rows[2],cup_mode?"CHOOSE NEXT":arcade_mode?"ROTATES":arena_names[chosen_arena]);}
   if(group==6){line(180,selection_practice_rows[1],practice_mode_names[trainer.mode],true);line(180,selection_practice_rows[2],characters[trainer.opponent].name,true);right_line(220,selection_practice_rows[3],arena_names[chosen_arena]);}
   if(group==12){static const char* hints[]={"1 FIGHT","5 RIVALS","TRAIN","SCORES","ON/OFF","MOVES","MEDALS","2 PLAYERS","3 RIVALS"};line(190,mode_row_y[mode_row],hints[mode_row],true);}
   if(group==14){
    for(int who=0;who<2;++who){
     bn::string<40> name=who?"P2 ":"P1 ";name+=characters[versus.fighters[who]].name;
     name+=versus.ready[who]?" READY":who?"  F READY":"  J READY";
     line(12,who?120:108,name);
    }
    right_line(226,132,arena_names[versus.arena]);
    if(versus.ready[0]||versus.ready[1])line(80,132,"LOCK");
    right_line(230,9,versus_input.connected()?"CONNECTED":"LOAD ADAPTER");
    if(!versus_input.connected())line(120,57,"LOAD LOCAL_VERSUS.LUA",true);
   }
   if(group==17){
    line(120,28,difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD",true);
    const auto& record=cup_progress.record(selected_cup,difficulty);
    right_line(76,114,bn::to_string<4>(record.clears));right_line(149,114,bn::to_string<4>(record.style));right_line(228,114,bn::to_string<4>(record.perfect));
    bn::string<16> best="--";if(record.best_ticks){best=cup_time(record.best_ticks);}line(44,126,best);
    line(120,93,cup_storage.failed?"SAVE FAILED: MEMORY ONLY":"PICK 2 CHALLENGERS",true);
    if(record.clears){
     if(!cup_crest)cup_crest=bn::sprite_items::cup_crest.create_sprite(207-120,60-80,selected_cup);
    }else{
     cup_crest.reset();
     line(207,61,"LOCK",true);
    }
   }
   if(group==18){
    line(120,17,cup_run.definition().name,true,2);
    line(120,37,cup_run.finished?"CUP CHAMPION!":cup_run.result==1?"RIVAL DEFEATED":cup_run.result==3?"DRAW - FREE RETRY":"DEFEATED",true);
    bn::string<40> value="WINS ";value+=bn::to_string<4>(cup_run.stage);value+="/3 CONT ";value+=bn::to_string<4>(cup_run.continues);value+=" TIME ";value+=cup_time(cup_run.ticks);line(120,51,value,true);

   }
   if(group==15)line(120,38,versus_input.connected()?"PAUSED":"RECONNECT INPUT ADAPTER",true);
   if(group==16){
    bn::string<32> heading;
    if(core.outcome==3)heading="DRAW";
    else{heading=core.outcome==1?"P1 WINS ":"P2 WINS ";heading+=versus.complete()?"SET":"ROUND";}
    line(120,41,heading,true,2);
    if(core.outcome!=3)line(120,57,characters[versus.fighters[core.outcome-1]].name,true);
    bn::string<24> score="P1 ";score+=bn::to_string<4>(versus.wins[0]);score+=" - ";score+=bn::to_string<4>(versus.wins[1]);score+=" P2";
    line(120,116,score,true);
    line(120,133,versus.complete()?"J/F REMATCH SET":"J/F NEXT ROUND",true);
    line(120,148,"K/G CHANGE POKEMON",true);
   }
   if(group==3&&cup_run.active)line(120,139,cup_run.continues?"REMATCH COSTS 1 CONTINUE":"NO CONTINUES LEFT",true);
   if(group==3&&arcade.active)line(120,139,arcade.continues?"REMATCH COSTS 1 CONTINUE":"NO CONTINUES LEFT",true);
   if(group==8){line(182,66,sound_settings.music?"ON":"OFF",true);line(182,90,sound_settings.sound?"ON":"OFF",true);}
   if(group==7){const auto&d=characters[selected];line(120,36,d.name,true,2);line(120,54,d.role,true);line(120,72,d.trait,true);line(120,86,d.tip1,true);line(120,98,d.tip2,true);if(d.melee_followup)line(120,136,"EEVEE CAN ALSO FOLLOW WITH A",true);}
   if(group==2){
    if(versus.active){
     bn::string<8> score=bn::to_string<4>(versus.wins[0]);score+="-";score+=bn::to_string<4>(versus.wins[1]);
     line(120,11,score,true);line(120,24,bn::to_string<8>(seconds),true);
    }else if(practice)line(120,14,"TRN",true);else line(120,14,bn::to_string<8>(seconds),true,2);
    if(screen==COUNTDOWN){
     line(120,71,bn::to_string<8>((count+59)/60),true,3);
     if(const auto* rival=cup_run.rival()){
      bn::string<20> name=cup_run.captain_round()?"CAPTAIN ":"";name+=rival->name;
      line(120,104,name,true);line(120,119,rival->cue,true);
     }
    }
    if(screen==RESUME_COUNTDOWN)line(120,71,"READY",true,2);
    if(screen==FIGHTING||screen==ROUND_END){
     static const char* statuses[]={"","LOW STA","BLOCK","NO STA","GUARD BREAK","R READY","PUNISH","FOLLOW B","FOLLOW A/B","AIR BRAKE","B BURST","UP BRAKE","VOLT STEP","SHELL ROLL","L+R ESCAPE","BREAKAWAY"};
     for(int i=0;i<2;i++){int status=feel.status(core,i);if(status)line(i?192:48,47,statuses[status],true);}
     if(charge)line(120,57,"SUPER! JUMP OR SQUEEZE",true);
    }

   }
   if(group>=9&&group<=11){
    const char* diff=difficulty==0?"EASY":difficulty==1?"NORMAL":"HARD";
    if(group==11){
     line(120,45,characters[selected].name,true);line(120,59,diff,true);
     const auto&r=progress.record(selected,difficulty);
     right_line(224,80,bn::to_string<12>(r.clears));
     right_line(224,94,bn::to_string<12>(r.perfect));
     bn::string<24> v;if(r.best_ticks){v=bn::to_string<12>(r.best_ticks/60);v+=" SEC";}else v="--";right_line(224,108,v);
    }else{
     line(120,22,group==10?"CHAMPION!":core.outcome==1?"RIVAL DEFEATED":core.outcome==3?"DRAW - TRY AGAIN":"DEFEATED",true,2);
     bn::string<40> v=characters[selected].name;v+=" / ";v+=diff;line(120,40,v,true);
     v="WINS ";v+=bn::to_string<4>(arcade.stage);v+=" / 5   CONTINUES ";v+=bn::to_string<4>(arcade.continues);line(120,56,v,true);
     v="BATTLE TIME ";v+=bn::to_string<12>(arcade.ticks/60);v+=" SEC";line(120,68,v,true);
     if(group==10){line(120,110,arcade.used?"ARCADE CLEAR!":"NO-CONTINUE CLEAR!",true);line(120,134,"A RECORDS  B MENU",true);}
     else {int rival=scouting_rival(arcade,core.outcome);if(rival>=0){v=core.outcome==1?"NEXT: ":"RETRY: ";v+=characters[rival].name;line(120,108,v,true);}line(120,134,core.outcome==1?"A NEXT RIVAL":core.outcome==3?"A RETRY":arcade.continues?"A USE CONTINUE":"A FINISH RUN",true);line(120,146,"B END RUN",true);}
    }
    if(storage.failed&&group==11)line(120,120,"SAVE FAILED: MEMORY ONLY",true);
   }
   if(group==13){
    line(120,41,characters[selected].name,true);
    bn::string<16> v=bn::to_string<4>(progress.medals(selected));v+=" / 3";line(143,55,v,true);
    const auto&m=progress.mastery[selected];v=bn::to_string<4>(m.followups);v+=" / 10";line(201,75,v,true);
    v=bn::to_string<4>(m.evades);v+=" / 5";line(201,90,v,true);
    line(201,105,progress.no_continue(selected)?"1 / 1":"0 / 1",true);
    line(120,119,storage.failed?"SAVE FAILED: MEMORY ONLY":"QUICK / ARCADE / CUP",true);
   }
   if(group==4){bn::string<40> result=characters[selected].name;result+=core.outcome==1?" WINS!":core.outcome==2?" WILL RETURN!":" - DRAW";line(120,54,result,true);}

  }
  // Stagger Cup text uploads; confirming remains immediate at every phase.
  if(group==18){
   if(scene_frames>=5&&result_detail_phase==0){
    bn::string<40> value=cup_run.definition().goal;value+=" ";value+=bn::to_string<4>(cup_run.style);value+="/";value+=bn::to_string<4>(cup_run.definition().target);line(120,67,value,true,1,&labels);
    if(cup_run.finished){
     value=cup_run.definition().crest;value+=" EARNED";
     line(136,91,value,true);
     line(136,106,cup_run.styled()?(cup_run.used?"STYLE CLEAR":"PERFECT + STYLE"):(cup_run.used?"CUP CLEAR":"PERFECT CLEAR"),true);
    }else if(cup_run.result==1&&!cup_run.rival()){
     line(120,91,"CHOOSE YOUR NEXT RIVAL",true);
     line(120,108,"ONE WIN TO THE CAPTAIN",true);
    }else if(cup_run.result!=2||cup_run.continues){
     const auto& rival=*cup_run.rival();value=cup_run.result==1?"CAPTAIN ":"RETRY: ";value+=rival.name;value+=" / ";value+=characters[rival.fighter].name;
     line(120,91,value,true);line(120,108,rival.cue,true);
    }else line(120,99,"RUN OVER - TRY ANOTHER CUP",true);
    result_detail_phase=1;
   }else if(scene_frames>=6&&result_detail_phase==1){
    if(cup_run.finished){
     cup_crest=bn::sprite_items::cup_crest.create_sprite(31-120,100-80,cup_run.cup);
     cup_crest->set_scale(2);
    }
    if(cup_run.finished)line(120,132,cup_storage.failed?"SAVE FAILED: MEMORY ONLY":cup_run.new_best?"NEW BEST TIME!":"CUP RECORD SAVED",true);
    else if(cup_run.result==1)line(120,132,cup_run.captain_round()?"A FACE CAPTAIN":"A CHOOSE RIVAL",true);
    else line(120,132,cup_run.result==3?"A FREE RETRY":cup_run.continues?"A USE CONTINUE":"A FINISH RUN",true);
    line(120,148,cup_run.finished?"A REMATCH CUP  B CUP BOARD":"B END RUN / CUP BOARD",true,1,&labels);
    result_detail_phase=2;
   }
  }
  // Result text arrives after the background and main labels, not during the
  // busy Arcade transition. No new screen, confirm press or minimum wait.
  if((group==4||group==9||group==10)&&match_goals.ready()){
   if(scene_frames>=3&&result_detail_phase==0){
    const auto& goal=match_goals.goal();bn::string<40> value;
    if(goal.kind==GOAL_COMPLETE)value="ALL 3 MASTERY MEDALS!";
    else{
     if(!goal.advanced)value="NEXT: ";
     value+=goal.label();value+=" ";
     if(goal.advanced){value+=bn::to_string<4>(goal.before);value+=">";}
     value+=bn::to_string<4>(goal.after);value+="/";value+=bn::to_string<4>(goal.target);
    }
    line(120,group==4?124:84,value,true,1,&result_labels);result_detail_bits|=1;result_detail_phase=1;
   }else if(scene_frames>=4&&result_detail_phase==1){
    if(storage.failed){line(120,group==4?112:96,"SAVE FAILED: MEMORY ONLY",true,1,&result_labels);result_detail_bits|=8;}
    else if(medal_earned){line(120,group==4?112:96,"MASTERY MEDAL EARNED!",true,1,&result_labels);result_detail_bits|=4;}
    result_detail_phase=2;
   }else if(scene_frames>=5&&result_detail_phase==2){
    int rival=group==9?scouting_rival(arcade,core.outcome):-1;
    if(rival>=0){bn::string<40> cue=characters[rival].trait;cue+=": ";cue+=scouting_actions[rival];line(120,120,cue,true,1,&result_labels);result_detail_bits|=2;}
    result_detail_phase=3;
   }
   pkmn_goals_debug[10]=result_detail_bits;
  }
  if(cup_run.active&&group==2&&hbe_setup_frame>=7&&practice_labels.empty()){
   bn::string<48> value=cup_run.definition().name;value+=" ";value+=bn::to_string<4>(cup_run.stage+1);value+="/3 CONT ";value+=bn::to_string<4>(cup_run.continues);line(120,137,value,true,1,&practice_labels);
  }
  if(arcade.active&&group==2&&hbe_setup_frame>=6&&practice_labels.empty()){bn::string<48> stage;if(arcade.active){stage=bn::to_string<4>(arcade.stage+1);stage+="/5 ";}stage+=arena_names[battle_arena];if(arcade.active){stage+="  CONT ";stage+=bn::to_string<4>(arcade.continues);}line(120,137,stage,true,1,&practice_labels);}
  if(versus.active&&group==2&&hbe_setup_frame>=6&&practice_labels.empty())line(120,137,"P1 J/K U/I  P2 F/G Q/E",true,1,&practice_labels);
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
 progress=storage.load();cup_progress=cup_storage.load();sound_settings.music=progress.music;sound_settings.sound=progress.sound;
 Screen screen=TITLE,help_return=TITLE,guide_return=SELECTOR,audio_return=TITLE,versus_resume=FIGHTING;
 int selected=0,cpu=1,difficulty=1,choice=0,count=0,frame=0,practice_row=0,practice_rounds=0,audio_row=0,selector_row=0;
 bool practice=false;uint32_t seed=0x504b4d4e;int peak=0,missed=0,versus_paused_count=0;
 uint32_t technique_frame=~0u;int brake_uses[2]{},orb_uses[2]{},burst_hits[2]{},step_uses[2]{},roll_uses[2]{};
 auto save_progress=[&](){progress.music=sound_settings.music;progress.sound=sound_settings.sound;if(storage.save(progress))mastery_dirty=false;};
 auto avoid_mirror=[&](){if(trainer.opponent==selected)trainer.opponent=(selected+1)%character_count;};
 auto start_match=[&](bool keep_stats=false,bool quick_practice=false){
  feel.reset();audio.reset();round.reset();renderer.reset_scene();seed=seed*1664525u+1013904223u;
  technique_frame=~0u;for(int i=0;i<2;++i)brake_uses[i]=orb_uses[i]=burst_hits[i]=step_uses[i]=roll_uses[i]=0;
  pkmn_starter_debug[21]=0;
  if(arcade.active){selected=arcade.fighter;difficulty=arcade.difficulty;cpu=arcade.opponent();}
  const auto* rival=cup_run.rival();
  if(rival){selected=cup_run.fighter;difficulty=cup_run.difficulty;cpu=rival->fighter;cup_run.begin_round();}
  if(versus.active){selected=versus.fighters[0];cpu=versus.fighters[1];}
  if(practice){avoid_mirror();cpu=trainer.opponent;trainer.begin_round(keep_stats);practice_rounds=keep_stats?practice_rounds+1:1;}
  if(mastery_dirty)save_progress();mastery_tracker.reset();match_medals=progress.medals(selected);medal_earned=false;match_goals.begin(progress,selected,practice||versus.active||cup_run.active);
  battle_arena=versus.active?versus.arena:rival?rival->arena:arcade.active?arcade.arena():chosen_arena;
  core.reset(selected,cpu,seed);core.training=practice;
  if(practice)trainer.prepare(core);
  ai.reset(difficulty,seed^0xa5a5a5a5u,rival&&cup_run.captain_round()?cup_run.definition().tactic:CaptainTactic::NONE);
  count=quick_practice?practice_retry_frames:match_countdown_display_ticks;screen=quick_practice?RESUME_COUNTDOWN:COUNTDOWN;choice=0;
  if(versus.active)count=60;
 };
 auto open_cup_picker=[&](){
  choice=cup_run.can_choose(0)?0:1;
  screen=CUP_PICKER;
 };
 while(true){
  const Input hardware=input();
  versus_input.poll(pkmn_versus_input);
  Input keys=versus_input.connected()?versus_input.player(0):hardware;
  if(versus.active&&(screen==PAUSED||screen==RESULT||screen==ROUND_END)){
   keys.held|=versus_input.player(1).held;keys.pressed|=versus_input.player(1).pressed;
  }
  ++frame;Screen previous_screen=screen;int previous_options=selector_row*3+chosen_arena;int previous_menu=selected+choice*10+difficulty*100+practice_row*1000+int(trainer.mode)*10000+trainer.opponent*100000+audio_row*1000000+mode_row*10000000+selected_cup*100000000;AudioCue cue=AUDIO_NONE;bool confirm=keys.pressed&(A|START);
  if(versus.active&&!versus_input.connected()&&screen!=VERSUS_SELECT&&screen!=PAUSED){
   versus_resume=screen;versus_paused_count=count;screen=PAUSED;choice=0;
   core.f[0].pending=core.f[1].pending=0;
  }
  if(!versus.active&&(keys.pressed&L)&&(screen==TITLE||screen==SELECTOR||screen==PRACTICE_SETUP||screen==PAUSED||screen==RESULT)){audio_return=screen;audio_row=0;screen=AUDIO_OPTIONS;}
  else if(screen==AUDIO_OPTIONS){
   if(keys.pressed&(B|START|L)){screen=audio_return;if(progress.music!=sound_settings.music||progress.sound!=sound_settings.sound)save_progress();}
   else if(keys.pressed&(UP|DOWN))audio_row=1-audio_row;
   else if(keys.pressed&(A|LEFT|RIGHT)){if(audio_row==0)sound_settings.music=!sound_settings.music;else sound_settings.sound=!sound_settings.sound;cue=UI_CONFIRM;}
  }
  else if(screen==TITLE){
   if(confirm){cup_mode=cup_run.active=false;practice=false;arcade.active=false;mode_row=0;screen=MODE_MENU;}
   else if(keys.pressed&R){cup_mode=cup_run.active=false;practice=false;arcade_mode=true;arcade.active=false;screen=SELECTOR;}
   else if(keys.pressed&DOWN){cup_mode=cup_run.active=false;mode_row=3;screen=RECORDS;}
   else if(keys.pressed&B){cup_mode=cup_run.active=false;arcade.active=false;arcade_mode=false;practice=true;practice_row=0;avoid_mirror();screen=PRACTICE_SETUP;}
   else if(keys.pressed&SELECT){help_return=TITLE;screen=HELP;}
  }
  else if(screen==MODE_MENU){
   if(keys.pressed&(UP|LEFT))mode_row=adjacent_mode(mode_row,false);
   else if(keys.pressed&(DOWN|RIGHT))mode_row=adjacent_mode(mode_row,true);
   if(keys.pressed&B)screen=TITLE;
   else if(confirm){
    cup_mode=cup_run.active=false;arcade.active=false;arcade_mode=false;practice=false;selector_row=0;
    if(mode_row<2){arcade_mode=mode_row==1;screen=SELECTOR;}
    else if(mode_row==2){arcade_mode=false;practice=true;practice_row=0;avoid_mirror();screen=PRACTICE_SETUP;}
    else if(mode_row==3)screen=RECORDS;
    else if(mode_row==4){audio_return=MODE_MENU;audio_row=0;screen=AUDIO_OPTIONS;}
    else if(mode_row==6)screen=MASTERY;
    else if(mode_row==8){cup_mode=true;screen=CUP_BOARD;}
    else if(mode_row==7){arcade_mode=false;versus.open();versus.fighters[0]=selected;screen=VERSUS_SELECT;}
    else{help_return=MODE_MENU;screen=HELP;}
   }
  }
  else if(screen==CUP_BOARD){
   if(keys.pressed&B){cup_mode=false;screen=MODE_MENU;mode_row=8;}
   else if(confirm){selector_row=0;screen=SELECTOR;}
   else{
    if(keys.pressed&LEFT)selected_cup=(selected_cup+cup_count-1)%cup_count;
    else if(keys.pressed&RIGHT)selected_cup=(selected_cup+1)%cup_count;
    else if(keys.pressed&UP)difficulty=(difficulty+2)%3;
    else if(keys.pressed&DOWN)difficulty=(difficulty+1)%3;
    if(keys.pressed&(LEFT|RIGHT))renderer.reset_scene();
   }
  }
  else if(screen==CUP_PICKER){
   if(keys.pressed&B){
    cup_run.active=false;screen=cup_run.stage?CUP_BOARD:SELECTOR;
   }else if(confirm){
    if(cup_run.choose_challenger(choice))start_match();
   }else if(keys.pressed&(LEFT|RIGHT)){
    const int direction=keys.pressed&LEFT?-1:1;
    do{choice=(choice+cup_challengers+direction)%cup_challengers;}while(!cup_run.can_choose(choice));
   }
  }
  else if(screen==CUP_RESULT){
   if(keys.pressed&B){cup_run.active=false;screen=CUP_BOARD;}
   else if(confirm){
    if(cup_run.finished){cup_run.begin(selected_cup,selected,difficulty);open_cup_picker();}
    else if(cup_run.result==1){if(cup_run.rival())start_match();else open_cup_picker();}
    else if(cup_run.retry(cup_run.result))start_match();
    else{cup_run.active=false;screen=CUP_BOARD;}
   }
  }
  else if(screen==VERSUS_SELECT){
   if(!versus_input.connected()){
    versus.unready();
    if(keys.pressed&B){versus.active=false;mode_row=7;screen=MODE_MENU;}
   }else{
    const Input p1=versus_input.player(0),p2=versus_input.player(1);
    if((p1.pressed&B)&&!versus.ready[0]&&!versus.ready[1]){
     versus.active=false;mode_row=7;screen=MODE_MENU;
    }else{
     if(!versus.ready[0]&&!versus.ready[1]){
      if(p1.pressed&L)versus.arena=(versus.arena+arena_count-1)%arena_count;
      else if(p1.pressed&R)versus.arena=(versus.arena+1)%arena_count;
     }
     versus.choose(0,p1);versus.choose(1,p2);
     if(versus.both_ready())start_match();
    }
   }
  }
  else if(screen==HELP){if(keys.pressed&(A|B|START|SELECT))screen=help_return;}
  else if(screen==FIGHTER_GUIDE){if(keys.pressed&(A|B|START|R))screen=guide_return;}
  else if(screen==SELECTOR){
   if(keys.pressed&B){if(selector_row)selector_row=0;else{screen=cup_mode?CUP_BOARD:MODE_MENU;mode_row=arcade_mode?1:0;}}
   else if(keys.pressed&SELECT)selector_row=selector_row?0:1;
   else if((keys.pressed&R)||((keys.pressed&A)&&!(keys.pressed&START)&&selector_row==3)){guide_return=SELECTOR;screen=FIGHTER_GUIDE;}
   else if(confirm){
    seed^=uint32_t(frame);
    if(cup_mode){cup_run.begin(selected_cup,selected,difficulty);selector_row=0;open_cup_picker();}
    else{
     if(arcade_mode)arcade.begin(selected,difficulty,seed);
     cpu=(selected+1+int(seed%(character_count-1)))%character_count;start_match();
    }
   }
   else if(selector_row==1){if(keys.pressed&UP)selector_row=0;else if(keys.pressed&DOWN)selector_row=(arcade_mode||cup_mode)?3:2;else if(keys.pressed&LEFT)difficulty=(difficulty+2)%3;else if(keys.pressed&RIGHT)difficulty=(difficulty+1)%3;}
   else if(selector_row==2){if(keys.pressed&UP)selector_row=1;else if(keys.pressed&DOWN)selector_row=3;else if(keys.pressed&LEFT)chosen_arena=(chosen_arena+arena_count-1)%arena_count;else if(keys.pressed&RIGHT)chosen_arena=(chosen_arena+1)%arena_count;}
   else if(selector_row==3){if(keys.pressed&UP)selector_row=(arcade_mode||cup_mode)?1:2;}
   else{if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;if(keys.pressed&RIGHT)selected=(selected+1)%character_count;if(keys.pressed&UP)selected=roster_up(selected);if(keys.pressed&DOWN){if(!roster_has_down(selected))selector_row=1;else selected+=roster_columns;}}
  }
  else if(screen==PRACTICE_SETUP){
   if(keys.pressed&B){if(practice_row)practice_row=0;else{screen=MODE_MENU;mode_row=2;practice=false;}}
   else if(keys.pressed&SELECT)practice_row=practice_row?0:1;
   else if((keys.pressed&R)||((keys.pressed&A)&&!(keys.pressed&START)&&practice_row==4)){guide_return=PRACTICE_SETUP;screen=FIGHTER_GUIDE;}
   else if(confirm){start_match();}
   else if(practice_row==0){
    if(keys.pressed&LEFT)selected=(selected+character_count-1)%character_count;if(keys.pressed&RIGHT)selected=(selected+1)%character_count;
    if(keys.pressed&UP)selected=roster_up(selected);
    if(keys.pressed&DOWN){if(!roster_has_down(selected))practice_row=1;else selected+=roster_columns;}
    avoid_mirror();trainer.validate_mode(selected);
   }
   else{
    if(keys.pressed&UP)--practice_row;
    else if((keys.pressed&DOWN)&&practice_row<4)++practice_row;
    else if(practice_row<4&&(keys.pressed&(LEFT|RIGHT))){
     int direction=keys.pressed&RIGHT?1:-1;
     if(practice_row==1)trainer.cycle_mode(selected,direction);
     else if(practice_row==3)chosen_arena=(chosen_arena+arena_count+direction)%arena_count;
     else{trainer.opponent=(trainer.opponent+character_count+direction)%character_count;if(trainer.opponent==selected)trainer.opponent=(trainer.opponent+character_count+direction)%character_count;}
    }
   }
  }
  else if(screen==COUNTDOWN||screen==RESUME_COUNTDOWN){core.f[0].pending=core.f[1].pending=0;if(--count<=0){screen=FIGHTING;}}
  else if(screen==FIGHTING){
   if((keys.pressed&START)||(versus.active&&(versus_input.player(1).pressed&START))){versus_resume=FIGHTING;versus_paused_count=count;screen=PAUSED;choice=0;core.f[0].pending=core.f[1].pending=0;}
   else if(practice&&(keys.pressed&SELECT))start_match(false,true);
   else{if(arcade.active)++arcade.ticks;core.frame(keys,versus.active?versus_input.player(1):practice?trainer.input(core):ai.input(core));if(cup_run.active)cup_run.observe(core);if(practice)trainer.observe(core);else if(!versus.active){mastery_dirty=mastery_tracker.observe(core,progress)||mastery_dirty;medal_earned=progress.medals(selected)>match_medals;}if(core.outcome){round.begin(core,practice);screen=ROUND_END;count=round.duration();}}
  }
  else if(screen==PAUSED&&versus.active){
   if(keys.pressed&(UP|LEFT))choice=(choice+3)%4;
   else if(keys.pressed&(DOWN|RIGHT))choice=(choice+1)%4;
   bool resume=(keys.pressed&(B|START))||(confirm&&choice==0);
   if(resume&&versus_input.connected()){
    screen=versus_resume;count=versus_paused_count;
    if(screen==FIGHTING){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}
   }else if((keys.pressed&B)&&!versus_input.connected()){
    versus.reset_set();screen=VERSUS_SELECT;
   }else if(confirm){
    if(choice==1&&versus_input.connected()){versus.reset_set();start_match();}
    else if(choice==2){versus.reset_set();screen=VERSUS_SELECT;}
    else if(choice==3){versus.active=false;screen=MODE_MENU;mode_row=7;}
   }
  }
  else if(screen==PAUSED){
   if(keys.pressed&(UP|LEFT)){choice=(choice+4)%5;if(((arcade.active&&!arcade.continues)||(cup_run.active&&!cup_run.continues))&&choice==1)choice=0;}
   else if(keys.pressed&(DOWN|RIGHT)){choice=(choice+1)%5;if(((arcade.active&&!arcade.continues)||(cup_run.active&&!cup_run.continues))&&choice==1)choice=2;}
   if((keys.pressed&B)||(keys.pressed&START)){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}
   else if(confirm){
    if(choice==0){screen=RESUME_COUNTDOWN;count=match_resume_countdown_display_ticks;}
    else if(choice==1){if(cup_run.active?cup_run.retry(2):!arcade.active||arcade.retry(2))start_match(false,practice);}
    else if(choice==2){if(mastery_dirty)save_progress();arcade.active=false;cup_run.active=false;screen=practice?PRACTICE_SETUP:SELECTOR;selector_row=0;practice_row=0;}
    else if(choice==3){help_return=PAUSED;screen=HELP;}else{audio_return=PAUSED;audio_row=0;screen=AUDIO_OPTIONS;}
   }
  }
  else if(screen==ROUND_END){round.advance();if(round.age==8&&mastery_dirty)save_progress();if(keys.pressed&(A|START))round.skip();count=round.duration()-round.age;if(round.done()){if(practice)start_match(true,true);else{screen=RESULT;choice=0;if(versus.active)versus.finish_round(core.outcome);if(arcade.active){if(core.outcome==1)arcade.advance();if(arcade.finished){progress.complete(arcade);medal_earned=progress.medals(selected)>match_medals;save_progress();screen=CHAMPION;}else screen=ARCADE_RESULT;}if(cup_run.active){cup_run.finish_round(core.outcome);if(cup_progress.complete(cup_run))cup_storage.save(cup_progress);screen=CUP_RESULT;}if(!versus.active&&!cup_run.active)match_goals.finish(progress);}}}
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
  else if(screen==RESULT&&versus.active){
   if(keys.pressed&(LEFT|RIGHT|UP|DOWN))choice=1-choice;
   if((keys.pressed&B)||(confirm&&choice==1)){versus.reset_set();screen=VERSUS_SELECT;}
   else if(confirm){if(versus.complete())versus.reset_set();start_match();}
  }
  else if(screen==RESULT){if(keys.pressed&(LEFT|RIGHT|UP|DOWN))choice=1-choice;if(keys.pressed&B){screen=SELECTOR;selector_row=0;}else if(confirm){if(choice==0)start_match();else{screen=SELECTOR;selector_row=0;}}}
  if(screen==PRACTICE_SETUP)trainer.validate_mode(selected);
  int current_menu=selected+choice*10+difficulty*100+practice_row*1000+int(trainer.mode)*10000+trainer.opponent*100000+audio_row*1000000+mode_row*10000000+selected_cup*100000000;
  if(previous_screen!=screen){
   if(previous_screen==FIGHTING)for(auto&v:core.f){v.tap_direction=0;v.tap_tick=-100;}
   if(previous_screen==AUDIO_OPTIONS)cue=UI_BACK;else if(screen==PAUSED)cue=UI_PAUSE;else if(screen==RESUME_COUNTDOWN)cue=UI_RESUME;
   else if(screen==COUNTDOWN)cue=ROUND_COUNT;else if(screen==FIGHTING)cue=ROUND_FIGHT;
   else if(screen==CUP_RESULT)cue=cup_run.result==1?RESULT_WIN:cup_run.result==2?RESULT_LOSE:RESULT_DRAW;
   else if(screen==CHAMPION)cue=RESULT_WIN;else if(screen==ARCADE_RESULT)cue=core.outcome==1?RESULT_WIN:core.outcome==2?RESULT_LOSE:RESULT_DRAW;
   else if(screen==RESULT)cue=core.outcome==3?RESULT_DRAW:versus.active||core.outcome==1?RESULT_WIN:RESULT_LOSE;
   else if(screen!=ROUND_END)cue=(previous_screen==AUDIO_OPTIONS||previous_screen==HELP||previous_screen==FIGHTER_GUIDE||screen==TITLE)?UI_BACK:UI_CONFIRM;
  }else if(cue==AUDIO_NONE&&(previous_menu!=current_menu||previous_options!=selector_row*3+chosen_arena)&&screen!=FIGHTING&&screen!=ROUND_END)cue=UI_MOVE;
  if(screen==COUNTDOWN&&(count==120||count==60))cue=ROUND_COUNT;
  if(screen==ROUND_END){if(round.age==18)cue=ROUND_KO;if(round.recall){if(round.age==44)cue=BALL_OPEN;else if(round.age==56)cue=BALL_ABSORB;else if(round.age==140)cue=BALL_SEAL;}}
  bool active=screen==FIGHTING||screen==ROUND_END;feel.update(core,active);
  audio.update(core,sound_settings.sound&&(screen==FIGHTING||(screen==ROUND_END&&round.age<18)));
  bool menu_music=screen==CUP_BOARD||screen==CUP_RESULT||screen==CUP_PICKER||screen==VERSUS_SELECT||screen==MASTERY||screen==MODE_MENU||screen==ARCADE_RESULT||screen==CHAMPION||screen==RECORDS||screen==TITLE||screen==SELECTOR||screen==PRACTICE_SETUP||screen==FIGHTER_GUIDE||screen==AUDIO_OPTIONS||(screen==HELP&&help_return!=PAUSED);
  ambience.update(menu_music,sound_settings);if(cue!=AUDIO_NONE)ambience.cue(cue,sound_settings);
  pkmn_versus_debug[0]=0x56533132;
  pkmn_versus_debug[1]=versus.active;
  pkmn_versus_debug[2]=versus_input.connected();
  pkmn_versus_debug[3]=versus.fighters[0];pkmn_versus_debug[4]=versus.fighters[1];
  pkmn_versus_debug[5]=versus.ready[0];pkmn_versus_debug[6]=versus.ready[1];
  pkmn_versus_debug[7]=versus.wins[0];pkmn_versus_debug[8]=versus.wins[1];
  pkmn_versus_debug[9]=versus.arena;pkmn_versus_debug[10]=versus.complete();
  pkmn_versus_debug[11]=versus_input.player(0).held;pkmn_versus_debug[12]=versus_input.player(1).held;
  pkmn_versus_debug[13]=versus_input.player(0).pressed;pkmn_versus_debug[14]=versus_input.player(1).pressed;
  pkmn_versus_debug[15]=versus.active?core.checksum():0;
  pkmn_versus_debug[16]=reinterpret_cast<uint32_t>(&pkmn_versus_descriptor);
  pkmn_versus_debug[17]=pkmn_versus_input.sequence;
  pkmn_cup_debug[0]=0x43555034;pkmn_cup_debug[1]=cup_mode;pkmn_cup_debug[2]=cup_run.active;
  pkmn_cup_debug[3]=selected_cup;pkmn_cup_debug[4]=cup_run.stage;pkmn_cup_debug[5]=cup_run.continues;
  pkmn_cup_debug[6]=cup_run.used;pkmn_cup_debug[7]=cup_run.ticks;pkmn_cup_debug[8]=cup_run.style;
  pkmn_cup_debug[9]=cup_run.finished;pkmn_cup_debug[10]=cup_run.awarded;pkmn_cup_debug[11]=cup_run.result;
  const auto* rival=cup_run.rival();
  pkmn_cup_debug[12]=rival?rival->fighter:-1;pkmn_cup_debug[13]=rival?rival->arena:-1;
  const auto& cup_record=cup_progress.record(selected_cup,difficulty);
  pkmn_cup_debug[14]=cup_record.clears;pkmn_cup_debug[15]=cup_record.style;pkmn_cup_debug[16]=cup_record.perfect;
  pkmn_cup_debug[17]=cup_record.best_ticks;pkmn_cup_debug[18]=cup_storage.failed;
  pkmn_cup_debug[19]=cup_run.definition().target;pkmn_cup_debug[20]=cup_run.last_stage;pkmn_cup_debug[21]=cup_run.new_best;
  pkmn_cup_debug[22]=sizeof(CupSave);pkmn_cup_debug[23]=cup_save_offsets[0];pkmn_cup_debug[24]=cup_save_offsets[1];
  pkmn_cup_debug[25]=cup_run.rival_slot;pkmn_cup_debug[26]=cup_run.defeated;
  pkmn_cup_debug[27]=rival&&cup_run.captain_round()?int(cup_run.definition().tactic):0;
  pkmn_arcade_debug[0]=0x41524338;pkmn_arcade_debug[1]=arcade_mode;pkmn_arcade_debug[2]=arcade.active;pkmn_arcade_debug[3]=arcade.stage;pkmn_arcade_debug[4]=arcade.continues;pkmn_arcade_debug[5]=arcade.used;pkmn_arcade_debug[6]=arcade.ticks;pkmn_arcade_debug[7]=arcade.finished;pkmn_arcade_debug[8]=progress.record(selected,difficulty).clears;pkmn_arcade_debug[9]=progress.record(selected,difficulty).perfect;pkmn_arcade_debug[10]=progress.record(selected,difficulty).best_ticks;pkmn_arcade_debug[11]=storage.failed;
  for(int i=0;i<arcade_rivals;++i)pkmn_arcade_debug[12+i]=arcade.roster[i];pkmn_arcade_debug[17]=mode_row;pkmn_arcade_debug[18]=character_count;pkmn_arcade_debug[19]=save_words;pkmn_arcade_debug[20]=choice;
  pkmn_mastery_debug[0]=0x4d415339;pkmn_mastery_debug[1]=chosen_arena;pkmn_mastery_debug[2]=battle_arena;pkmn_mastery_debug[3]=selector_row;pkmn_mastery_debug[4]=progress.mastery[selected].followups;pkmn_mastery_debug[5]=progress.mastery[selected].evades;pkmn_mastery_debug[6]=progress.no_continue(selected);pkmn_mastery_debug[7]=progress.medals(selected);pkmn_mastery_debug[8]=mastery_dirty;pkmn_mastery_debug[9]=medal_earned;pkmn_mastery_debug[10]=save_schema;pkmn_mastery_debug[11]=sizeof(SaveSlot);pkmn_mastery_debug[12]=frame/24;pkmn_mastery_debug[13]=arena_sway(battle_arena,80,screen==FIGHTING?frame/24:-1);
  bool goals_visible=match_goals.ready()&&(screen==RESULT||screen==ARCADE_RESULT||screen==CHAMPION);
  const auto& goal=match_goals.goal();int scout=screen==ARCADE_RESULT?scouting_rival(arcade,core.outcome):-1;
  pkmn_goals_debug[0]=0x474f414c;pkmn_goals_debug[1]=goals_visible;pkmn_goals_debug[2]=goals_visible?goal.kind:GOAL_NONE;
  pkmn_goals_debug[3]=goal.before;pkmn_goals_debug[4]=goal.after;pkmn_goals_debug[5]=goal.target;pkmn_goals_debug[6]=goal.advanced;
  pkmn_goals_debug[7]=match_goals.who();pkmn_goals_debug[8]=scout+1;pkmn_goals_debug[9]=scout>=0;
  pkmn_power_debug[0]=0x50313100;pkmn_power_debug[10]=character_count;
  pkmn_expansion_debug[0]=0x45585032;pkmn_expansion_debug[1]=character_count;pkmn_expansion_debug[2]=arena_count;
  pkmn_expansion_debug[3]=roster_page(selected);pkmn_expansion_debug[4]=roster_page_count(selected);pkmn_expansion_debug[5]=save_schema;
  pkmn_expansion_debug[6]=sizeof(SaveSlot);pkmn_expansion_debug[7]=save_offsets[0];pkmn_expansion_debug[8]=save_offsets[1];
  if(technique_frame!=core.event_frame){
   technique_frame=core.event_frame;
   for(int n=0;n<core.event_count;++n){const auto&e=core.events[n];if(e.kind==EV_AIR_BRAKE)++brake_uses[e.owner];if(e.kind==EV_ORB_ARM)++orb_uses[e.owner];if(e.kind==EV_BURST_HIT)++burst_hits[e.owner];if(e.kind==EV_VOLT_STEP)++step_uses[e.owner];if(e.kind==EV_SHELL_ROLL)++roll_uses[e.owner];}
  }
  pkmn_technique_debug[0]=0x54454333;pkmn_starter_debug[0]=0x53545235;
  for(int i=0;i<2;++i){
   const auto&v=core.f[i];const auto&orb=core.p[i];
   pkmn_technique_debug[1+i]=characters[v.character].technique;
   pkmn_technique_debug[3+i]=v.air_brake_used;pkmn_technique_debug[5+i]=v.air_brake_until;
   pkmn_technique_debug[7+i]=v.vy;pkmn_technique_debug[9+i]=core.can_air_brake(i);
   pkmn_technique_debug[11+i]=core.can_burst(i);pkmn_technique_debug[13+i]=orb.active;
   pkmn_technique_debug[15+i]=orb.burst_at;pkmn_technique_debug[17+i]=orb.x;
   pkmn_technique_debug[19+i]=orb.y;pkmn_technique_debug[21+i]=core.burst_phase(orb);
   pkmn_starter_debug[1+i]=characters[v.character].technique;pkmn_starter_debug[3+i]=step_uses[i];pkmn_starter_debug[5+i]=roll_uses[i];pkmn_starter_debug[7+i]=v.step_ready;pkmn_starter_debug[9+i]=v.mobility_direction;pkmn_starter_debug[11+i]=v.shell_roll;pkmn_starter_debug[13+i]=core.rolling(i);pkmn_starter_debug[15+i]=v.tap_direction;pkmn_starter_debug[17+i]=v.tap_tick;pkmn_starter_debug[19+i]=v.action==VOLT_STEP?core.age(v):-1;
   pkmn_technique_debug[24+i]=brake_uses[i];pkmn_technique_debug[26+i]=orb_uses[i];pkmn_technique_debug[28+i]=burst_hits[i];
  }
  unsigned technique_mask=0;for(int n=0;n<core.event_count;++n)technique_mask|=1u<<core.events[n].kind;
  pkmn_technique_debug[23]=technique_mask;
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
