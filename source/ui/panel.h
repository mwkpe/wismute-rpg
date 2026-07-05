#ifndef WIS_UI_PANEL_H
#define WIS_UI_PANEL_H


#include <glm/glm.hpp>
#include "apeiron/engine/collision.h"
#include "apeiron/engine/transform.h"


namespace wis::ui {


class Panel
{
public:
  void set_size(float w, float h) { size_ = glm::vec2{w, h}; }
  void set_size(const glm::vec2& size) { size_ = size; }
  void apply();
  void show() { is_visible_ = true; }
  void hide() { is_visible_ = false; }

  apeiron::engine::Transform& transform() { return transform_; }
  const apeiron::engine::Transform& transform() const { return transform_; }

  [[nodiscard]] const glm::vec2& size() const { return size_; }
  [[nodiscard]] const apeiron::engine::collision::Quad& collision_quad() const {
      return collision_quad_; }
  [[nodiscard]] bool is_visible() const { return is_visible_; }
  [[nodiscard]] glm::vec3 as_world_position(float x, float y, float height = 0.0f) const;
  [[nodiscard]] glm::vec3 as_world_position(const glm::vec2& pos, float height = 0.0f) const;

protected:
  glm::vec2 size_ = glm::vec2{1.0f};
  apeiron::engine::Transform transform_;
  apeiron::engine::collision::Quad collision_quad_ = {};
  bool is_visible_ = true;
};


}  // namespace wis::gui


#endif  // WIS_UI_PANEL_H
