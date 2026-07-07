#ifndef WIS_UI_QUAD_H
#define WIS_UI_QUAD_H


#include "apeiron/engine/entity.h"
#include "apeiron/opengl/quad.h"


namespace wis {


class Quad final : public apeiron::engine::Entity
{
public:
  Quad() = default;

  void init(float w, float h, apeiron::engine::Face face) {
      quad_.init(w, h, face); size_ = {w, h}; }
  void init(const glm::vec2& size, apeiron::engine::Face face) {
      quad_.init(size, face); size_ = size; }

  glm::vec2 size() const { return size_; }
  void render() const override { quad_.render(); }

private:
  apeiron::opengl::Quad quad_;
  glm::vec2 size_ = {1.0f, 1.0f};
};


}  // namespace wis


#endif  // WIS_UI_QUAD_H
