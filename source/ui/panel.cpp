#include "panel.h"


#include "game/constants.h"


void wis::ui::Panel::apply()
{
  const auto& m = transform_.model_matrix();

  auto tl = glm::vec3{m * glm::vec4{size_.x * -0.5f, 0.0f, size_.y * -0.5f, 1.0f}};
  auto tr = glm::vec3{m * glm::vec4{size_.x *  0.5f, 0.0f, size_.y * -0.5f, 1.0f}};
  auto br = glm::vec3{m * glm::vec4{size_.x *  0.5f, 0.0f, size_.y *  0.5f, 1.0f}};
  auto bl = glm::vec3{m * glm::vec4{size_.x * -0.5f, 0.0f, size_.y *  0.5f, 1.0f}};

  collision_quad_ = apeiron::engine::collision::Quad{tl, tr, br, bl};

  quad_.init(size_, apeiron::engine::Face::Up);
  quad_.transform() = transform_;
}


glm::vec3 wis::ui::Panel::as_world_position(float x, float y) const
{
  return glm::vec3{transform_.model_matrix() *
      glm::vec4{size_.x * -0.5f + x, 0.0f, size_.y * -0.5f + y, 1.0f}};
}


glm::vec3 wis::ui::Panel::as_world_position(const glm::vec2& position) const
{
  return as_world_position(position.x, position.y);
}


apeiron::engine::Transform wis::ui::Panel::as_world_transform(float x, float y) const
{
  const auto& panel_pos = transform_.position();
  const auto sprite_offset = glm::vec2{val::tile_size_ui(), val::tile_size_ui()} * 0.5f;

  x = panel_pos.x + size_.x * -0.5f + x + sprite_offset.x;
  float z = panel_pos.z + size_.y * -0.5f + y + sprite_offset.y;

  auto t = transform_;
  t.set_position(x, 0.0f, z);

  return t;
}


apeiron::engine::Transform wis::ui::Panel::as_world_transform(const glm::vec2& position) const
{
  return as_world_transform(position.x, position.y);
}
