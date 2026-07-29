#include "stage.h"


#include <SDL3/SDL_keycode.h>
#include <print>
#include <ranges>

#include "apeiron/engine/collision.h"
#include "apeiron/engine/enums.h"

#include "core/constants.h"
#include "core/palette.h"

#include "game/cast_spell.h"
#include "game/constants.h"

#include "util/utility.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto is_alive = std::views::filter([](const auto& e) { return e.health > 0; });
constexpr auto has_mesh = std::views::filter([](const auto& e) { return e.mesh_index != 20; });


}  // namespace


wis::Stage::Stage(entt::registry& registry,
    entt::dispatcher& dispatcher,
    const App_data& app_data,
    Game_data& game_data,
    const Atlas& atlas,
    Scene& scene)
    :
    registry_{registry},
    dispatcher_{dispatcher},
    app_data_{app_data},
    game_data_{game_data},
    atlas_{atlas},
    scene_{scene}
{
  dispatcher_.sink<event::Action_selected>().connect<&Stage::on_action_selected>(this);
}


void wis::Stage::init()
{
  init_renderer();

  sprite_entity_.transform().set_origin(0.0f, 0.0f, -val::tile_size() * 0.5f)
      .set_rotation_deg(45.0f, 0.0f, 0.0f)
      .set_rotation_pivot(engine::Axis::X, 0.0f, 0.0f, val::tile_size() * 0.5f);
}


void wis::Stage::init_scene()
{
  lattice_.init(scene_.size(), val::tile_size());

  auto field_size = lattice_.field_size();
  grid_.init(field_size, lattice_.size(), Palette::colors[47]);
  grid_.transform().set_position(field_size.x * 0.5f, 0.0f, field_size.y * 0.5f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);

  init_camera_controllers();

  const auto start_index = scene_.start_index();
  const auto start_position = lattice_.as_position_xz(start_index, glm::vec3{0.0f, 0.0f, 0.4f});

  player_ = Player{start_position, start_index, 102, 0.03f, 4.0f, val::tau()};
}


void wis::Stage::update()
{
  if (game_data_.control.use_orbit_camera) {
    orbit_controller_.apply(camera_);
  }
  else {
    free_controller_.apply(camera_);
  }
}


void wis::Stage::update_input(const engine::Input* input)
{
  if (input) {
    update_ego_camera(input);
  }
}


void wis::Stage::render()
{
  setup_view();

  if (app_data_.debug.wireframe) {
    Renderer::set_gl_wireframe(true);
  }
  else {
    Renderer::set_gl_wireframe(false);
  }

  pixel_renderer_.use();
  pixel_renderer_.set_time(app_data_.timing.runtime_s);

  Renderer::set_gl_depth_test(false);
  render_ground();
  render_overlay();
  render_debug_overlay();

  Renderer::set_gl_depth_test(true);
  render_sprites();
  render_debug();
}


void wis::Stage::handle_event([[maybe_unused]] const engine::Key_down_event& event)
{
  switch (event.keycode) {
    case SDLK_O:
      reset_orbit_controller();
      game_data_.control.use_orbit_camera = true;
    break;
  }
}


void wis::Stage::handle_event([[maybe_unused]] const engine::Key_up_event& event)
{
  switch (event.keycode) {
    case SDLK_O:
      game_data_.control.use_orbit_camera = false;
    break;
  }
}


void wis::Stage::handle_event(const engine::Mouse_button_down_event& event)
{
  const auto& cursor = game_data_.cursor.stage;

  switch (event.button) {
    case engine::Mouse_button::Right: {
      game_data_.camera.drag = true;
    }
    break;
    case engine::Mouse_button::Left: {
      if (selected_action_id_ == 0) {
        if (!scene_.tile(cursor.scene_index)->is_nil) {
          game_data_.stage.selected_index = cursor.scene_index;
        }
        else {
          game_data_.stage.selected_index = 0;
        }
      }

      if (selected_action_id_) {
        auto spell = scene_.spell(selected_action_id_);

        if (game_data_.cursor.type == Cursor_type::Cross && spell) {
          cast_spell(*spell, player_, scene_.tiles(), scene_.slimes(), cursor.scene_index);
          dispatcher_.trigger(event::Action_triggered{selected_action_id_});
        }
        else {
          dispatcher_.trigger(event::Action_deselected{selected_action_id_});
        }

        range_finder_.clear();
        selected_action_id_ = 0;
        game_data_.cursor.type = Cursor_type::White;
      }

      player_.mesh_index = 102;
    }
    break;
    default:;
  }
}


