/*
The ingame state. This contains all of the game logic & game rendering code.
*/

#include "snake.hpp"

using namespace nwge;

Ingame::Ingame() {}

bool Ingame::preload() {
  return true;
}

bool Ingame::init() {
  reset(); // ensure everything is reset
  return true;
}

void Ingame::loadHighscore() {
  if(!gC.store.exists("highscore")) {
    mHighscore = -1;
    return;
  }

  auto rw = gC.store.open("highscore");
  u32 score;
  rw.read(&score, sizeof(score), 1);
  mHighscore = score;
}

void Ingame::reset() {
  // re-seed the rng
  srand(time(nullptr));
  // ensure we are alive
  mAlive = true;
  // reset the score
  mScore = 0;
  // load high score
  loadHighscore();
  // and the movement timer
  mMoveTimer = 0.0f;
  // remove the snake's tail
  mSnakeBits.clear();
  // halt movement until user input
  mSnakeDir = Dir::NONE;
  // randomise snake position
  mSnakePos = {rand() % cBoardW, rand() % cBoardH};
  // place some food on the board
  putFood();
}

bool Ingame::lastBitAt(Dir dir) {
  // there cannot be any bits adjacent to the snake's head if there are no bits
  // in the first place
  if(mSnakeBits.size() < 1)
    return false;

  // calculate the relative position of the first snake bit
  auto lastBitRel = mSnakeBits[0] - mSnakePos;
  // compare it against the position
  switch(dir) {
  case Dir::UP:
    return lastBitRel == glm::ivec2{0, -1};
  case Dir::RIGHT:
    return lastBitRel == glm::ivec2{1, 0};
  case Dir::DOWN:
    return lastBitRel == glm::ivec2{0, 1};
  case Dir::LEFT:
    return lastBitRel == glm::ivec2{-1, 0};
  default:
    return true;
  }
}

bool Ingame::on(Event &evt) {
  switch(evt.type) {
  case Event::Type::KeyDown: // when a key is presesd
    switch(evt.data.key.sym) {
    case SDLK_UP: // on Up Arrow, try to move up
      // if there's a snake bit upwards adjacent to the snake head, do not move
      // up -- it'd be an instant game over
      if(!lastBitAt(Dir::UP))
        mSnakeDir = Dir::UP;
      break;
    case SDLK_RIGHT: // same as Up Arrow, just right instead of up
      if(!lastBitAt(Dir::RIGHT))
        mSnakeDir = Dir::RIGHT;
      break;
    case SDLK_DOWN:
      if(!lastBitAt(Dir::DOWN))
        mSnakeDir = Dir::DOWN;
      break;
    case SDLK_LEFT:
      if(!lastBitAt(Dir::LEFT))
        mSnakeDir = Dir::LEFT;
      break;
    case SDLK_ESCAPE: // on Escape, if not alive, return to main menu
      if(!mAlive)
        setState<Title>();
      break;
    case SDLK_SPACE: // on Space, if not alive, reset everything
      if(!mAlive)
        reset();
      break;
    }
    break;

  default: // ignore all other events
    break;
  }

  return true;
}

void Ingame::putFood() {
  // keep trying until we succeed
  while(true) {
    // place the food at a random point
    mFoodPos = {rand() % cBoardW, rand() % cBoardH};

    // food cannot be at the same place as the snake's head
    if(mFoodPos == mSnakePos)
      continue;

    // ensure food isn't anywhere on the snake's tail
    for(const auto &bit: mSnakeBits)
      if(mFoodPos == bit)
        continue;

    // success!
    break;
  }
}

using namespace std::string_literals;

void Ingame::die() {
  mAlive = false;

  if(mScore < mHighscore)
    return;

  auto rw = gC.store.create("highscore");
  rw.write(&mScore, sizeof(mScore), 1);
}

