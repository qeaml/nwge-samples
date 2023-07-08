#include "snake.hpp"

using namespace nwge;

Commons gC;

void preloadCommons() {
  c::eprint("Loading common resources...");

  data::addDataDir("data");
  gC.pack
    .nqFont("ui", 100, gC.fonts.ui)
    .nqFont("code", 100, gC.fonts.code)
    .nqFont("num", 100, gC.fonts.num);
}
