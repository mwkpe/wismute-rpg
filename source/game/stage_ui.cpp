#include "stage_ui.h"


#include <memory>
#include <print>
#include <ranges>

#include "app/error.h"
#include "core/palette.h"
#include "game/constants.h"
#include "game/cards.h"


namespace {


constexpr auto is_spent = std::views::filter([](const auto& a) { return a.is_spent; });
constexpr auto is_not_spent = std::views::filter([](const auto& a) { return !a.is_spent; });


}  // namespace


wis::Stage_ui::Stage_ui(entt::registry& registry,
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
    camera_{{0.0f, 36.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    action_panel_{dispatcher}
{
}


void wis::Stage_ui::init()
{
  // Viewport and GL init is done in stage class

  renderer_.init();
  pixel_renderer_.init(val::pixel_size(), val::sprite_size_ui());

  lattice_.init({16, 10}, val::tile_size_ui());
  auto field_size = lattice_.field_size();

  grid_.init(field_size, lattice_.size(), Palette::colors[47]);
  grid_.transform().set_position(0.0f, 0.001f, 0.0f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);

  setup_view();
  set_screen_limits();  // Needs view initialized

  float s = val::tile_size_ui();

  std::vector<Card> cards{
    Move{1},
    Move{2},
    Move{3},
    Move{4},
    Fireball{},
    Inferno{},
    Jet{},
    Splash{},
    Missile{},
    Teleport{}
  };

  action_panel_.set_size(10 * s, s);
  action_panel_.transform().set_position(0.0f, 0.0f, bottom_ - s * 0.5f)
      .set_rotation_deg(-15.0f, 0.0f, 0.0f)
      .set_rotation_pivot(apeiron::engine::Axis::X, 0.0f, 0.0f, s * 0.5f);
  action_panel_.apply();
  action_panel_.init(cards);

  undo_panel_.set_size(s, 8 * s);
  undo_panel_.transform().set_position(left_ + 1.2f, 0.0f, 0.0f)
      .set_rotation_deg(0.0f, 0.0f, -45.0f)
      .set_rotation_pivot(apeiron::engine::Axis::Z, s * -0.5f, 0.0f, 0.0f);
  undo_panel_.apply();
}


void wis::Stage_ui::update(float delta_s)
{
  action_panel_.update(delta_s);
}


void wis::Stage_ui::render()
{
  // Viewport and buffer selected in stage class

  Renderer::gl_clear_depth_buffer();
  setup_view();
  render_panel();
  render_debug();
}


bool wis::Stage_ui::handle_event(const apeiron::engine::Mouse_button_down_event& event)
{
  if (auto point = panel_point(event.x, event.y, action_panel_.collision_quad()); point) {
    action_panel_.click(*point);
    return true;
  }

  return false;
}


bool wis::Stage_ui::handle_event([[maybe_unused]] const apeiron::engine::Mouse_button_up_event& event)
{
  return false;
}


bool wis::Stage_ui::handle_event(const apeiron::engine::Mouse_motion_event& event)
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

  if (auto point = panel_point(event.x, event.y, action_panel_.collision_quad()); point) {
    cursor.panel_position = *point;
    cursor.on_panel = true;

    action_panel_.hover(*point);
  }
  else {
    action_panel_.clear_hover();
  }

  if (auto point = panel_point(event.x, event.y, undo_panel_.collision_quad()); point) {
    cursor.panel_position = *point;
    cursor.on_panel = true;
  }

  return false;
}


void wis::Stage_ui::setup_view()
{
  glm::mat4 projection = glm::perspective(glm::radians(game_data_.camera.fov),
      app_data_.window.aspect_ratio, 1.0f, 100.0f);

  pixel_renderer_.use();
  pixel_renderer_.preset_projection(projection);
  pixel_renderer_.preset_view(camera_.view());
  pixel_renderer_.set_view_projection();

  renderer_.use();
  renderer_.preset_projection(projection);
  renderer_.preset_view(camera_.view());
  renderer_.set_view_projection();
}


void wis::Stage_ui::set_screen_limits()
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


void wis::Stage_ui::render_panel()
{
  renderer_.use();
  //renderer_.render(action_panel_.quad(), Palette::colors[46]);

  pixel_renderer_.use();
  Renderer::set_gl_depth_test(false);

  for (const auto& widget : action_panel_.actions() | is_not_spent) {
    entity_.transform() = action_panel_.as_world_transform(widget.position);
    pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index);

    if (widget.hovered) {
      pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index + 40);
    }
  }

  pixel_renderer_.set_desaturation_factor(1.0f);
  pixel_renderer_.enable_desaturation();

  for (const auto& widget : action_panel_.actions() | is_spent) {
    entity_.transform() = action_panel_.as_world_transform(widget.position);
    pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index);
  }

  pixel_renderer_.enable_desaturation(false);
  Renderer::set_gl_depth_test(true);
}


void wis::Stage_ui::render_debug()
{
  renderer_.use();

  if (app_data_.debug.show_ui_grid) {
    renderer_.render(grid_);
  }

  //renderer_.render(action_panel_.quad(), Palette::colors[22]);
  //renderer_.render(undo_panel_.quad(), Palette::colors[22]);
}


std::optional<glm::vec3> wis::Stage_ui::screen_point(float screen_x, float screen_y)
{
  using namespace apeiron::engine::collision;

  float nx = screen_x / static_cast<float>(app_data_.window.logical_width) * 2.0f - 1.0f;
  float ny = (screen_y / static_cast<float>(app_data_.window.logical_height) * 2.0f - 1.0f) * -1.0f;

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}


std::optional<glm::vec2> wis::Stage_ui::panel_point(float screen_x, float screen_y,
    const apeiron::engine::collision::Quad& panel)
{
  using namespace apeiron::engine::collision;

  float nx = screen_x / static_cast<float>(app_data_.window.logical_width) * 2.0f - 1.0f;
  float ny = (screen_y / static_cast<float>(app_data_.window.logical_height) * 2.0f - 1.0f) * -1.0f;

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());

  return intersection_point(ray, panel);
}
