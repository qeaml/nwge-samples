#include "breakout.hpp"

using namespace nwge;

namespace breakout {

Shared gShared;

Intro::Intro() {}
Intro::~Intro() {}

bool Intro::preload() {
  data::addDataDir("data");
  gShared.commonPack.nqFont("ui", 100, gShared.font);
  gShared.ownPack.nqTexture("logos.png", mLogos);
  return true;
}

bool Intro::init() { return true; }
bool Intro::on(Event&) { return true; }

bool Intro::tick(float delta) {
  mTimer += delta;
  while(mTimer >= 1) {
    mTimer -= 1;
    mLogoNo++;

    if(mLogoNo >= cLogoCount) {
      setState<Title>();
      break;
    }
  }
  return true;
}

void Intro::render() {
  render::clear({0, 0, 0});
  float shade = fmin(-4*abs(mTimer-0.5)+2, 1);
  render::color({shade, shade, shade});
  render::rect(
    gShared.ar.rect({{0.4, 0.4, 0}, {0.2, 0.2}}),
    mLogos,
    {{float(mLogoNo)/cLogoCount, 0}, {1.0f/cLogoCount, 1}}
  );
}

}
