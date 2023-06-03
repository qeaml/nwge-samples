#include "paint.hpp"
#include <SDL2/SDL_main.h>

int main(int, char**) {
  nwge::start<Paint>({
    .maxFPS = 60.0f,
  });
  return 0;
}
