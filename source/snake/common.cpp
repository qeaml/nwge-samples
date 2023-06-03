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

void initCommons() {
  auto blankSfc = SDL_CreateRGBSurfaceWithFormat(0, 64, 64, 32, SDL_PIXELFORMAT_RGBA32);
  SDL_Rect sfcRect{0, 0, blankSfc->w, blankSfc->h};
  SDL_FillRect(blankSfc, &sfcRect, 0xFFFFFFFF);
  gC.tex.blank.replace(blankSfc);
  SDL_FreeSurface(blankSfc);
}
