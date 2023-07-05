/*
Main Pong code. TODO: Comments
*/

#include "pong.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace nwge;

int main(int, char**) {
  nwge::start<Game>();
  return 0;
}

Game::Game() {}
Game::~Game() {}

bool Game::preload() {
  data::addDataDir("data");
  mCommonPack.nqFont("ui", 32, mFont);
  mGamePack.nqTexture("logo.png", mLogo);
  return true;
}

using namespace std::string_literals;

bool Game::init() {
  reset();
  console::registerCommand("g.hardmode", [this](auto&){
    mHardMode = !mHardMode;
    console::print("hard mode: %s", mHardMode ? "on" : "off");
    return true;
  });
  return true;
}

bool Game::on(Event &evt) {
  if(mBegun && mActive)
    return onGame(evt);
  else if(mBegun)
    return onSummary(evt);
  else
    return onTitle(evt);
}

bool Game::tick(float delta) {
  mTimer += delta;
  if(mBegun && mActive)
    return tickGame(delta);
  else if(mBegun)
    return tickSummary(delta);
  else
    return tickTitle(delta);
}

void Game::render() {
  render::clear({.2, .2, .2});
  if(mBegun && mActive)
    return renderGame();
  else if(mBegun)
    return renderSummary();
  else
    return renderTitle();
}

void Game::reset() {
  srand(time(NULL));
  mOscOff = float(rand())/RAND_MAX * M_PI;
  mTimer = 0.0f;
  mBegun = mActive = false;
  mBallPos = {0.5, 0.5};
  mBallDir = {0, 0};
  mPlayerPos[0] = mPlayerPos[1] = 0.5;
  mPlayerMvm[0] = mPlayerMvm[1] = 0;
  mScore[0] = mScore[1] = 0;
  mSlowTimer = !mHardMode;
}

void Game::renderPaddles() {
  render::color(cPlayerColorL);
  render::rect(mAR.rect({
    {cPaddleXL, mPlayerPos[0]-cPaddleHeight/2, 0},
    {cPaddleWidth, cPaddleHeight}}));
  render::rect(mAR.rect({{0.49, 0.1, 0}, {0.01, 0.8}}));
  render::color(cPlayerColorR);
  render::rect(mAR.rect({
    {cPaddleXR, mPlayerPos[1]-cPaddleHeight/2, 0},
    {cPaddleWidth, cPaddleHeight}}));
  render::rect(mAR.rect({{0.5, 0.1, 0}, {0.01, 0.8}}));
}

bool Game::onTitle(Event &evt) {
  switch(evt.type) {
  case Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_SPACE:
      mBegun = mActive = true;
      mPlayerPos[0] = mPlayerPos[1] = 0.5;

      mBallDir = {
        rand() & 1 ? -1 : 1,
        rand() & 1 ? -1 : 1
      };

      return true;
    case SDLK_ESCAPE:
      return false;
    }
    break;
  default:
    break;
  }
  return true;
}

bool Game::tickTitle(float) {
  mPlayerPos[0] = 0.5 + sinf(mTimer)/5;
  mPlayerPos[1] = 0.5 + sinf(mTimer+mOscOff)/5;
  return true;
}

void Game::renderTitle() {
  renderPaddles();

  render::color({0, 0, 0});
  render::rect(mAR.rect({{0, 0, 0}, {1, 0.3}}));

  render::color();
  render::rect(mAR.rect({{0.4, 0.05, 0}, {0.2, 0.1}}), mLogo);

  mFont.put("Press SPACE to begin.", mAR.pos({0.05, 0.2}));

  mFont.put("v1.1", mAR.pos({0.05, 0.9}));
}

bool Game::onGame(Event &evt) {
  switch(evt.type) {
  case nwge::Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_w:
      mPlayerMvm[0] = -1;
      break;
    case SDLK_s:
      mPlayerMvm[0] = 1;
      break;
    case SDLK_UP:
      mPlayerMvm[1] = -1;
      break;
    case SDLK_DOWN:
      mPlayerMvm[1] = 1;
      break;
    }
    break;
  case nwge::Event::Type::KeyUp:
    switch(evt.data.key.sym) {
    case SDLK_w:
      if(mPlayerMvm[0] < 0)
        mPlayerMvm[0] = 0;
      break;
    case SDLK_s:
      if(mPlayerMvm[0] > 0)
        mPlayerMvm[0] = 0;
      break;
    case SDLK_UP:
      if(mPlayerMvm[1] < 0)
        mPlayerMvm[1] = 0;
      break;
    case SDLK_DOWN:
      if(mPlayerMvm[1] > 0)
        mPlayerMvm[1] = 0;
      break;
    }
    break;
  default:
    break;
  }
  return true;
}

