#pragma once

/*
pong.hpp
----------
Pong
*/

#include <nwge/engine.hpp>

class Game: public nwge::State {
private:
  nwge::data::PackRef mCommonPack{"common"};
  nwge::data::PackRef mGamePack{"pong"};

  nwge::render::Texture mLogo;
  nwge::render::Font mFont;

  float mTimer = 0.0f, mOscOff = 0.0f;

  bool mBegun = false,
       mActive = false;

  glm::vec2 mBallPos;
  glm::vec2 mBallDir;
  float mPlayerPos[2];
  int mPlayerMvm[2];
  unsigned mScore[2];

  float mSlowTimer = 0.0f;

  bool mPlayerTwoWins = false;
  bool mHardMode = false;

  const nwge::render::AspectRatio &mAR = nwge::render::keepAspectRatio(16, 9);
  constexpr static float
    cPaddleXL = 0.07,
    cPaddleXR = 0.9,
    cPaddleWidth = 0.03,
    cPaddleHeight = 0.25,
    cPaddleLL = cPaddleXL,
    cPaddleLR = cPaddleXL + cPaddleWidth,
    cPaddleRL = cPaddleXR,
    cPaddleRR = cPaddleXR + cPaddleWidth,
    cBallWidth = 0.01 * (9.f/16.f),
    cBallHeight = 0.01,
    cPaddleFastSpd = 0.6,
    cPaddleSlowSpd = 0.5,
    cBallFastSpd = 0.6,
    cBallSlowSpd = 0.45;

  constexpr static int cWinScore = 8;

  constexpr static glm::vec3
    cPlayerColorL = {.7f, .5f, .3f},
    cPlayerColorR = {.3f, .5f, .3f};

  void reset();

  void renderPaddles();

  bool onTitle(nwge::Event &evt);
  bool onGame(nwge::Event &evt);
  bool onSummary(nwge::Event &evt);

  bool tickTitle(float delta);
  bool tickGame(float delta);
  bool tickSummary(float delta);

  void renderTitle();
  void renderGame();
  void renderSummary();

public:
  Game();
  NWGE_STATE_METHODS(Game);
};
