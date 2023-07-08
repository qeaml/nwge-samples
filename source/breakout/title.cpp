#include "breakout.hpp"

using namespace nwge;

namespace breakout {

Title::Title() {}
Title::~Title() {}

bool Title::preload() {
  gShared.ownPack.nqTexture("title.png", mTitle)
                 .nqTexture("bg.png", mBg);
  return true;
}

bool Title::init() { return true; }

bool Title::on(Event &evt) {
  switch(evt.type) {
  case Event::Type::MouseMotion: {
    auto x = evt.data.motion.x,
         y = evt.data.motion.y;
    if(x < cMenuX || x > cMenuX+cMenuW
    ||(y < cMenuY+cMenuPad || y > cMenuY+cMenuPad+cBtnCnt*cMenuBtnH))
      mHover = -1;
    else
      mHover = (y - (cMenuY+cMenuPad)) / cMenuBtnH;
    break;
  }

  case Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_UP:
      if(mHover <= 0)
        mHover = cBtnCnt-1;
      else
        mHover--;
      break;
    case SDLK_DOWN:
      if(mHover >= cBtnCnt-1)
        mHover = 0;
      else
        mHover++;
      break;
    case SDLK_RETURN:
      return handleClick();
    }
    break;

  case Event::Type::MouseUp:
    if(evt.data.click.button == Event::Data::MouseClick::Button::Left)
      return handleClick();
    break;

  default:
    break;
  }
  return true;
}

bool Title::tick(float) {
  return true;
}

void Title::render() {
  render::clear({0, 0, 0});

  render::color();
  render::rect(gShared.ar.rect({{0, 0, 0}, {1, 1}}), mBg);
  render::rect(gShared.ar.rect({{0.1, 0.3, 0}, {0.3, 0.1}}), mTitle);

  render::color({0, 0, 0});
  render::rect(gShared.ar.rect({
    {cMenuX, cMenuY, 0},
    {cMenuW, cMenuPad*2 + cBtnCnt*cMenuBtnH}
  }));

  if(mHover >= 0) {
    render::color({0.2, 0.2, 0.3});
    render::rect(gShared.ar.rect({
      {cMenuX, cMenuY+cMenuPad+mHover*cMenuBtnH, 0},
      {cMenuW, cMenuBtnH}
    }));
  }

  render::color();
  gShared.font.put("Start", gShared.ar.pos(
    {cMenuX+cMenuPad, cMenuY+cMenuPad}
  ), cMenuBtnH);
  gShared.font.put("Quit", gShared.ar.pos(
    {cMenuX+cMenuPad, cMenuY+cMenuPad+cMenuBtnH}
  ), cMenuBtnH);
}

bool Title::handleClick() {
  switch(mHover) {
  case 0:
    setState<Ingame>();
    break;
  case 1:
    return false;
  }
  return true;
}

}
