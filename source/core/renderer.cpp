#include "renderer.h"


#include <glad/glad.h>


void wis::Renderer::init()
{
  shader_.load("shader/basic.vs", "shader/basic.fs");
  shader_.use();

  // Init uniforms
  use_world_space();
  set_colorize(false);
  set_invert_color(false);
  set_desaturate(false);
  set_desaturation_factor(1.0f);

  shader_.set_uniform("color_mode", 0xff);
  shader_.set_uniform("color", glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});
}


void wis::Renderer::use()
{
  shader_.use();
}


void wis::Renderer::gl_init()
{
  glCullFace(GL_BACK);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LESS);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}


void wis::Renderer::set_gl_viewport(std::int32_t x, std::int32_t y,
    std::int32_t w, std::int32_t h)
{
  glViewport(x, y, w, h);
}


void wis::Renderer::set_gl_frame_buffer(std::int32_t id)
{
  glBindFramebuffer(GL_FRAMEBUFFER, id);
}


void wis::Renderer::set_gl_wireframe(bool wireframe)
{
  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}


void wis::Renderer::set_gl_depth_test(bool depth_test)
{
  if (depth_test) {
    glEnable(GL_DEPTH_TEST);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }
}


void wis::Renderer::set_gl_blend(bool blend)
{
  if (blend) {
    glEnable(GL_BLEND);
  }
  else {
    glDisable(GL_BLEND);
  }
}


void wis::Renderer::set_gl_color_mask(bool r, bool g, bool b, bool a)
{
  glColorMask(r, g, b, a);
}


void wis::Renderer::gl_clear()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void wis::Renderer::gl_clear(float r, float g, float b)
{
  glClearColor(r, g, b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void wis::Renderer::gl_clear(const glm::vec3& color)
{
  glClearColor(color.r, color.g, color.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void wis::Renderer::gl_clear(const glm::vec4& color)
{
  glClearColor(color.r, color.g, color.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void wis::Renderer::gl_clear_depth_buffer()
{
  glClear(GL_DEPTH_BUFFER_BIT);
}


void wis::Renderer::use_world_space()
{
  shader_.set_uniform("render_mode", 0);
}


void wis::Renderer::use_screen_space()
{
  shader_.set_uniform("render_mode", 1);
}


void wis::Renderer::set_projection(const glm::mat4& projection)
{
  shader_.set_uniform("projection", projection);
}


void wis::Renderer::set_view_projection()
{
  view_projection_ = projection_ * view_;
  shader_.set_uniform("view_projection", view_projection_);
}


void wis::Renderer::use_vertex_color_shading()
{
  shader_.set_uniform("color_mode", 0);
}


void wis::Renderer::use_color_shading()
{
  shader_.set_uniform("color_mode", 1);
}


void wis::Renderer::use_texture_shading()
{
  shader_.set_uniform("color_mode", 2);
}


void wis::Renderer::set_colorize(bool colorize)
{
  shader_.set_uniform("colorize", colorize);
}


void wis::Renderer::set_invert_color(bool invert)
{
  shader_.set_uniform("invert_color", invert);
}


void wis::Renderer::set_desaturate(bool desaturate)
{
  shader_.set_uniform("desaturate", desaturate);
}


void wis::Renderer::set_desaturation_factor(float factor)
{
  shader_.set_uniform("desaturation_factor", factor);
}


void wis::Renderer::render(const apeiron::engine::Entity& entity)
{
  use_vertex_color_shading();
  shader_.set_uniform("model", entity.transform().model_matrix());
  entity.render();

  draw_calls_++;
}


void wis::Renderer::render(const apeiron::engine::Entity& entity, const glm::vec4& color)
{
  use_color_shading();
  shader_.set_uniform("model", entity.transform().model_matrix());
  shader_.set_uniform("color", color);
  entity.render();

  draw_calls_++;
}


void wis::Renderer::render(const apeiron::engine::Entity& entity,
    const apeiron::opengl::Meshset& meshset, std::uint32_t index)
{
  use_vertex_color_shading();
  shader_.set_uniform("model", entity.transform().model_matrix());
  meshset.render(index);
  draw_calls_++;
}


void wis::Renderer::render_screen(const apeiron::engine::Entity& entity)
{
  use_screen_space();

  shader_.set_uniform("translation", entity.transform().position());
  shader_.set_uniform("scale", entity.transform().scale());
  entity.render();

  use_world_space();
}
