#include "snake.hpp"

using namespace nwge;

Commons gC;

void preloadCommons() {
  c::eprint("Loading common resources...\n");

  data::addDataDir("data");
  gC.pack
    .nqFont("ui", 32, gC.fonts.ui)
    .nqFont("code", 16, gC.fonts.code)
    .nqFont("num", 32, gC.fonts.num);
}

