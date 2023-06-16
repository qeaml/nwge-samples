#include "paint.hpp"
#include <SDL2/SDL_image.h>

using namespace nwge;

Paint::~Paint() {
  SDL_FreeSurface(mDrawing);
  IMG_Quit();
}

bool Paint::preload() {
  IMG_Init(IMG_INIT_PNG);
  data::addDataDir("data");
  mPack.nqTexture("toolbar.png", mToolbarTex);
  return true;
}

bool Paint::init() {
  updateDrawingSize();
  mDrawing = SDL_CreateRGBSurface(0,
    mDrawingSz.x, mDrawingSz.y,
    32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  if(mDrawing == nullptr) {
    c::eprint("could not allocate drawing surface: %s", SDL_GetError());
    return false;
  }

  SDL_Rect drawingRect{0, 0, int(mDrawingSz.x), int(mDrawingSz.y)};
  SDL_FillRect(mDrawing, &drawingRect, 0xFFFFFFFF);

  mDrawingRndr = SDL_CreateSoftwareRenderer(mDrawing);
  SDL_SetRenderDrawColor(mDrawingRndr, 255, 255, 255, 255);
  SDL_RenderClear(mDrawingRndr);
  mDrawingTex.replace(mDrawing);

  glm::u8 a = (mDrawingClr >> 24) & 0xFF,
          r = (mDrawingClr >> 16) & 0xFF,
          g = (mDrawingClr >> 8) & 0xFF,
          b = (mDrawingClr) & 0xFF;
  SDL_SetRenderDrawColor(mDrawingRndr, r, g, b, a);

  return true;
}

void Paint::updateDrawingSize() {
  mWindowSz = nwge::render::windowSize();
  mDrawingSz = mWindowSz;
  mDrawingSz.x -= cToolbarWidth;
  mDrawingRenderSz = {
    float(mDrawingSz.x)/mWindowSz.x,
    float(mDrawingSz.y)/mWindowSz.y,
  };

  mToolbarBtnTexCoord = {
    float(cToolbarWidth)/96,
    float(cToolbarWidth)/64
  };
  mToolbarBtnRndSz = {
    float(cToolbarWidth)/mWindowSz.x,
    float(cToolbarWidth)/mWindowSz.y,
  };
}

const std::unordered_map<SDL_Keycode, Paint::Tool> gToolKeys{
  {SDLK_b, Paint::Tool::Pencil},
  {SDLK_x, Paint::Tool::Eraser},
};

bool Paint::on(Event &evt) {
  switch(evt.type) {
  case Event::Type::KeyDown: {
    if(evt.data.key.sym == SDLK_s) {
      return IMG_SavePNG(mDrawing, "drawing.png") == 0;
    }

    auto search = gToolKeys.find(evt.data.key.sym);
    if(search != gToolKeys.end())
      mTool = search->second;
    break;
  }

  case Event::Type::MouseDown:
    if(evt.data.click.button != Event::Data::MouseClick::Button::Left)
      break;

    mIsDrawing = true;
    mStrokeFrom = {
      evt.data.click.x * mWindowSz.x,
      evt.data.click.y * mWindowSz.y,
    };
    break;

  case Event::Type::MouseMotion:
    mCursorPos = {evt.data.motion.x, evt.data.motion.y};

    if(!mIsDrawing)
      break;

    processStroke({
      evt.data.motion.x * mWindowSz.x,
      evt.data.motion.y * mWindowSz.y,
    });
    break;

  case Event::Type::MouseUp:
    mIsDrawing = false;
    break;

  default: break;
  }

  return true;
}

void Paint::processStroke(glm::uvec2 to) {
  switch(mTool) {
  case Tool::Pencil:
    SDL_RenderDrawLine(mDrawingRndr, mStrokeFrom.x, mStrokeFrom.y, to.x, to.y);
    break;
  case Tool::Eraser: {
    SDL_Rect eraseRect{
      int(to.x)-cEraserWidth, int(to.y)-cEraserWidth,
      2*cEraserWidth + 1, 2*cEraserWidth + 1
    };
    SDL_FillRect(mDrawing, &eraseRect, 0xFFFFFFFF);
    break;
  }
  }
  mStrokeFrom = to;
}

bool Paint::tick(float) { return true; }

void Paint::render() {
  render::clear();

  SDL_RenderPresent(mDrawingRndr);
  mDrawingTex.replace(mDrawing);
  render::rect({{0.0f, 0.0f, 0.0f}, mDrawingRenderSz}, mDrawingTex);

  render::rect(
    {{1.0f-mToolbarBtnRndSz.x, mToolbarBtnRndSz.y*0, 0.1f}, mToolbarBtnRndSz},
    mToolbarTex,
    {{0.0f, (mTool==Tool::Pencil) * mToolbarBtnTexCoord.y}, mToolbarBtnTexCoord});
  render::rect(
    {{1.0f-mToolbarBtnRndSz.x, mToolbarBtnRndSz.y*1, 0.1f}, mToolbarBtnRndSz},
    mToolbarTex,
    {{0.0f, (mTool==Tool::Eraser) * mToolbarBtnTexCoord.y}, mToolbarBtnTexCoord});

  render::rect(
    {{1.0f-mToolbarBtnRndSz.x, mToolbarBtnRndSz.y*0, 0.1f}, mToolbarBtnRndSz},
    mToolbarTex,
    {{mToolbarBtnTexCoord.x, mToolbarBtnTexCoord.y*0}, mToolbarBtnTexCoord});
  render::rect(
    {{1.0f-mToolbarBtnRndSz.x, mToolbarBtnRndSz.y*1, 0.1f}, mToolbarBtnRndSz},
    mToolbarTex,
    {{mToolbarBtnTexCoord.x, mToolbarBtnTexCoord.y*1}, mToolbarBtnTexCoord});

  render::rect(
    {{mCursorPos - glm::vec2{0, mToolbarBtnRndSz.y}, 0.5f}, mToolbarBtnRndSz},
    mToolbarTex,
    {{mToolbarBtnTexCoord.x, mToolbarBtnTexCoord.y*int(mTool)}, mToolbarBtnTexCoord});
}
