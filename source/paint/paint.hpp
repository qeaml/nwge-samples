#pragma once

/*
paint.hpp
----------
Definitions for the sample Paint app.
*/

#include <SDL2/SDL_render.h>
#include <glm/fwd.hpp>
#include <nwge/engine.hpp>
#include <unordered_map>

/*
Colors are stored in ARGB32
*/
class Paint: public nwge::State {
public:
  enum class Tool {
    Pencil,
    Eraser,
  };

private:
  constexpr static glm::u32 cToolbarWidth = 32;
  constexpr static int cEraserWidth = 10;

  glm::uvec2 mDrawingSz, mWindowSz;
  glm::vec2 mDrawingRenderSz;
  void updateDrawingSize();
  void resizeDrawing();

  SDL_Surface *mDrawing = nullptr;
  SDL_Renderer *mDrawingRndr = nullptr;
  nwge::render::Texture mDrawingTex;

  Tool mTool = Tool::Pencil;
  glm::vec2 mCursorPos;
  glm::u32 mDrawingClr = 0xFF000000;

  nwge::data::PackRef mPack{"paint"};
  nwge::render::Texture mToolbarTex;
  glm::vec2 mToolbarBtnTexCoord, mToolbarBtnRndSz;

  bool mIsDrawing = false;
  glm::uvec2 mStrokeFrom{0.0f, 0.0f};
  void processStroke(glm::uvec2 to);

public:
  Paint() = default;

  NWGE_STATE_METHODS(Paint);
};