void wis::Stage::handle_event(const engine::Mouse_button_up_event& event)
{
  switch (event.button) {
    case engine::Mouse_button::Right: {
      game_data_.camera.drag = false;
    }
    break;
    default:;
  }
}


void wis::Stage::handle_event(const engine::Mouse_motion_event& event)
{
  auto& cursor = game_data_.cursor.stage;
  auto& stage = game_data_.stage;

  game_data_.cursor.type = Cursor_type::White;

  if (game_data_.control.use_orbit_camera && game_data_.camera.drag) {
    orbit_controller_.orbit(event.x_rel, -event.y_rel, game_data_.control.sensitivity * 4.0f);
  }
  else if (game_data_.camera.drag) {
    auto current_point = ground_point(event.x, event.y);
    auto previous_point = ground_point(event.x + event.x_rel, event.y + event.y_rel);
    drag_camera(current_point->x - previous_point->x, current_point->z - previous_point->z);
  }
  else if (auto point = ground_point(event.x, event.y); point) {
    cursor.ground_position = *point;

    if (auto index = lattice_.as_index(point->x, point->z); index) {
      cursor.scene_index = *index;
      cursor.scene_coords = lattice_.as_coords(*index);
      cursor.scene_position = lattice_.as_position_xz(*index);

      if (const auto* tile = scene_.tile(cursor.scene_index); tile && !tile->is_nil) {
        stage.hovered_index = cursor.scene_index;

        if (selected_action_id_ > 0) {
          if (range_finder_.within_valid_range(cursor.scene_index)) {
            game_data_.cursor.type = Cursor_type::Cross;
          }
        }
      }
      else {
        stage.hovered_index = 0;
      }
    }
    else {
      cursor.scene_index = 0;
      cursor.scene_coords = glm::uvec2{0};
      cursor.scene_position = glm::vec3{0.0f};
      stage.hovered_index = 0;
    }
  }
  else {
    cursor.scene_index = 0;
    cursor.scene_coords = glm::uvec2{0};
    cursor.scene_position = glm::vec3{0.0f};
    cursor.ground_position = glm::vec3{0.0f};
    stage.hovered_index = 0;
  }
}


void wis::Stage::handle_event([[maybe_unused]] const engine::Mouse_wheel_event& event)
{
}


void wis::Stage::on_action_selected(const event::Action_selected& event)
{
  selected_action_id_ = event.id;
  player_.mesh_index = 104;

  if (auto spell = scene_.spell(selected_action_id_); spell) {
    range_finder_.find(*spell, scene_, player_.scene_index);
  }

  std::print("Action {} selected\n", event.id);
}


void wis::Stage::init_renderer()
{
  renderer_.init();
  pixel_renderer_.init(val::pixel_size(), val::sprite_size());
}


void wis::Stage::init_camera_controllers()
{
  constexpr float pitch = -55.0f;
  constexpr float yaw = -90.0f;
  const float height = game_data_.camera.height;
  const auto dir = engine::direction_from_angles(pitch, yaw);
  const float x = lattice_.field_size().x * 0.5f;
  const float z = lattice_.field_size().y + 6.0f - static_cast<float>(scene_.margin().y * 2u);

  free_controller_.init(pitch, yaw, {x, height, z});
  orbit_controller_.init(pitch, yaw, height, free_controller_.position() + dir * height);
}


void wis::Stage::reset_orbit_controller()
{
  const float pitch = free_controller_.pitch();
  const float yaw = free_controller_.yaw();
  const float height = free_controller_.position().y;
  const auto dir = engine::direction_from_angles(pitch, yaw);

  orbit_controller_.init(pitch, yaw, height, free_controller_.position() + dir * height);
}


