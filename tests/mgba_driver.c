#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static void quiet(struct mLogger* l,int cat,enum mLogLevel level,const char*fmt,va_list args){if(level==mLOG_FATAL||level==mLOG_ERROR){vfprintf(stderr,fmt,args);fputc(10,stderr);}}
int main(int argc,char**argv){if(argc<2)return 2;struct mLogger logger={quiet,NULL};mLogSetDefaultLogger(&logger);struct mCore*c=mCoreFind(argv[1]);if(!c||!c->init(c))return 3;mCoreConfigInit(&c->config,"pkmn-test");mCoreConfigSetDefaultIntValue(&c->config,"skipBios",1);mCoreConfigSetDefaultIntValue(&c->config,"useBios",0);mCoreLoadConfig(c);static color_t video[240*160];c->setVideoBuffer(c,video,240);c->setAudioBufferSize(c,1024);if(!mCoreLoadFile(c,argv[1]))return 4;c->reset(c);char op[32],file[512];unsigned frames,keys,address,value;
while(scanf("%31s",op)==1){if(op[0]=='f'){scanf("%u %u",&frames,&keys);c->setKeys(c,keys);for(unsigned i=0;i<frames;i++)c->runFrame(c);printf("frames %u\n",c->frameCounter(c));}
else if(op[0]=='d'){scanf("%511s",file);FILE*f=fopen(file,"wb");fprintf(f,"P6\n240 160\n255\n");for(int i=0;i<240*160;i++){uint32_t v=video[i];fputc(v&255,f);fputc((v>>8)&255,f);fputc((v>>16)&255,f);}fclose(f);printf("dump %s\n",file);}
else if(op[0]=='r'){scanf("%x",&address);printf("read %08x %08x\n",address,c->busRead32(c,address));}
else if(op[0]=='s'){scanf("%x",&address);printf("state");for(int i=0;i<32;i++)printf(" %u",c->busRead32(c,address+i*4));printf("\n");}
else if(op[0]=='a'){scanf("%u %x",&frames,&address);unsigned n,super_count=0,maxsp=0,defense=0,prevact=0,peak=0,effect_frames=0,effect_themes=0,peak_particles=0;int threat_age=-1;for(n=0;n<frames;n++){
 unsigned state=c->busRead32(c,address+4);if(state==6)break;
 keys=0;if(state==3){int action=c->busRead32(c,address+16*4),enemy=c->busRead32(c,address+17*4),sp=c->busRead32(c,address+10*4),sta=c->busRead32(c,address+8*4),x=c->busRead32(c,address+12*4),ex=c->busRead32(c,address+13*4),y=c->busRead32(c,address+14*4),dist=abs(ex-x)/256;
 if((unsigned)sp>maxsp)maxsp=sp;if(action==5&&prevact!=5)super_count++;prevact=action;
 if(enemy==5){if(threat_age<0)threat_age=0;else threat_age++;}else if(threat_age>=0&&threat_age<100)threat_age++;else threat_age=-1;
 if(n%6==0){if(sp==100&&sta<1500){keys=ex>x?32:16;}
 else if(threat_age>=0&&30+dist/5-threat_age<=14&&30+dist/5-threat_age>=8&&action<=1&&sta>=1080){keys=128;defense++;}
 else if(sp==100&&sta>=1500&&y>=120*256)keys=256;
 else if(dist<27&&sta>=480)keys=1;
 else if(dist>48&&sta>=1800&&n%18==0)keys=2;
 else if(dist>24)keys=ex>x?16:32;
 }else if(dist>24&&action==0)keys=ex>x?16:32;
 }
 c->setKeys(c,keys);c->runFrame(c);unsigned usage=c->busRead32(c,address+21*4);if(usage>peak)peak=usage;
 effect_frames|=c->busRead32(c,address+30*4);effect_themes|=c->busRead32(c,address+31*4);unsigned particles=c->busRead32(c,address+29*4);if(particles>peak_particles)peak_particles=particles;
 }printf("auto frames=%u supers=%u max_sp=%u squeeze_attempts=%u peak_q12=%u effect_frames=%u effect_themes=%u peak_particles=%u\n",n,super_count,maxsp,defense,peak,effect_frames,effect_themes,peak_particles);}
else if(op[0]=='w'){scanf("%x %x",&address,&value);c->busWrite32(c,address,value);printf("write %08x %08x\n",address,value);}
else if(op[0]=='q')break;fflush(stdout);}c->deinit(c);return 0;}
