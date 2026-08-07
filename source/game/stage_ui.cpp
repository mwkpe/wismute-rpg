#include "stage_ui.h"


#include <memory>
#include <print>
#include <ranges>

#include "app/error.h"
#include "core/palette.h"
#include "game/constants.h"
#include "util/utility.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto not_available = std::views::filter([](const auto& e) { return !e.is_available; });
constexpr auto is_available = std::views::filter([](const auto& e) { return e.is_available; });


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
    camera_{{0.0f, 32.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    action_panel_{dispatcher}
{
}


void wis::Stage_ui::init()
{
  renderer_.init();
  pixel_renderer_.init(val::pixel_size(), val::sprite_size_ui());
  pixel_renderer_.set_palette(game_data_.color.palette);

  lattice_.init({16, 10}, val::tile_size_ui());
  auto field_size = lattice_.field_size();

  grid_.init(field_size, lattice_.size(), game_data_.color.palette[3]);
  grid_.transform().set_position(0.0f, 0.001f, 0.0f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);

  setup_view();
  set_screen_limits();  // Needs view initialized

  constexpr float tile_size = val::tile_size_ui();

  portrait_panel_.set_size(tile_size * 2.0f, tile_size * 2.0f);
  portrait_panel_.transform().set_position(right_ - tile_size, 0.0f, top_ + tile_size);
  portrait_panel_.apply();
}


void wis::Stage_ui::update(float delta_s)
{
  action_panel_.update(delta_s);

  if (game_data_.color.live_update_palette) {
    pixel_renderer_.use();
    pixel_renderer_.set_palette(game_data_.color.palette);
  }
}


void wis::Stage_ui::render()
{
  Renderer::gl_clear_depth_buffer();

  setup_view();
  render_panels();
  render_debug();
  render_cursor();

  game_data_.stats.ui_draw_calls = pixel_renderer_.draw_calls();
}


void wis::Stage_ui::set_spells(std::span<const Spell_slot> spell_slots)
{
  float s = val::tile_size_ui();

  action_panel_.set_size(static_cast<float>(spell_slots.size()) * s, s);
  action_panel_.transform().set_position(0.0f, 0.0f, bottom_ - s * 0.5f)
      .set_rotation_deg(-15.0f, 0.0f, 0.0f)
      .set_rotation_pivot(engine::Axis::X, 0.0f, 0.0f, s * 0.5f);
  action_panel_.apply();
  action_panel_.init(spell_slots);
}


bool wis::Stage_ui::handle_event(const engine::Mouse_button_down_event& event)
{
  switch (event.button) {
    case engine::Mouse_button::Left: {
      if (auto point = panel_point(event.x, event.y, action_panel_.collision_quad()); point) {
        action_panel_.click(*point);
        return true;
      }
    }
    break;
    case engine::Mouse_button::Right: {
      if (auto point = panel_point(event.x, event.y, action_panel_.collision_quad()); point) {
        return true;
      }
    }
    break;
    default:;
  }

  return false;
}


bool wis::Stage_ui::handle_event([[maybe_unused]] const engine::Mouse_button_up_event& event)
{
  return false;
}


bool wis::Stage_ui::handle_event(const engine::Mouse_motion_event& event)
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

  if (auto point = panel_point(event.x, event.y, portrait_panel_.collision_quad()); point) {
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
    throw Error::format("Error setting screen limits at {}, {}", logical_left, logical_top);
  }

  if (auto point = screen_point(logical_right, logical_bottom); point) {
    right_ = point->x;
    bottom_ = point->z;
  }
  else {
    throw Error::format("Error setting screen limits at {}, {}", logical_right, logical_bottom);
  }
}


void wis::Stage_ui::render_panels()
{
  Renderer::set_gl_depth_test(false);

  renderer_.use();
  //renderer_.render(action_panel_.quad(), Palette::colors[12]);
  //renderer_.render(portrait_panel_.quad(), Palette::colors[12]);

  pixel_renderer_.use();

  // Actions
  for (const auto& widget : action_panel_.actions() | is_available) {
    entity_.transform() = action_panel_.as_world_transform(widget.position);
    pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index);

    if (widget.hovered) {
      pixel_renderer_.render(entity_, atlas_.ui(), 40);
    }

    if (game_data_.stage.amplification[widget.element]) {
      pixel_renderer_.render(entity_, atlas_.ui(), 41);
    }
  }

  //pixel_renderer_.set_desaturation_factor(1.0f);
  //pixel_renderer_.enable_desaturation();

  for (const auto& widget : action_panel_.actions() | not_available) {
    entity_.transform() = action_panel_.as_world_transform(widget.position);
    pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index + 20);
  }

  //pixel_renderer_.enable_desaturation(false);

  // Portrait
  for (const auto& widget : portrait_panel_.decorations()) {
    entity_.transform() = portrait_panel_.as_world_transform(widget.position);
    pixel_renderer_.render(entity_, atlas_.ui(), widget.mesh_index);
  }

  Renderer::set_gl_depth_test(true);
}


void wis::Stage_ui::render_debug()
{
  renderer_.use();

  if (app_data_.debug.show_ui_grid) {
    renderer_.render(grid_);
  }

  //renderer_.render(action_panel_.quad(), Palette::colors[22]);
  //renderer_.render(portrait_panel_.quad(), Palette::colors[22]);
}


void wis::Stage_ui::render_cursor()
{
  pixel_renderer_.use();
  Renderer::set_gl_depth_test(false);

  cursor_.transform().set_position(game_data_.cursor.ui.screen_position);

  std::uint32_t cursor_index = 90;

  switch (game_data_.cursor.type) {
    case Cursor_type::White: cursor_index = 90; break;
    case Cursor_type::Green: cursor_index = 91; break;
    case Cursor_type::Red: cursor_index = 92; break;
    case Cursor_type::Check: cursor_index = 93; break;
    case Cursor_type::Verboten: cursor_index = 94; break;
    case Cursor_type::Wait: cursor_index = 95; break;
    case Cursor_type::Cross: cursor_index = 96; break;
  }

  pixel_renderer_.render(cursor_, atlas_.ui(), cursor_index);

  Renderer::set_gl_depth_test(true);
}


std::optional<glm::vec3> wis::Stage_ui::screen_point(float screen_x, float screen_y)
{
  using namespace engine::collision;

  auto [nx, ny] = util::as_ndc(screen_x, screen_y, app_data_.window.logical_width,
      app_data_.window.logical_height);

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}


std::optional<glm::vec2> wis::Stage_ui::panel_point(float screen_x, float screen_y,
    const apeiron::engine::collision::Quad& panel)
{
  using namespace engine::collision;

  auto [nx, ny] = util::as_ndc(screen_x, screen_y, app_data_.window.logical_width,
      app_data_.window.logical_height);

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());

  return intersection_point(ray, panel);
}
