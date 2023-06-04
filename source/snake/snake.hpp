#pragma once

/*
Sample Snake app
----------------
This is the header file that defines all the common resources shared between
states as well as all the states themselves.
*/

// Main include file for the engine. It also includes SDL_main.h for us as well
// as define some utility macros that we don't end up using here.
#include <nwge/engine.hpp>

// Common resources shared between states
struct Commons {
  // The bundle used by the game. 'PackRef' is a term lifted from an earlier
  // version of bundles, kept to differentiate between the bundle (which is the
  // bundle file itself) and the pack (which is just a named reference to a
  // bundle)
  nwge::data::PackRef pack{"snake"};

  nwge::data::Store store{};

  // aspect ratio object to ensure nothing is stretched out in weird ways
  const nwge::render::AspectRatio &ar =
    nwge::render::keepAspectRatio(1, 1);

  // Common font
  struct Fonts {
    // Note: The fonts should be the last member of any structure, including the
    //       parent structure. The Font::replace method, which bakes the basic
    //       printable ASCII glyphs into a master texture for the font, performs
    //       out-of-bounds array access causing other members to be overwritten.
    //       It is not currently known why this happens (the array containing
    //       character information is even padded with an extra dummy entry to
    //       counteract this, but to no avail)
    nwge::render::Font
      ui, num, code;
  } fonts;
};

// The global Commons instance, referred to as gC hereafter.
extern Commons gC;

// Functions related to the lifetime of gC
void preloadCommons();

// Minimal initialization state. The only purpose of this state is to call
// preloadCommons() and initCommons(), then switch to the Title state.
class Init: public nwge::State {
public:
  Init();
  NWGE_STATE_METHODS(Init);
};

// The tile screen/main menu state.
class Title: public nwge::State {
private:
  u32 mSelection = 0;
  // A texture owned by this state. It is loaded in State::preload() and is
  // unloaded by the destructor.
  nwge::render::Texture mLogo;

public:
  Title();
  // Expands to the basic methods required to be present on all State subclasses
  NWGE_STATE_METHODS(Title);
};

// Credits/license screen state.
class Credits: public nwge::State {
private:
  // The data system loads strings into std::string objects, but the render
  // system uses C strings for fonts.
  std::string mCredits;
  const char *mCreditsCSTR;
  // How far down have we scrolled so far?
  float mScroll = 0;
  // Which direction are we currently scrolling?
  // 1 = down; 0 = not scrolling; -1 = up;
  int mScrollDir = 0;

public:
  Credits();
  NWGE_STATE_METHODS(Credits);
};

// The ingame state, the meat & potatoes of the game.
class Ingame: public nwge::State {
private:
  // Simple direction enumeration
  enum class Dir {
    UP, RIGHT, DOWN, LEFT, NONE
  };

  // The extents of the game board
  static constexpr i32 cBoardW = 10,
                       cBoardH = 10;

  // player's score
  u32 mScore = 0;
  i64 mHighscore = 0;
  glm::ivec2 mSnakePos, // position of the snake's head
             mFoodPos;  // position of the food
  // direction of snake movement
  Dir mSnakeDir = Dir::NONE;
  // The bits following the snake head around. This is a slice of absolute
  // game board coordinates. You can learn more about how this works in the
  // implementation of Ingame::tick (in ingame.cpp)
  nwge::c::Slice<glm::ivec2> mSnakeBits;
  // true if the game has not yet ended
  bool mAlive = true;

  // Base movement interval
  constexpr static float cMoveIval = 0.5f;
  // Timer limiting how fast the snake moves
  float mMoveTimer = 0.0f;

  // determines if there is a snake tail bit at the provided direction relative
  // to the snake's head
  bool lastBitAt(Dir dir);
  void die();
  // reset all the game variables
  void reset();
  // generate a new food position
  void putFood();
  void loadHighscore();

public:
  Ingame();
  NWGE_STATE_METHODS(Ingame);
};
