#include "game_ui.h"


#include "app/error.h"
#include "engine/palette.h"
#include "game/constants.h"


wis::Game_ui::Game_ui(entt::registry& registry,
    entt::dispatcher& dispatcher,
    const App_data& app_data,
    Game_data& game_data,
    const Atlas& atlas)
    :
    registry_{registry},
    dispatcher_{dispatcher},
    app_data_{app_data},
    game_data_{game_data},
    atlas_{atlas},
    camera_{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 24.0f, 0.0f}}
{
}


void wis::Game_ui::init()
{
  // Viewport and GL init is done in stage class

  renderer_.init();
  pixel_renderer_.init(0.1f, 16);

  lattice_.init({20, 10}, tile_size());
  auto field_size = lattice_.field_size();

  grid_.init(field_size, lattice_.size(), Palette::colors[47]);
  grid_.transform().set_position(0.0f, 0.001f, 0.0f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);

  setup_view();
  set_screen_limits();  // Needs view initialized

  panel_.set_size(1.6f, 12.8f);
  panel_.transform().set_position(left_ + 0.9f, 0.0f, 0.0f)
      .set_rotation_deg(0.0f, 0.0f, -25.0f)
      .set_rotation_pivot(apeiron::engine::Axis::Z, tile_size() * -0.5f, 0.0f, 0.0f);
  panel_.apply();

  panel_quad_.init(panel_.size(), apeiron::engine::Face::Up);
  panel_quad_.transform() = panel_.transform();

  dispatcher_.sink<event::Action_selected>().connect<&Game_ui::on_action_selected>(this);
}


void wis::Game_ui::update()
{
}


void wis::Game_ui::render()
{
  // Viewport and buffer selected in stage class

  Renderer::gl_clear_depth_buffer();
  setup_view();
  render_debug();
}


bool wis::Game_ui::handle_event([[maybe_unused]] const apeiron::engine::Mouse_button_down_event& event)
{
  return false;
}


bool wis::Game_ui::handle_event([[maybe_unused]] const apeiron::engine::Mouse_button_up_event& event)
{
  return false;
}


bool wis::Game_ui::handle_event(const apeiron::engine::Mouse_motion_event& event)
{
  auto& cursor = game_data_.cursor.ui;

  if (auto point = screen_point(event.x, event.y); point) {
    cursor.screen_position = *point;
  }
  else {
    cursor.screen_position = glm::vec3{0.0f};
  }

  cursor.panel_position = glm::vec2{-1.0f, -1.0f};
  cursor.on_panel = false;

  if (auto point = panel_point(event.x, event.y, panel_.collision_quad()); point) {
    cursor.panel_position = *point;
    cursor.on_panel = true;
  }

  return false;
}


void wis::Game_ui::on_action_selected([[maybe_unused]] const event::Action_selected& event)
{
}


void wis::Game_ui::setup_view()
{
  glm::mat4 projection = glm::perspective(glm::radians(game_data_.camera.fov),
      app_data_.window.aspect_ratio, 1.0f, 100.0f);

  pixel_renderer_.use();
  pixel_renderer_.preset_projection(projection);
  pixel_renderer_.preset_view(camera_.perspective_view());
  pixel_renderer_.set_view_projection();

  renderer_.use();
  renderer_.preset_projection(projection);
  renderer_.preset_view(camera_.perspective_view());
  renderer_.set_view_projection();
}


void wis::Game_ui::set_screen_limits()
{
  auto logical_left = 0.0f;
  auto logical_right = static_cast<float>(app_data_.window.logical_width);
  auto logical_top = 0.0f;
  auto logical_bottom = static_cast<float>(app_data_.window.logical_height);

  if (auto point = screen_point(logical_left, logical_top); point) {
    left_ = point->x;
    top_ = point->z;
  }
  else {
    throw wis::Error::format("Error calculating screen limits at {}, {}", logical_left,
        logical_top);
  }

  if (auto point = screen_point(logical_right, logical_bottom); point) {
    right_ = point->x;
    bottom_ = point->z;
  }
  else {
    throw wis::Error::format("Error calculating screen limits at {}, {}", logical_right,
        logical_bottom);
  }
}


void wis::Game_ui::render_panel()
{
  //renderer_.use();
  //Renderer::set_gl_depth_test(false);

  //Renderer::set_gl_depth_test(true);
}


void wis::Game_ui::render_debug()
{
  renderer_.use();

  if (app_data_.debug.show_ui_grid) {
    renderer_.render(grid_);
  }

  renderer_.render(panel_quad_, Palette::colors[22]);
}


std::optional<glm::vec3> wis::Game_ui::screen_point(float screen_x, float screen_y)
{
  using namespace apeiron::engine::collision;

  float nx = screen_x / static_cast<float>(app_data_.window.logical_width) * 2.0f - 1.0f;
  float ny = (screen_y / static_cast<float>(app_data_.window.logical_height) * 2.0f - 1.0f) * -1.0f;

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}


std::optional<glm::vec2> wis::Game_ui::panel_point(float screen_x, float screen_y,
    const apeiron::engine::collision::Quad& panel)
{
  using namespace apeiron::engine::collision;

  float nx = screen_x / static_cast<float>(app_data_.window.logical_width) * 2.0f - 1.0f;
  float ny = (screen_y / static_cast<float>(app_data_.window.logical_height) * 2.0f - 1.0f) * -1.0f;

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());

  return intersection_point(ray, panel);
}
