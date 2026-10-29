#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/blip_buf.h>
#include <mgba/core/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static FILE* capture_audio=NULL;static unsigned audio_samples=0,audio_rate=32768;
static void u16(FILE*f,unsigned v){fputc(v&255,f);fputc((v>>8)&255,f);}
static void u32(FILE*f,unsigned v){u16(f,v);u16(f,v>>16);}
static void finish_audio(void){if(!capture_audio)return;fseek(capture_audio,0,SEEK_SET);fwrite("RIFF",1,4,capture_audio);u32(capture_audio,36+audio_samples*4);fwrite("WAVEfmt ",1,8,capture_audio);u32(capture_audio,16);u16(capture_audio,1);u16(capture_audio,2);u32(capture_audio,audio_rate);u32(capture_audio,audio_rate*4);u16(capture_audio,4);u16(capture_audio,16);fwrite("data",1,4,capture_audio);u32(capture_audio,audio_samples*4);fclose(capture_audio);capture_audio=NULL;}
static void run_frame(struct mCore*c){c->runFrame(c);short samples[2048];struct blip_t*l=c->getAudioChannel(c,0),*r=c->getAudioChannel(c,1);int count=blip_samples_avail(l);if(count>1024)count=1024;if(count>blip_samples_avail(r))count=blip_samples_avail(r);if(count>0){blip_read_samples(l,samples,count,1);blip_read_samples(r,samples+1,count,1);if(capture_audio){for(int i=0;i<count*2;i++)u16(capture_audio,(uint16_t)samples[i]);audio_samples+=count;}}}
static void quiet(struct mLogger* l,int cat,enum mLogLevel level,const char*fmt,va_list args){if(level==mLOG_FATAL||level==mLOG_ERROR){vfprintf(stderr,fmt,args);fputc(10,stderr);}}
int main(int argc,char**argv){if(argc<2)return 2;struct mLogger logger={quiet,NULL};mLogSetDefaultLogger(&logger);struct mCore*c=mCoreFind(argv[1]);if(!c||!c->init(c))return 3;mCoreConfigInit(&c->config,"pkmn-test");mCoreConfigSetDefaultIntValue(&c->config,"skipBios",1);mCoreConfigSetDefaultIntValue(&c->config,"useBios",0);mCoreConfigSetDefaultIntValue(&c->config,"volume",256);mCoreConfigSetDefaultIntValue(&c->config,"mute",0);mCoreConfigSetDefaultIntValue(&c->config,"sampleRate",32768);mCoreLoadConfig(c);static color_t video[240*160];c->setVideoBuffer(c,video,240);c->setAudioBufferSize(c,1024);if(!mCoreLoadFile(c,argv[1]))return 4;c->reset(c);blip_set_rates(c->getAudioChannel(c,0),c->frequency(c),audio_rate);blip_set_rates(c->getAudioChannel(c,1),c->frequency(c),audio_rate);char op[32],file[512];unsigned frames,keys,address,value;
while(scanf("%31s",op)==1){if(op[0]=='f'){scanf("%u %u",&frames,&keys);c->setKeys(c,keys);for(unsigned i=0;i<frames;i++)run_frame(c);printf("frames %u\n",c->frameCounter(c));}
else if(op[0]=='d'){scanf("%511s",file);FILE*f=fopen(file,"wb");fprintf(f,"P6\n240 160\n255\n");for(int i=0;i<240*160;i++){uint32_t v=video[i];fputc(v&255,f);fputc((v>>8)&255,f);fputc((v>>16)&255,f);}fclose(f);printf("dump %s\n",file);}
else if(op[0]=='r'){scanf("%x",&address);printf("read %08x %08x\n",address,c->busRead32(c,address));}
else if(op[0]=='s'){scanf("%x",&address);printf("state");for(int i=0;i<32;i++)printf(" %u",c->busRead32(c,address+i*4));printf("\n");}
else if(op[0]=='v'){scanf("%x",&address);printf("feel");for(int i=0;i<16;i++)printf(" %u",c->busRead32(c,address+i*4));printf("\n");}
else if(op[0]=='o'){scanf("%511s",file);finish_audio();capture_audio=fopen(file,"wb");if(!capture_audio)return 5;for(int i=0;i<44;i++)fputc(0,capture_audio);audio_samples=0;printf("audio open\n");}
else if(op[0]=='c'){finish_audio();printf("audio closed samples=%u rate=%u\n",audio_samples,audio_rate);}
else if(op[0]=='a'||op[0]=='b'){scanf("%u %x",&frames,&address);unsigned feel_address=0;if(op[0]=='b')scanf("%x",&feel_address);unsigned n,super_count=0,maxsp=0,defense=0,prevact=0,peak=0,effect_frames=0,effect_themes=0,peak_particles=0;unsigned events=0,themes=0,charging=0,released=0,voices=0,shake=0;int threat_age=-1;for(n=0;n<frames;n++){
 unsigned state=c->busRead32(c,address+4);if(state==6)break;
 keys=0;if(state==3){int action=c->busRead32(c,address+16*4),enemy=c->busRead32(c,address+17*4),sp=c->busRead32(c,address+10*4),sta=c->busRead32(c,address+8*4),x=c->busRead32(c,address+12*4),ex=c->busRead32(c,address+13*4),y=c->busRead32(c,address+14*4),dist=abs(ex-x)/256;
 if((unsigned)sp>maxsp)maxsp=sp;if(action==5&&prevact!=5)super_count++;prevact=action;
 if(enemy==5){if(threat_age<0)threat_age=0;else threat_age++;}else if(threat_age>=0&&threat_age<100)threat_age++;else threat_age=-1;
 if(n%6==0){if(sp==100&&(sta<1500||(feel_address&&dist<80&&((ex>x&&x>34*256)||(ex<x&&x<206*256))))){keys=ex>x?32:16;}
 else if(threat_age>=0&&30+dist/5-threat_age<=14&&30+dist/5-threat_age>=8&&action<=1&&sta>=1080){keys=128;defense++;}
 else if(sp==100&&sta>=1500&&y>=120*256)keys=256;
 else if(dist<27&&sta>=480)keys=1;
 else if(dist>48&&sta>=1800&&n%18==0)keys=2;
 else if(dist>24)keys=ex>x?16:32;
 }else if(action==0){if(feel_address&&sp==100&&dist<80)keys=ex>x?32:16;else if(dist>24)keys=ex>x?16:32;}
 }
 c->setKeys(c,keys);run_frame(c);unsigned usage=c->busRead32(c,address+21*4);if(usage>peak)peak=usage;
 effect_frames|=c->busRead32(c,address+30*4);effect_themes|=c->busRead32(c,address+31*4);unsigned particles=c->busRead32(c,address+29*4);if(particles>peak_particles)peak_particles=particles;
 if(feel_address){events|=c->busRead32(c,feel_address+4);themes|=c->busRead32(c,feel_address+5*4);charging|=c->busRead32(c,feel_address+6*4);released|=c->busRead32(c,feel_address+7*4);unsigned v=c->busRead32(c,feel_address+4*4);if(v>voices)voices=v;int x=(int)c->busRead32(c,feel_address+2*4);unsigned a=abs(x);if(a>shake)shake=a;}
 }printf("auto frames=%u supers=%u max_sp=%u squeeze_attempts=%u peak_q12=%u effect_frames=%u effect_themes=%u peak_particles=%u events=%u audio_themes=%u charge_themes=%u release_themes=%u voices=%u shake=%u\n",n,super_count,maxsp,defense,peak,effect_frames,effect_themes,peak_particles,events,themes,charging,released,voices,shake);}
else if(op[0]=='w'){scanf("%x %x",&address,&value);c->busWrite32(c,address,value);printf("write %08x %08x\n",address,value);}
else if(op[0]=='q')break;fflush(stdout);}finish_audio();c->deinit(c);return 0;}
