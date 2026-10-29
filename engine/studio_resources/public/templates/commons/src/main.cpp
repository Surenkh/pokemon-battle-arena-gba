#include "bn_core.h"
#if __has_include("pkmn_app.h")
#include "pkmn_app.h"
int main(){bn::core::init();pkmn::run_app();}
#else
#include <neo_logs.h>

#include <bn_core.h>
#include <bn_camera_actions.h>

#include <neo_variables.h>

#include "game.h"

int main()
{
  bn::core::init();

  bn::camera_ptr camera = bn::camera_ptr::create(0, 0);
  neo::game game(camera);

  while (true)
  {
    game.run();
    bn::core::update();
  }
}

#endif
