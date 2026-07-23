#ifndef WIS_UI_WIDGET_H
#define WIS_UI_WIDGET_H


#include <cstdint>
#include <glm/glm.hpp>
#include "apeiron/engine/collision.h"
#include "game/constants.h"


namespace wis::ui {


struct Widget
{
  Widget() = default;
  Widget(std::uint16_t mesh_index,
      float x,
      float y,
      float w = val::tile_size_ui(),
      float h = val::tile_size_ui())
      :
      mesh_index{mesh_index},
      position{x, y},
      size{w, h},
      rect{x - 0.5f * w, y - 0.5f * h, w, h} {}

  std::uint32_t mesh_index = 0;
  glm::vec2 position = glm::vec2{0.0f};
  glm::vec2 size = glm::vec2{0.0f};
  apeiron::engine::collision::Rect<float> rect = {};
};


}  // namespace wis::ui


#endif  // WIS_UI_WIDGET_H
