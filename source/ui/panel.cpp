#include "panel.h"


void wis::ui::Panel::apply()
{
  const auto& m = transform_.model_matrix();

  auto tl = glm::vec3{m * glm::vec4{size_.x * -0.5f, 0.0f, size_.y * -0.5f, 1.0f}};
  auto tr = glm::vec3{m * glm::vec4{size_.x *  0.5f, 0.0f, size_.y * -0.5f, 1.0f}};
  auto br = glm::vec3{m * glm::vec4{size_.x *  0.5f, 0.0f, size_.y *  0.5f, 1.0f}};
  auto bl = glm::vec3{m * glm::vec4{size_.x * -0.5f, 0.0f, size_.y *  0.5f, 1.0f}};

  collision_quad_ = apeiron::engine::collision::Quad{tl, tr, br, bl};
}


glm::vec3 wis::ui::Panel::as_world_position(float x, float y, float height) const
{
  return glm::vec3{transform_.model_matrix() *
      glm::vec4{size_.x * -0.5f + x, height, size_.y * -0.5f + y, 1.0f}};
}


glm::vec3 wis::ui::Panel::as_world_position(const glm::vec2& pos, float height) const
{
  return as_world_position(pos.x, pos.y, height);
}