bool Ingame::tick(float delta) {
  // don't do anything when dead
  if(!mAlive)
    return true;

  // increase movement timer
  mMoveTimer += delta;
  // calculate movement interval -- the higher the score the lower it is
  float ival = SDL_max(cMoveIval - float(mScore)/100, 0.075);
  // wait until the movement timer reaches the interval
  if(mMoveTimer < ival)
    return true;
  // subtract the interval from the timer (in case the game lags, we will just
  // move on every tick until we catch up)
  mMoveTimer -= ival;

  // calculate the next position of the snake's head
  auto nextSnakePos = mSnakePos;
  switch(mSnakeDir) {
  case Dir::UP:
    nextSnakePos.y -= 1;
    break;
  case Dir::RIGHT:
    nextSnakePos.x += 1;
    break;
  case Dir::DOWN:
    nextSnakePos.y += 1;
    break;
  case Dir::LEFT:
    nextSnakePos.x -= 1;
    break;
  default: break;
  }

  // if the next position is out of bounds, die
  if(nextSnakePos.x < 0 || nextSnakePos.x >= cBoardW
  ||(nextSnakePos.y < 0 || nextSnakePos.y >= cBoardH)) {
    die();
    return true;
  }

  // if the next position is on a tail bit, die
  for(const auto &bit: mSnakeBits)
    if(nextSnakePos == bit) {
      die();
      return true;
    }

  bool shouldPutFood = false;
  // if the next position is on the food, award
  if(nextSnakePos == mFoodPos) {
    mScore++;
    // this will lengthen the snake's tail. more on it a bit futher down
    mSnakeBits.insert({0, 0});
    // put food AFTER we update the snake's position
    shouldPutFood = true;
  }

  // update the snake's tail for the next position. this was annoying as all
  // hell to figure out
  if(mSnakeBits.size() > 0)
    // first we shift all the bits backward by one.
    // in reference to line 173: the 0,0 we added at the end will be updated to
    // a proper value here
    for(size_t i = mSnakeBits.size()-1; i > 0; i--)
      mSnakeBits[i] = mSnakeBits[i-1];
  // and we update the first one to the snake's previous position
  mSnakeBits[0] = mSnakePos;

  // update the snake position to the next position
  mSnakePos = nextSnakePos;

  // put food after updating the snake's position, since putFood() uses
  // mSnakePos
  if(shouldPutFood)
    putFood();

  return true;
}

void Ingame::render() {
  render::clear({0.1, 0.1, 0.1}); // preemptive clear

  const auto &ar = gC.ar;

  // board background
  render::color({0, 0, 0});
  render::rect(ar.rect({{0, 0, 0}, {1, 1}}));

  glm::vec2 blockSz = glm::vec2{1.0f/cBoardW, 1.0f/cBoardH},
            snakePos = {blockSz.x * mSnakePos.x, blockSz.y * mSnakePos.y},
            foodPos = {blockSz.x * mFoodPos.x, blockSz.y * mFoodPos.y};

  // snake head
  render::color({1, 0, 0});
  render::rect(ar.rect({{snakePos, 0}, blockSz}));

  // snake tail
  render::color({0, 1, 0});
  for(const auto &bit: mSnakeBits) {
    glm::vec2 bitPos{blockSz.x * bit.x, blockSz.y * bit.y};
    render::rect(ar.rect({{bitPos, 0}, blockSz}));
  }

  // food
  render::color({1, 1, 0});
  render::rect(ar.rect({{foodPos, 0}, blockSz}));

  // format the score to a C string
  char score[100];
  snprintf(score, sizeof(score), "%u", mScore);
  render::color();

  // on game over
  if(!mAlive) {
    // notify user of game over
    gC.fonts.ui.put("Game Over!", ar.pos({0.1, 0.1}), 0.09);

    gC.fonts.ui.put("Score:", ar.pos({0.1, 0.2}), 0.04);
    render::color({0, 1, 0});
    gC.fonts.num.put(score, ar.pos({0.4, 0.2}), 0.04);

    render::color();
    if(mHighscore > 0) {
      char highscore[100];
      snprintf(highscore, sizeof(highscore), "%lld", mHighscore);
      gC.fonts.ui.put("Highscore:", ar.pos({0.1, 0.25}), 0.04);
      render::color({0, 1, 0});
      gC.fonts.num.put(highscore, ar.pos({0.4, 0.25}), 0.04);
    } else {
      gC.fonts.ui.put("NEW Highscore:", ar.pos({0.1, 0.25}), 0.04);
      render::color({0, 1, 0});
      gC.fonts.num.put(score, ar.pos({0.4, 0.25}), 0.04);
    }

    // the hints
    render::color();
    gC.fonts.ui.put("Press ESCAPE to return to main menu.", ar.pos({0.1, 0.4}), 0.04);
    gC.fonts.ui.put("Press SPACE to try again.", ar.pos({0.1, 0.45}), 0.04);
  } else {
    // show score at top of screen
    gC.fonts.ui.put("Score:", ar.pos({0.02, 0.01}), 0.04);
    render::color({0, 1, 0});
    gC.fonts.num.put(score, ar.pos({0.2, 0.01}), 0.04);
  }
}

Ingame::~Ingame() {}
