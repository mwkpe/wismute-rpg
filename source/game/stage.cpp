#include "stage.h"


#include <SDL3/SDL_keycode.h>
#include <iostream>

#include "apeiron/engine/color_converter.h"
#include "apeiron/engine/collision.h"
#include "apeiron/engine/enums.h"

#include "core/constants.h"
#include "core/palette.h"
#include "game/constants.h"


namespace {


namespace engine = apeiron::engine;


auto as_ndc(float x, float y, std::uint32_t w, std::uint32_t h) -> std::tuple<float, float>
{
  return {x / static_cast<float>(w) * 2.0f - 1.0f, 1.0f - y / static_cast<float>(h) * 2.0f};
}


}  // namespace


wis::Stage::Stage(entt::registry& registry,
    entt::dispatcher& dispatcher,
    const App_data& app_data,
    Game_data& game_data,
    const Atlas& atlas)
    :
    registry_{registry},
    dispatcher_{dispatcher},
    app_data_{app_data},
    game_data_{game_data},
    atlas_{atlas}
{
}


void wis::Stage::init()
{
  init_dispatcher();
  init_renderer();

  load_scene();
  init_camera_controllers();  // Must know scene size

  sprite_entity_.transform().set_origin(0.0f, 0.0f, -val::tile_size() * 0.5f)
      .set_rotation_deg(45.0f, 0.0f, 0.0f)
      .set_rotation_pivot(engine::Axis::X, 0.0f, 0.0f, val::tile_size() * 0.5f);

  player_ = Player{lattice_.as_position_xz(9, glm::vec3{0.0f, 0.0f, 0.4f}),
      9, 102, 0.03f, 4.0f, val::tau()};
}


void wis::Stage::load_scene()
{
  //scene_.create_test_mini();
  scene_.load_scene("assets/test_scene.json");
  lattice_.init(scene_.size(), val::tile_size());

  auto field_size = lattice_.field_size();
  grid_.init(field_size, lattice_.size(), Palette::colors[47]);
  grid_.transform().set_position(field_size.x * 0.5f, 0.001f, field_size.y * 0.5f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);
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
  //Renderer::gl_clear(Palette::colors[48]);
  Renderer::gl_clear(engine::as_rgb_norm("#181514"));

  if (app_data_.debug.wireframe) {
    Renderer::set_gl_wireframe(true);
  }
  else {
    Renderer::set_gl_wireframe(false);
  }

  pixel_renderer_.use();
  pixel_renderer_.set_time(app_data_.timing.runtime_s);

  render_ground();
  render_overlay();
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
  switch (event.button) {
    case engine::Mouse_button::Right: {
      game_data_.camera.drag = true;
    }
    break;
    case engine::Mouse_button::Left: {
      game_data_.stage.selected_scene_index = game_data_.cursor.stage.scene_index;
      path_finder_.clear();
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
    }
    else {
      cursor.scene_index = 0;
      cursor.scene_coords = glm::uvec2{0};
      cursor.scene_position = glm::vec3{0.0f};
      path_finder_.clear();
    }
  }
  else {
    cursor.scene_index = 0;
    cursor.scene_coords = glm::uvec2{0};
    cursor.scene_position = glm::vec3{0.0f};
    cursor.ground_position = glm::vec3{0.0f};
    path_finder_.clear();
  }

  if (game_data_.stage.selected_scene_index && cursor.scene_index &&
      game_data_.stage.selected_scene_index != cursor.scene_index &&
      game_data_.stage.selected_scene_index == player_.scene_index) {
    auto [a, b] = path_finder_.endpoints();

    if (player_.scene_index != a || cursor.scene_index != b) {
      path_finder_.search(scene_.tiles(), player_.scene_index, cursor.scene_index);
    }
  }
}


void wis::Stage::handle_event([[maybe_unused]] const engine::Mouse_wheel_event& event)
{
}


void wis::Stage::on_enemy_hit([[maybe_unused]] const event::Enemy_hit& event)
{
}


void wis::Stage::init_dispatcher()
{
  dispatcher_.sink<event::Enemy_hit>().connect<&Stage::on_enemy_hit>(this);
}


void wis::Stage::init_renderer()
{
  Renderer::gl_init();
  Renderer::set_gl_frame_buffer(0);
  Renderer::set_gl_viewport(0, 0, app_data_.window.pixel_width, app_data_.window.pixel_height);

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
  constexpr float z = 20.0f;

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
  for (const auto& tile : scene_.tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));
    pixel_renderer_.set_tile_position({tile.col, tile.row});
    pixel_renderer_.render(ground_entity_, atlas_.stage(), tile.mesh_index);
  }
}


void wis::Stage::render_overlay()
{
  Renderer::gl_clear_depth_buffer();

  if (game_data_.stage.selected_scene_index > 0) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(
        game_data_.stage.selected_scene_index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 381);
  }

  if (path_finder_.has_path()) {
    for (const auto index : path_finder_.path()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(index));
      pixel_renderer_.render(ground_entity_, atlas_.stage(), 386);
    }
  }

  if (app_data_.debug.show_tile_info) {
    Renderer::gl_clear_depth_buffer();

    for (const auto& tile : scene_.tiles()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));

      if (tile.north_index()) {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 382);
      }

      if (tile.south_index()) {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 383);
      }

      if (tile.east_index()) {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 384);
      }

      if (tile.west_index()) {
        pixel_renderer_.render(ground_entity_, atlas_.stage(), 385);
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

    if (sprite.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.stage(), sprite.mesh_index + 20);
    }
  }

  pixel_renderer_.enable_breathe();

  // Player
  {
    pixel_renderer_.set_breathe_amplitude(player_.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(player_.breathe_speed);
    pixel_renderer_.set_breathe_phase(player_.breathe_phase);

    sprite_entity_.transform().set_position(player_.position);
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), player_.mesh_index);

    if (player_.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.stage(), player_.mesh_index + 20);
    }
  }

  // Slimes
  for (const auto& slime : scene_.slimes()) {
    pixel_renderer_.set_breathe_amplitude(slime.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(slime.breathe_speed);
    pixel_renderer_.set_breathe_phase(slime.breathe_phase);

    sprite_entity_.transform().set_position(slime.position);
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), slime.mesh_index);

    if (slime.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.stage(), slime.mesh_index + 20);
    }
  }

  pixel_renderer_.enable_breathe(false);
}


void wis::Stage::render_debug()
{
  if (app_data_.debug.show_stage_grid) {
    renderer_.use();
    renderer_.render(grid_);
  }
}


std::optional<glm::vec3> wis::Stage::ground_point(float screen_x, float screen_y)
{
  using namespace engine::collision;

  auto [nx, ny] = as_ndc(screen_x, screen_y, app_data_.window.logical_width,
      app_data_.window.logical_height);

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}
