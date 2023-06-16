#include "breakout.hpp"

int main(int, char**) {
  nwge::start<breakout::Intro>({
    .windowSize = {1024, 768},
    .appName = "Breakout",
  });

  return 0;
}
