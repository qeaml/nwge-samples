/*
Game entrypoint. The sole purpose of this is to call nwge::start with the
correct state and options.
*/

#include "snake.hpp"

int main(int, char**) {
  nwge::start<Init>();
  return 0;
}
