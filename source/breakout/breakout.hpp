#pragma once

/*
breakout.hpp
----------
Breakout
*/

#include <nwge/engine.hpp>

namespace breakout {

#define comptime static constexpr

struct Shared {
  nwge::data::PackRef commonPack{"common"},
                      ownPack{"breakout"};
  const nwge::render::AspectRatio &ar = nwge::render::keepAspectRatio(4, 3);
  nwge::render::Font font;
};

extern Shared gShared;

class Intro: public nwge::State {
private:
  float mTimer = 0.0f;
  comptime unsigned cLogoCount = 2;
  unsigned mLogoNo = 0;
  nwge::render::Texture mLogos;

public:
  Intro();
  NWGE_STATE_METHODS(Intro);
};

class Title: public nwge::State {
private:
  nwge::render::Texture mTitle,
                        mBg;

  int mHover = -1;

  static constexpr int cBtnCnt = 2;
  static constexpr float
    cMenuX = 0.1,
    cMenuY = 0.4,
    cMenuW = 0.3,
    cMenuPad = 0.005,
    cMenuBtnH = 0.04;

  bool handleClick();

public:
  Title();
  NWGE_STATE_METHODS(Title);
};

class Ingame: public nwge::State {
private:
  nwge::render::Texture mBg, mTiles;
  bool mPaused = false,
       mGameover = false;

  /* Gameplay stuff */
  static constexpr unsigned
    cBlockCntX = 16, // must be multiple of 8 for efficient storage!
    cBlockCntY = 6,
    cBlockBatch = cBlockCntX/8;
  static constexpr float
    cBounceScore = 12.5,
    cPlayerW = 0.1,
    cPlayerH = (cPlayerW/2)*(4.0/3),
    cPlayerStartX = 0.5 - cPlayerW/2,
    cPlayerY = 0.9,
    cPlayerMinX = 0,
    cPlayerMaxX = 1-cPlayerW,
    cPlayerSpd = 0.6,
    cBallH = 0.05,
    cBallW = cBallH*(3.0/4),
    cBallStartX = 0.5-cBallW/2,
    cBallStartY = cPlayerY-cBallH,
    cBallSpd = 0.5,
    cBlockPad = 0.005,
    cBlocksX = cBlockPad,
    cBlocksY = 0.1,
    cBlockSlotX = (1.0-cBlockPad)/cBlockCntX,
    cBlockSlotY = cBlockSlotX/2 * (4.0/3),
    cBlockW = cBlockSlotX - cBlockPad,
    cBlockH = cBlockSlotY - cBlockPad,
    cBlockBatchW = cBlockSlotX*8;

  unsigned mScore = 0,
           mBounces = 0,
           mLives = 3;
  float mSlowBall = 1.0f;

  // each block is just a single bit
  u8 mBlocks[cBlockCntX/8*cBlockCntY];

  float mPlayerX = cPlayerStartX;
  int mPlayerVel = 0;
  glm::vec2 mBall = {cBallStartX, cBallStartY};
  glm::vec2 mBallVel = {0, 0};


  void resetBall();
  void reset();

  // utility functions to ease working the the heavily compressed blocks
  bool block(unsigned x, unsigned y);
  void block(unsigned x, unsigned y, bool val);

  /* Pause menu stuff */
  int mHover = -1;

  static constexpr int cBtnCnt = 3;
  static constexpr float
    cMenuW = 0.2,
    cMenuPad = 0.005,
    cMenuBtnH = 0.04,
    cMenuX = 0.5 - cMenuW/2,
    cMenuY = 0.5 - (cMenuPad*2+cMenuBtnH*cBtnCnt)/2;

  bool eventPause(nwge::Event &evt);
  bool tickPause();
  bool handlePauseClick();
  void renderPause();

public:
  Ingame();
  NWGE_STATE_METHODS(Ingame);
};

}
