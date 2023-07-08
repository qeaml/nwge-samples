/*
The credits screen. This just display the license for the fonts used in this
sample application. The user may also scroll the license in case one wants to
read it.
*/

#include "snake.hpp"

using namespace nwge;

Credits::Credits() {}

bool Credits::preload() {
  gC.pack.nqString("credits", mCredits);
  return true;
}

bool Credits::init() {
  // again, if you've looked at snake.hpp you'll know why this is done
  mCreditsCSTR = mCredits.c_str();
  return true;
}

bool Credits::on(Event &evt) {
  switch(evt.type) {
  case Event::Type::KeyDown: // when a key is pressed
    if(mScroll < 0)
      mScroll = 0; // ensure scroll is valid

    switch(evt.data.key.sym) {
    case SDLK_ESCAPE: // on Escape, return to main menu
      setState<Title>();
      break;
    case SDLK_DOWN: // on Down Arrow, start scrolling downwards
      mScrollDir = 1;
      break;
    case SDLK_UP: // on Up Arrow, start scrolling upwards
      mScrollDir = -1;
      break;
    }
    break;
  case Event::Type::KeyUp: // when a key is relesased
    switch(evt.data.key.sym) {
    case SDLK_UP:    // on Up/Down Arrow, stop scrolling altogether
    case SDLK_DOWN:
      mScrollDir = 0;
      break;
    }
    break;
  default: // ignore all other events
    break;
  }
  return true;
}

// Scrolling speed multiplier
#define SCROLL_SPEED 0.7

bool Credits::tick(float delta) {
  // Scroll the text. The boundaries of 0.0-1.6 are chosed arbitraily based on
  // the font size.
  mScroll = SDL_clamp(mScroll + delta*mScrollDir*SCROLL_SPEED, 0.0, 3);
  return true;
}

void Credits::render() {
  render::clear({0, 0, 0}); // preemptive clar()

  // put the license text on screen, scrolled
  render::color();
  gC.fonts.code.put(mCreditsCSTR, {0.02, -mScroll}, 0.04);

  // put a background behind the hint
  render::color({0, 0, 0});
  render::rect({{0, 0.95, 0}, {1, 0.05}});

  // the hint
  render::color();
  gC.fonts.ui.put("Press ESCAPE to return.", {0.01, 0.96}, 0.03);
}

Credits::~Credits() {}
