#include "pkmn_presentation_audio.h"
#include "bn_sound_item.h"
#include "bn_sound_items.h"
#include "bn_music.h"
#include "bn_music_items.h"
namespace pkmn {
static const bn::sound_item* const cues[]={
 &bn::sound_items::ui_move,&bn::sound_items::ui_confirm,&bn::sound_items::ui_back,&bn::sound_items::ui_pause,&bn::sound_items::ui_resume,
 &bn::sound_items::round_count,&bn::sound_items::round_fight,&bn::sound_items::round_ko,&bn::sound_items::ball_open,&bn::sound_items::ball_absorb,&bn::sound_items::ball_seal,
 &bn::sound_items::result_win,&bn::sound_items::result_lose,&bn::sound_items::result_draw};
int PresentationAudio::active_voices()const{return voice&&voice->active()?1:0;}
bool PresentationAudio::music_playing()const{return bn::music::playing();}
int PresentationAudio::music_position()const{return bn::music::playing()?bn::music::position():-1;}
void PresentationAudio::update(bool menu_music,const AudioSettings&settings){
 ++clock;
 if(!settings.sound){if(voice&&voice->active())voice->stop();voice.reset();}
 if(!settings.music){if(bn::music::playing())bn::music::stop();music_level=0;return;}
 if(menu_music&&!bn::music::playing()){bn::music_items::title_theme.play(0,true);music_level=0;}
 if(!bn::music::playing())return;
 const int target=menu_music?(active_voices()?64:84):0;
 if(music_level<target){music_level+=3;if(music_level>target)music_level=target;}
 else if(music_level>target){music_level-=6;if(music_level<target)music_level=target;}
 if(!music_level&&!menu_music)bn::music::stop();else bn::music::set_volume(bn::fixed(music_level)/256);
}
void PresentationAudio::cue(AudioCue which,const AudioSettings&settings){
 if(!settings.sound||which<0||which>=AUDIO_CUE_COUNT)return;
 if(which==UI_MOVE&&clock-last_move<4)return;
 if(which==UI_MOVE)last_move=clock;
 if(voice&&voice->active())voice->stop();
 // Source amplitudes are bounded too; long stingers stay below short combat peaks.
 const int volume=which==UI_MOVE?104:which==BALL_ABSORB?96:112;
 voice=cues[which]->play(bn::fixed(volume)/256);
 played_mask|=1u<<which;++played;
}
}
