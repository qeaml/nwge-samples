#include "breakout.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace nwge;

namespace breakout {

void Ingame::reset() {
  srand(time(nullptr));
  resetBall();

  mScore = 0;
  mPaused = false;
  mLives = 3;
  mGameover = false;

  mBall = {cBallStartX, cBallStartY};

  for(size_t i = 0; i < cBlockBatch * cBlockCntY; i++)
    mBlocks[i] = ~0;
}

void Ingame::resetBall() {
  mBounces = 0;
  mBallVel = { rand() <= 0 ? -1 : 1, -1 };
  mBall = {cBallStartX, cBallStartY};
  mPlayerX = cPlayerStartX;
}

bool Ingame::on(Event &evt) {
  if(mPaused)
    return eventPause(evt);

  switch(evt.type) {
  case Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_ESCAPE:
      if(mGameover)
        setState<Title>();
      else
        mPaused = true;
      break;
    case SDLK_SPACE:
      reset();
      break;
    case SDLK_LEFT:
      if(!mGameover)
        mPlayerVel = -1;
      break;
    case SDLK_RIGHT:
      if(!mGameover)
        mPlayerVel = 1;
      break;
    }
    break;
  case Event::Type::KeyUp:
    switch(evt.data.key.sym) {
    case SDLK_LEFT:
      if(mPlayerVel < 0)
        mPlayerVel = 0;
      break;
    case SDLK_RIGHT:
      if(mPlayerVel > 0)
        mPlayerVel = 0;
      break;
    }
    break;
  default:
    break;
  }

  return true;
}

bool Ingame::tick(float delta) {
  if(mPaused)
    return tickPause();

  if(mGameover)
    return true;

  mPlayerX += mPlayerVel * cPlayerSpd * delta;
  mPlayerX = SDL_clamp(mPlayerX, cPlayerMinX, cPlayerMaxX);

  glm::vec2 nextBall = mBall + mBallVel * cBallSpd * delta;

  bool move = true;
  if(nextBall.x < 0) {
    mBallVel.x *= -1;
    move = false;
  }
  if(nextBall.x > 1-cBallW) {
    mBallVel.x *= -1;
    move = false;
  }
  if(nextBall.y < 0) {
    mBallVel.y *= -1;
    move = false;
  }
  if(nextBall.y > 1-cBallH) {
    resetBall();
    mLives--;
    mGameover = mLives == 0;
    move = false;
  }
  if(nextBall.x >= mPlayerX && nextBall.x < mPlayerX+cPlayerW
  &&(nextBall.y >= cPlayerY-cBallH && nextBall.y < cPlayerY+cPlayerH)) {
    mBallVel.y *= -1;
    mBall.y = cPlayerY-cBallH;
    move = false;
  }
  if(nextBall.x >= cBlockPad && nextBall.x <= 1-cBlockPad
  &&(nextBall.y >= cBlocksY && nextBall.y <= cBlocksY+cBlockCntY*cBlockSlotY)) {
    for(unsigned y = 0; y < cBlockCntY; y++) {
    for(unsigned x = 0; x < cBlockCntX; x++) {
      if(!block(x, y))
        continue;

      float rx1 = cBlocksX + x*cBlockSlotX,
            rx2 = rx1 + cBlockW,
            ry1 = cBlocksY + y*cBlockSlotY,
            ry2 = ry1 + cBlockH,
            bx1 = mBall.x,
            bx2 = bx1 + cBallW,
            by1 = mBall.y,
            by2 = by1 + cBallH;

      if(bx1 < rx2 && bx2 > rx1 && by1 < ry2 && by2 > ry1) {
        mBallVel.y *= -1;
        mBall.y = ry2;
        block(x, y, false);
        move = false;
        mBounces++;
        mScore += (mBounces+1) * cBounceScore;
        break;
      }
    }
    }
  }

  if(move)
    mBall = nextBall;

  return true;
}

