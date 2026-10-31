#pragma once
#include "bn_optional.h"
#include "bn_sound_handle.h"
namespace pkmn {
struct AudioSettings { bool music=true,sound=true; };
enum AudioCue { UI_MOVE,UI_CONFIRM,UI_BACK,UI_PAUSE,UI_RESUME,ROUND_COUNT,ROUND_FIGHT,ROUND_KO,BALL_OPEN,BALL_ABSORB,BALL_SEAL,RESULT_WIN,RESULT_LOSE,RESULT_DRAW,AUDIO_CUE_COUNT,AUDIO_NONE=AUDIO_CUE_COUNT };
class PresentationAudio {
 bn::optional<bn::sound_handle> voice;int music_level=0,clock=0,last_move=-10;
public:
 unsigned played_mask=0;int played=0;
 void update(bool menu_music,const AudioSettings&settings);
 void cue(AudioCue cue,const AudioSettings&settings);
 int active_voices()const;
 int music_gain_q8()const{return music_level;}
 bool music_playing()const;
 int music_position()const;
};
}
