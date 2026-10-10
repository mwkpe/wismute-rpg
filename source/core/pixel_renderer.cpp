#include "pixel_renderer.h"


#include <format>
#include "core/constants.h"


void wis::Pixel_renderer::init(float pixel_size, std::uint32_t tile_size)
{
  shader_.load("shader/pixel.vert", "shader/pixel.frag", "shader/pixel.geom");
  shader_.use();

  // Init uniforms
  set_pixel_size(pixel_size);
  set_tile_size(tile_size);

  enable_color_blending(false);
  enable_color_desaturation(false);
  enable_tile_tilt(false);
  enable_tile_breathing(false);
  enable_pixel_displacement(false);

  set_color_index(0);

  set_color_blending_alpha(0.0f);
  set_color_desaturation_strength(0.0f);
  set_tile_tilt_amplitude(0.0f);
  set_tile_breathing(0.0f, 0.0f, 0.0f);
  set_pixel_displacement_strength(0.0f);
}


void wis::Pixel_renderer::use()
{
  shader_.use();
}


void wis::Pixel_renderer::set_view(const glm::mat4& view)
{
  shader_.set_uniform("view", view);
}


void wis::Pixel_renderer::set_projection(const glm::mat4& projection)
{
  shader_.set_uniform("projection", projection);
}


void wis::Pixel_renderer::set_view_projection()
{
  view_projection_ = projection_ * view_;
  shader_.set_uniform("view_projection", view_projection_);
}


void wis::Pixel_renderer::set_palette(std::span<const glm::vec4> palette)
{
  if (palette.size() != cval::palette_size) {
    return;
  }

  auto index = 0;

  for (const auto& color : palette) {
    shader_.set_uniform(std::format("colors[{}]", index++).c_str(), color);
  }
}


void wis::Pixel_renderer::set_pixel_size(float pixel_size)
{
  shader_.set_uniform("pixel_size", pixel_size);
}


void wis::Pixel_renderer::set_tile_size(std::uint32_t tile_size)
{
  shader_.set_uniform("tile_size", tile_size);
}


void wis::Pixel_renderer::set_time(float time)
{
  shader_.set_uniform("time", time);
}


void wis::Pixel_renderer::set_tile_position(const glm::uvec2& position)
{
  shader_.set_uniform("tile_position", position);
}


void wis::Pixel_renderer::set_color_index(std::uint32_t index)
{
  shader_.set_uniform("color_index", index);
}


void wis::Pixel_renderer::enable_color_blending(bool enable)
{
  shader_.set_uniform("color_blending_enabled", enable);
}


void wis::Pixel_renderer::enable_color_desaturation(bool enable)
{
  shader_.set_uniform("color_desaturation_enabled", enable);
}


void wis::Pixel_renderer::enable_tile_tilt(bool enable)
{
  shader_.set_uniform("tile_tilt_enabled", enable);
}


void wis::Pixel_renderer::enable_tile_breathing(bool enable)
{
  shader_.set_uniform("tile_breathing_enabled", enable);
}


void wis::Pixel_renderer::enable_pixel_displacement(bool enable)
{
  shader_.set_uniform("pixel_displacement_enabled", enable);
}


void wis::Pixel_renderer::set_color_blending_alpha(float alpha)
{
  shader_.set_uniform("color_blending_alpha", alpha);
}


void wis::Pixel_renderer::set_color_desaturation_strength(float strength)
{
  shader_.set_uniform("desaturation_strength", strength);
}


void wis::Pixel_renderer::set_tile_tilt_amplitude(float amplitude)
{
  shader_.set_uniform("tile_tilt_amplitude", amplitude);
}


void wis::Pixel_renderer::set_tile_breathing(float amplitude, float frequency, float phase)
{
  shader_.set_uniform("tile_breathing_amplitude", amplitude);
  shader_.set_uniform("tile_breathing_frequency", frequency);
  shader_.set_uniform("tile_breathing_phase", phase);
}


void wis::Pixel_renderer::set_pixel_displacement_strength(float strength)
{
  shader_.set_uniform("pixel_displacement_strength", strength);
}


void wis::Pixel_renderer::render(const apeiron::engine::Entity& entity,
    const apeiron::opengl::Meshset& meshset, std::uint32_t index)
{
  shader_.set_uniform("model", entity.transform().model_matrix());
  meshset.render_points(index);

  draw_calls_++;
}


void wis::Pixel_renderer::render(const apeiron::engine::Entity& entity,
    const apeiron::opengl::Meshset& meshset, std::uint32_t index, std::uint32_t color_index)
{
  shader_.set_uniform("model", entity.transform().model_matrix());
  set_color_index(color_index);
  meshset.render_points(index);
  set_color_index(0);

  draw_calls_++;
}