void wis::Stage::update_ego_camera(const engine::Input* input)
{
  if (app_data_.debug.noclip) {
    auto distance = 10.0f * app_data_.timing.delta_s;

    if (input->forward) { free_controller_.move(engine::Direction::Forward, distance); }
    if (input->backward) { free_controller_.move(engine::Direction::Backward, distance); }
    if (input->left) { free_controller_.move(engine::Direction::Left, distance); }
    if (input->right) { free_controller_.move(engine::Direction::Right, distance); }

    free_controller_.orient(input->mouse_x_rel, input->mouse_y_rel, 0.025f);
  }
}


void wis::Stage::drag_camera(float dx, float dy)
{
  free_controller_.move(dx, 0.0f, dy);
}


void wis::Stage::setup_view()
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


void wis::Stage::render_ground()
{
  for (const auto& tile : scene_.tiles() | has_mesh) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));
    pixel_renderer_.set_tile_position({tile.col, tile.row});
    pixel_renderer_.render(ground_entity_, atlas_.stage(), tile.mesh_index);
  }
}


void wis::Stage::render_overlay()
{
  const auto hovered_index = game_data_.stage.hovered_index;
  const auto selected_index = game_data_.stage.selected_index;

  if (hovered_index > 0 && hovered_index != selected_index) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(hovered_index));
    //pixel_renderer_.render(ground_entity_, atlas_.stage(), 381);
  }

  if (selected_index > 0) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(selected_index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 380);
  }

  {
    auto valid_range = range_finder_.valid_range();

    for (const auto index : range_finder_.full_range()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(index));

      if (std::ranges::find(valid_range, index) != valid_range.end()) {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 380, 5);
      }
      else {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 386);
      }
    }
  }
}


void wis::Stage::render_sprites()
{
  for (const auto& sprite : scene_.sprites()) {
    sprite_entity_.transform().set_position(sprite.position);
    pixel_renderer_.set_tile_position({sprite.scene_coords.x, sprite.scene_coords.y});
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), sprite.mesh_index);

    // if (sprite.scene_index == game_data_.stage.selected_index) {
    //   pixel_renderer_.render(sprite_entity_, atlas_.stage(), sprite.mesh_index + 20);
    // }
  }

  pixel_renderer_.enable_breathe();

  // Player
  {
    pixel_renderer_.set_breathe_amplitude(player_.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(player_.breathe_speed);
    pixel_renderer_.set_breathe_phase(player_.breathe_phase);

    auto pos = lattice_.as_position_xz(player_.scene_index, glm::vec3{0.0f, 0.0f, 0.4f});

    sprite_entity_.transform().set_position(pos);
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), player_.mesh_index);

    // if (player_.scene_index == game_data_.stage.selected_index) {
    //   pixel_renderer_.render(sprite_entity_, atlas_.stage(), player_.mesh_index + 20);
    // }
  }

  // Slimes
  for (const auto& slime : scene_.slimes() | is_alive) {
    pixel_renderer_.set_breathe_amplitude(slime.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(slime.breathe_speed);
    pixel_renderer_.set_breathe_phase(slime.breathe_phase);

    sprite_entity_.transform().set_position(slime.position);
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), slime.mesh_index);

    // if (slime.scene_index == game_data_.stage.selected_index) {
    //   pixel_renderer_.render(sprite_entity_, atlas_.stage(), slime.mesh_index + 20);
    // }
  }

  pixel_renderer_.enable_breathe(false);
}


void wis::Stage::render_debug()
{
}


void wis::Stage::render_debug_overlay()
{
  if (app_data_.debug.show_tile_info) {
    for (const auto& tile : scene_.tiles()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));

      if (tile.is_nil) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 360); }
      if (tile.north_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 382); }
      if (tile.south_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 383); }
      if (tile.east_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 384); }
      if (tile.west_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 385); }
    }
  }

  if (app_data_.debug.show_stage_grid) {
    renderer_.use();
    renderer_.render(grid_);
    pixel_renderer_.use();
  }
}


std::optional<glm::vec3> wis::Stage::ground_point(float screen_x, float screen_y)
{
  using namespace engine::collision;

  auto [nx, ny] = util::as_ndc(screen_x, screen_y, app_data_.window.logical_width,
      app_data_.window.logical_height);

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}
