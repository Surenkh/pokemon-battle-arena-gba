#include "pkmn_roster_navigation.h"
#include "pkmn_effect_animation.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace pkmn;
int main(){unsigned checks=0;
 for(int who=0;who<character_count;++who){
  assert(roster_pages==1&&roster_page(who)==0&&roster_page_count(who)==13);++checks;
  assert(roster_x(who)-22>=8&&roster_x(who)+22<=232&&roster_y(who)>=20&&roster_y(who)+26<=102);++checks;
  assert(std::strlen(characters[who].name)*4-1<=selection_card_width-4);++checks;
  int up=roster_up(who);assert(up>=0&&up<13&&up%5==who%5);++checks;
  if(roster_has_down(who)){assert(roster_up(who+roster_columns)==who);++checks;}
  for(int other=who+1;other<13;++other){assert(roster_y(who)!=roster_y(other)||roster_x(other)-roster_x(who)>=selection_card_width);++checks;}
  unsigned punch=0;const auto& c=characters[who];
  for(int age=-2;age<c.melee_total+10;++age){int frame=power_punch_frame(age,c.melee_start,c.melee_active);assert(frame>=-1&&frame<6);if(frame>=0)punch|=1u<<frame;++checks;}
  assert(punch==63);++checks;
 }
 for(int born=0;born<16;++born){unsigned frames=0;for(int t=born;t<born+80;++t){int frame=super_effect_frame(t,born);assert(frame>=0&&frame<8);frames|=1u<<frame;++checks;}assert(frames==255);++checks;}
 for(int training=0;training<2;++training)for(int row=1;row<=(training?4:3);++row){int y=selection_row_y(training,row);assert(y-6>=102&&y+5<selection_footer_y-4);++checks;}
 std::printf("PASS %u selection/power checks: one page, valid navigation, readable labels, bounded geometry, complete punch phases and eight-frame supers.\n",checks);
}
