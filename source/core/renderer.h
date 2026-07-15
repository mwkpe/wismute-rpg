#ifndef WIS_CORE_RENDERER_H
#define WIS_CORE_RENDERER_H


#include <cstdint>
#include <glm/glm.hpp>

#include "apeiron/engine/entity.h"
#include "apeiron/opengl/meshset.h"
#include "apeiron/opengl/shader.h"


namespace wis {


class Renderer final
{
public:
  void init();
  void use();

  // OpenGL
  static void gl_init();

  static void set_gl_viewport(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
  static void set_gl_frame_buffer(std::int32_t id);
  static void set_gl_wireframe(bool wireframe);
  static void set_gl_depth_test(bool depth_test);
  static void set_gl_blend(bool blend);
  static void set_gl_color_mask(bool r, bool g, bool b, bool a);

  static void gl_clear();
  static void gl_clear(float r, float g, float b);
  static void gl_clear(const glm::vec3& color);
  static void gl_clear(const glm::vec4& color);
  static void gl_clear_depth_buffer();

  // Uniforms
  void use_world_space();
  void use_screen_space();
  void preset_view(const glm::mat4& view) { view_ = view; }
  void preset_projection(const glm::mat4& projection) { projection_ = projection; }
  void set_projection(const glm::mat4& projection);
  void set_view_projection();

  void use_vertex_color_shading();
  void use_color_shading();
  void use_texture_shading();
  void set_colorize(bool colorize);
  void set_invert_color(bool invert);
  void set_desaturate(bool desaturate);
  void set_desaturation_factor(float factor);

  // Rendering
  void render(const apeiron::engine::Entity& entity);
  void render(const apeiron::engine::Entity& entity, const glm::vec4& color);
  void render(const apeiron::engine::Entity& entity, const apeiron::opengl::Meshset& meshset,
      std::uint32_t index);
  void render_screen(const apeiron::engine::Entity& entity);

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


#endif  // WIS_CORE_RENDERER_H
