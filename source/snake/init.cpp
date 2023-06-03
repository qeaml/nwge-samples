/*
The Init state. Its only purpose is to call the xxxCommons() methods.
*/

#include "snake.hpp"

Init::Init() {}

bool Init::preload() {
  preloadCommons();
  return true;
}

bool Init::init() { return true; }

bool Init::on(nwge::Event&) { return true; }

bool Init::tick(float) {
  nwge::setState<Title>();
  return true;
}

void Init::render() {}

Init::~Init() {}
