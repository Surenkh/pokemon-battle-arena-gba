#include "pkmn_round_presentation.h"
#include <cstdio>
using namespace pkmn;
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL presentation:%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
 for(int a=0;a<6;++a)for(int b=0;b<6;++b)for(int outcome=1;outcome<=3;++outcome)for(int practice=0;practice<2;++practice)
 for(int side=0;side<2;++side){CombatCore c;c.reset(a,b,17);c.outcome=outcome;c.f[0].x=(side?208:32)*256;c.f[1].x=(side?32:208)*256;
  RoundPresentation p;p.begin(c,practice);auto before=c.checksum();bool expected=outcome<3&&!practice;CHECK(p.recall==expected);CHECK(p.duration()==(expected?156:60));
  int seen=0,previous_scale=256;
  for(int frame=0;frame<p.duration();++frame){CHECK(p.age==frame);CHECK(!p.done());seen|=1<<p.phase();
   CHECK(p.ball_x_q8()>=16*256&&p.ball_x_q8()<=224*256);CHECK(p.body_scale_q8()>=16&&p.body_scale_q8()<=256);
   if(expected){CHECK(p.loser==2-outcome);CHECK(p.ball_y_q8()>=24*256&&p.ball_y_q8()<=113*256);CHECK(p.body_scale_q8()<=previous_scale);previous_scale=p.body_scale_q8();
    CHECK(p.body_visible(1-p.loser));CHECK(p.body_visible(p.loser)==(frame<92));CHECK(p.beam_visible()==(frame>=48&&frame<92));
    if(frame>=92)CHECK(!p.body_visible(p.loser));
   }else CHECK(!p.ball_visible());
   CHECK(c.checksum()==before);p.advance();
  }
  CHECK(p.done()&&p.phase()==RoundPresentation::DONE);CHECK(!p.ball_visible());p.advance();CHECK(p.age==p.duration());
  if(expected)CHECK((seen&127)==127);p.reset();CHECK(p.age==0&&!p.recall&&p.loser==-1);
 }
 {CombatCore c;c.reset(0,1);c.outcome=1;RoundPresentation p;p.begin(c,false);p.age=111;p.skip();CHECK(!p.done());p.age=112;p.skip();CHECK(p.done());}
 // Timeout loss still recalls the losing fighter; a draw never selects a loser.
 {CombatCore c;c.reset(0,1);c.outcome=2;c.remaining=0;RoundPresentation p;p.begin(c,false);CHECK(p.recall&&p.loser==0&&c.f[0].hp>0);}
 std::printf("PASS %d round-presentation checks; all fighters, outcomes, stage edges, practice, skip and checksum isolation.\n",checks);
}