void Ingame::render() {
  render::clear({0, 0, 0});
  if(mGameover) {
    render::color({0.2, 0.2, 0.2});
  } else {
    render::color();
  }

  render::rect(gShared.ar.rect({{0, 0, 0}, {1, 1}}), mBg);

  for(size_t y = 0; y < cBlockCntY; y++) {
  #pragma unroll // will always loop twice, so might as well
  for(size_t b = 0; b < cBlockBatch; b++) {
    u8 blocks = mBlocks[y*cBlockBatch+b];

    #pragma unroll // will always loop 8 times, so might as well
    for(size_t off = 0; off < 8; off++)
      if(blocks & (1 << off))
        render::rect(gShared.ar.rect({
          {cBlocksX + b*cBlockBatchW + off*cBlockSlotX,
           cBlocksY + y*cBlockSlotY, 0},
          {cBlockW, cBlockH}
        }),
        mTiles,
        {{48.0/128, 0}, {48.0/128, 24.0/32}});
        // never nesters crying right now 🤓
  }
  }

  char buf[1024];
  snprintf(buf, sizeof(buf), "Lives: %u", mLives);
  gShared.font.put(buf, gShared.ar.pos({0.21, 0.01}));
  snprintf(buf, sizeof(buf), "Score: %06u", mScore);
  gShared.font.put(buf, gShared.ar.pos({0.01, 0.01}));

  render::color();
  if(mPaused)
    renderPause();

  if(mGameover) {
    render::color({0, 0, 0});
    render::rect(gShared.ar.rect({
      {0, 0.4, 0}, {1, 0.2}
    }));
    render::color();
    gShared.font.put("Game Over!", gShared.ar.pos({0.01, 0.41}));
    gShared.font.put(buf, gShared.ar.pos({0.01, 0.46}));
    gShared.font.put("Press ESCAPE to quit, or SPACE to retry.",
      gShared.ar.pos({0.01, 0.55}));

    return;
  }

  render::rect(gShared.ar.rect({
    {mBall, 0}, {cBallW, cBallH}}),
    mTiles,
    {{96.0/128, 0}, {16.0/128, 16.0/32}}
  );
  render::rect(gShared.ar.rect({
    {mPlayerX, cPlayerY, 0}, {cPlayerW, cPlayerH}}),
    mTiles,
    {{0, 0}, {48.0/128, 24.0/32}}
  );
}

bool Ingame::block(unsigned x, unsigned y) {
  size_t i = size_t(y) * cBlockCntX + x;
  // woah, so crazy!
  size_t byte = i >> 3; // i / 8
  u8 bit = i & 7,       // i % 8
     mask = 1 << bit;
  return (mBlocks[byte] & mask) == mask;
}

void Ingame::block(unsigned x, unsigned y, bool val) {
  size_t i = size_t(y) * cBlockCntX + x;
  size_t byte = i >> 3; // i / 8
  u8 bit = i & 7,       // i % 8
     retainBits = ~(1 << bit),
     replaceBit = (val & 1) << bit;
  mBlocks[byte] = (mBlocks[byte] & retainBits) | replaceBit;
}

bool Ingame::eventPause(Event &evt) {
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

  case Event::Type::MouseUp:
    if(evt.data.click.button == Event::Data::MouseClick::Button::Left)
      return handlePauseClick();
    break;

  case Event::Type::KeyDown:
    switch(evt.data.key.sym) {
    case SDLK_ESCAPE:
      mPaused = false;
      break;
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
      return handlePauseClick();
    }
    break;

  default:
    break;
  }

  return true;
}

bool Ingame::handlePauseClick() {
  switch(mHover) {
  case 0:
    mPaused = false;
    break;
  case 1:
    setState<Title>();
    break;
  case 2:
    return false;
  }
  return true;
}

bool Ingame::tickPause() {
  return true;
}

void Ingame::renderPause() {
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
  gShared.font.put("PAUSED", gShared.ar.pos(
    {cMenuX, cMenuY-2*cMenuPad-cMenuBtnH}
  ));
  gShared.font.put("Resume", gShared.ar.pos(
    {cMenuX+cMenuPad, cMenuY+cMenuPad}
  ));
  gShared.font.put("Main Menu", gShared.ar.pos(
    {cMenuX+cMenuPad, cMenuY+cMenuPad+cMenuBtnH}
  ));
  gShared.font.put("Quit", gShared.ar.pos(
    {cMenuX+cMenuPad, cMenuY+cMenuPad+2*cMenuBtnH}
  ));
}

bool Ingame::preload() {
  gShared.ownPack.nqTexture("bgmute.png", mBg)
                 .nqTexture("tiles.png", mTiles);
  return true;
}

bool Ingame::init() {
  reset();
  return true;
}

Ingame::Ingame() {}
Ingame::~Ingame() {}

}