bool Game::tickGame(float delta) {
  float paddleSpd = mHardMode ? cPaddleSlowSpd : cPaddleFastSpd;
  mPlayerPos[0] = SDL_clamp(
    mPlayerPos[0] + mPlayerMvm[0]*delta*paddleSpd,
    cPaddleHeight/2,
    1 - cPaddleHeight/2);
  mPlayerPos[1] = SDL_clamp(
    mPlayerPos[1] + mPlayerMvm[1]*delta*paddleSpd,
    cPaddleHeight/2,
    1 - cPaddleHeight/2);

  float ballSpd = mHardMode ? cBallFastSpd : cBallSlowSpd;
  ballSpd -= mSlowTimer * ballSpd;
  mSlowTimer = SDL_max(0, mSlowTimer-delta);
  glm::vec2 nextBallPos = mBallPos + mBallDir*delta*ballSpd;

  float leftBallX = nextBallPos.x - cBallWidth/2,
        rightBallX = nextBallPos.x + cBallWidth/2,
        topBallY = nextBallPos.y - cBallHeight/2,
        btmBallY = nextBallPos.y + cBallHeight/2;

  if(leftBallX < 0) {
    mScore[1]++;
    nextBallPos = {0.5, 0.5};
    mSlowTimer = !mHardMode;
    mBallDir *= -1;
  }
  else if(rightBallX > 1) {
    mScore[0]++;
    nextBallPos = {0.5, 0.5};
    mSlowTimer = !mHardMode;
    mBallDir *= -1;
  }
  else if(topBallY < 0 || btmBallY > 1) {
    mBallDir.y *= -1;
  }
  else if(leftBallX < cPaddleLR && rightBallX > cPaddleLL
  &&(topBallY < mPlayerPos[0]+cPaddleHeight/2)
  &&(btmBallY > mPlayerPos[0]-cPaddleHeight/2)) {
    mBallDir.x = -mBallDir.x;
    nextBallPos.x = cPaddleXL + cBallWidth + cPaddleWidth;
  }
  else if(rightBallX > cPaddleRL && leftBallX < cPaddleRR
  &&(topBallY < mPlayerPos[1]+cPaddleHeight/2)
  &&(btmBallY > mPlayerPos[1]-cPaddleHeight/2)) {
    mBallDir.x = -mBallDir.x;
    nextBallPos.x = cPaddleXR - cBallWidth;
  }

  if(mScore[0] >= cWinScore) {
    mActive = false;
    mPlayerTwoWins = false;
  } else if(mScore[1] >= cWinScore) {
    mActive = false;
    mPlayerTwoWins = true;
  }

  mBallPos = nextBallPos;

  return true;
}

void Game::renderGame() {
  renderPaddles();

  render::color();
  render::rect(mAR.rect({{mBallPos-cBallWidth/2, 0}, {cBallWidth, cBallHeight}}));

  char score[20];
  snprintf(score, sizeof(score), "%u", mScore[0]);
  mFont.put(score, mAR.pos({cPaddleLL, 0.05}));

  snprintf(score, sizeof(score), "%u", mScore[1]);
  mFont.put(score, mAR.pos({cPaddleRL, 0.05}));
}

bool Game::onSummary(Event &evt) {
  switch(evt.type) {
  case nwge::Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_SPACE:
      reset();
      break;
    }
    break;
  default:
    break;
  }
  return true;
}

bool Game::tickSummary(float) {
  return true;
}

void Game::renderSummary() {
  renderPaddles();

  render::color({0, 0, 0});
  render::rect(mAR.rect({{0, 0, 0}, {1, 0.25}}));

  render::color();
  mFont.put(
    mPlayerTwoWins ? "Player 2 wins!" : "Player 1 wins!",
    mAR.pos({0.05, 0.05})
  );
  mFont.put("Press SPACE to reset.", mAR.pos({0.05, 0.15}));
}
