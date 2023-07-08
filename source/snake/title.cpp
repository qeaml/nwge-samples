/*
The Title screen/main menu.

Mostly made up of UI code.
*/

#include "snake.hpp"

using namespace nwge; // to save us some characters

Title::Title() {}

bool Title::preload() {
  /*
  Enqueue the logo texture to be loaded. We provide the data system with a
  filename for the texture and a reference to a texture object to load the
  texture into. Keep in mind that texture objects, and all other GPU objects,
  have a default constructor for an invalid object. This will initialize the
  texture to a new object and upload the texture data to it.
  */
  mPack.nqTexture("logo.png", mLogo);
  return true;
}

bool Title::init() { return true; }

// menu option names
static const char *const gOptNames[] = {
  "Start", "Credits", "Quit"
};

// amount of menu options
constexpr static u32 gOptCount = sizeof gOptNames / sizeof(char*);

bool Title::on(Event &evt) { // quality UI code
  switch(evt.type) {
  case Event::Type::KeyDown: // when a key is pressed
    switch(evt.data.key.sym) {
    // Handle menu navigation via up/down/left/right arrows.
    case SDLK_UP:
      if(mSelection == 0)
        mSelection = gOptCount;
      mSelection--;
      break;
    case SDLK_DOWN:
      mSelection++;
      if(mSelection >= gOptCount)
        mSelection = 0;
      break;

    // Handle if an option is selected
    case SDLK_SPACE:
    case SDLK_RETURN:
      switch(mSelection) {
      case 0:
        setState<Ingame>();
        break;
      case 1:
        setState<Credits>();
        break;
      case 2:
        return false;
      }
      break;
    }
    break;
  default: // ignore all other events
    break;
  }

  return true;
}

bool Title::tick(float) {
  return true;
}

// Some constants for the menu layout
#define BG_X 0.0
#define BG_W 0.3
#define LOGO_Y 0.3
#define LOGO_H 0.12
#define OPT_Y 0.5
#define OPT_H 0.05
#define TEXT_OFF_X 0.02
#define TEXT_OFF_Y 0.01
#define TEXT_H (OPT_H)

void Title::render() {
  render::clear(); // preemptively clear the screen
  const auto &ar = gC.ar;

  // background of the menu
  render::color({0, 0, 0});
  render::rect(ar.rect({{BG_X, 0, 0}, {BG_W, 1}}));

  // logo
  render::color();
  render::rect(ar.rect({{BG_X, LOGO_Y, 0.1}, {BG_W, LOGO_H}}), mLogo);

  // background of the selected option
  render::color({0, 1, 0});
  render::rect(
    ar.rect({{BG_X, OPT_Y + mSelection*OPT_H, 0}, {BG_W, OPT_H+2*TEXT_OFF_Y}}));

  // option text
  render::color();
  for(u32 i = 0; i < gOptCount; i++) {
    gC.fonts.ui.put(gOptNames[i],
      ar.pos({BG_X+TEXT_OFF_X, OPT_Y + i*OPT_H + TEXT_OFF_Y}), TEXT_H);
  }

  // little disclaimer at bottom left
  render::color();
  gC.fonts.ui.put("Powered by nwge!",
    ar.pos({TEXT_OFF_X, 1-OPT_H-TEXT_OFF_Y}), TEXT_H);
}

Title::~Title() {}
