#ifndef WIS_CORE_PIXEL_RENDERER_H
#define WIS_CORE_PIXEL_RENDERER_H


#include <cstdint>
#include <glm/glm.hpp>

#include "apeiron/engine/entity.h"
#include "apeiron/opengl/meshset.h"
#include "apeiron/opengl/shader.h"

#include "core/palette.h"


namespace wis {


class Pixel_renderer final
{
public:
  void init(float pixel_size, std::uint32_t tile_size);
  void use();

  void preset_view(const glm::mat4& view) { view_ = view; }
  void preset_projection(const glm::mat4& projection) { projection_ = projection; }
  void set_view(const glm::mat4& view);
  void set_projection(const glm::mat4& projection);
  void set_view_projection();

  // Game constants
  void set_palette();
  void set_pixel_size(float pixel_size);
  void set_tile_size(std::uint32_t tile_size);

  // Generic variables
  void set_time(float time);
  void set_tile_position(const glm::uvec2& position);

  // Post process variables
  void enable_blending(bool blending = true);
  void enable_desaturation(bool desaturation = true);
  void enable_tile_tilt(bool tilt = true);
  void enable_pixel_displacement(bool displacement = true);
  void enable_breathe(bool breathe = true);
  void set_blending_alpha(float alpha);
  void set_desaturation_factor(float factor);
  void set_tile_tilt(float tilt);
  void set_pixel_displacement(float displacement);
  void set_breathe_amplitude(float amplitude);
  void set_breathe_speed(float speed);
  void set_breathe_phase(float phase);

  // Rendering
  void render(const apeiron::engine::Entity& entity, const apeiron::opengl::Meshset& meshset,
      std::uint32_t index);

  // Raycasting
  [[nodiscard]] glm::mat4 view_projection() { return view_projection_; }
  [[nodiscard]] glm::mat4 inverse_view_projection() { return glm::inverse(view_projection_); }

  // Stats
  std::uint32_t draw_calls() const { auto calls = draw_calls_; draw_calls_ = 0; return calls; }

private:
  apeiron::opengl::Shader shader_;
  glm::mat4 view_ = glm::mat4{0.0f};
  glm::mat4 projection_ = glm::mat4{0.0f};
  glm::mat4 view_projection_ = glm::mat4{0.0f};
  mutable std::uint32_t draw_calls_ = 0u;
};


}  // namespace wis


#endif  // WIS_CORE_PIXEL_RENDERER_H
