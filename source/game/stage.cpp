#include "stage.h"


#include <SDL3/SDL_keycode.h>
#include <iostream>

#include "apeiron/engine/collision.h"
#include "apeiron/engine/enums.h"

#include "engine/constants.h"
#include "engine/palette.h"
#include "game/constants.h"


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
  Renderer::gl_init();
  Renderer::set_gl_viewport(0, 0, app_data_.window.pixel_width, app_data_.window.pixel_height);

  renderer_.init();
  pixel_renderer_.init(0.1f, 16);

  scene_.create_test();
  lattice_.init(scene_.size(), tile_size());

  auto field_size = lattice_.field_size();

  grid_.init(field_size, lattice_.size(), Palette::colors[47]);
  grid_.transform().set_position(field_size.x * 0.5f, 0.001f, field_size.y * 0.5f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);

  sprite_entity_.transform().set_origin(0.0f, 0.0f, -tile_size() * 0.5f)
      .set_rotation_deg(45.0f, 0.0f, 0.0f)
      .set_rotation_pivot(apeiron::engine::Axis::X, 0.0f, 0.0f, tile_size() * 0.5f);

  camera_.setup(-65.0f, -90.0f, {field_size.x * 0.5f, 22.0f, 17.0f});

  dispatcher_.sink<event::Enemy_hit>().connect<&Stage::on_enemy_hit>(this);

  player_ = Player{glm::vec3{8.8f, 0.0f, 7.6f}, 53, 102, 0.03f, 4.0f, tau()};
}


void wis::Stage::update()
{
  camera_.update();
}


void wis::Stage::update_input(const apeiron::engine::Input* input)
{
  if (input) {
    update_ego_camera(input);
  }
}


void wis::Stage::render()
{
  setup_view();

  Renderer::set_gl_frame_buffer(0);
  Renderer::set_gl_viewport(0, 0, app_data_.window.pixel_width, app_data_.window.pixel_height);
  Renderer::gl_clear(Palette::colors[48]);

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


void wis::Stage::handle_event([[maybe_unused]] const apeiron::engine::Key_down_event& event)
{
}


void wis::Stage::handle_event([[maybe_unused]] const apeiron::engine::Key_up_event& event)
{
}


void wis::Stage::handle_event(const apeiron::engine::Mouse_button_down_event& event)
{
  switch (event.button) {
    case apeiron::engine::Mouse_button::Right: {
      game_data_.camera.drag = true;
    }
    break;
    case apeiron::engine::Mouse_button::Left: {
      game_data_.stage.selected_scene_index = game_data_.cursor.scene_index;
      path_finder_.clear();
    }
    break;
    default:;
  }
}


void wis::Stage::handle_event(const apeiron::engine::Mouse_button_up_event& event)
{
  switch (event.button) {
    case apeiron::engine::Mouse_button::Right: {
      game_data_.camera.drag = false;
    }
    break;
    default:;
  }
}


void wis::Stage::handle_event(const apeiron::engine::Mouse_motion_event& event)
{
  if (game_data_.camera.drag) {
    auto current_point = ground_point(event.x, event.y);
    auto previous_point = ground_point(event.x + event.x_rel, event.y + event.y_rel);
    drag_camera(current_point->x - previous_point->x, current_point->z - previous_point->z);
  }
  else if (auto point = ground_point(event.x, event.y); point) {
    game_data_.cursor.ground_position = *point;

    if (auto index = lattice_.as_index(point->x, point->z); index) {
      game_data_.cursor.scene_index = *index;
      game_data_.cursor.scene_coords = lattice_.as_coords(*index);
      game_data_.cursor.scene_position = lattice_.as_position_xz(*index);
    }
    else {
      game_data_.cursor.scene_index = 0;
      game_data_.cursor.scene_coords = glm::uvec2{0};
      game_data_.cursor.scene_position = glm::vec3{0.0f};
      path_finder_.clear();
    }
  }
  else {
    game_data_.cursor.scene_index = 0;
    game_data_.cursor.scene_coords = glm::uvec2{0};
    game_data_.cursor.scene_position = glm::vec3{0.0f};
    game_data_.cursor.ground_position = glm::vec3{0.0f};
    path_finder_.clear();
  }

  if (game_data_.stage.selected_scene_index && game_data_.cursor.scene_index &&
      game_data_.stage.selected_scene_index != game_data_.cursor.scene_index &&
      game_data_.stage.selected_scene_index == player_.scene_index) {
    auto [a, b] = path_finder_.endpoints();

    if (player_.scene_index != a || game_data_.cursor.scene_index != b) {
      path_finder_.search(scene_.tiles(), player_.scene_index, game_data_.cursor.scene_index);
    }
  }
}


void wis::Stage::handle_event([[maybe_unused]] const apeiron::engine::Mouse_wheel_event& event)
{
  float delta = static_cast<float>(event.y);

  auto& camera = game_data_.camera;
  camera.zoom += delta;

  if (camera.zoom < camera.min_zoom) {
    camera.zoom = camera.min_zoom;
    delta = 0.0f;
  }
  else if (camera.zoom > camera.max_zoom) {
    camera.zoom = camera.max_zoom;
    delta = 0.0f;
  }

  camera_.move(0.0f, -delta, -delta * 0.3f);
  float pitch = camera_.pitch() + delta * 0.5f;
  camera_.set_pitch(pitch);
}


void wis::Stage::on_enemy_hit([[maybe_unused]] const event::Enemy_hit& event)
{
}


void wis::Stage::render_ground()
{
  for (const auto& tile : scene_.tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));
    pixel_renderer_.set_tile_position({tile.col, tile.row});
    pixel_renderer_.render(ground_entity_, atlas_.meshes(), tile.mesh_index);
  }
}


void wis::Stage::render_overlay()
{
  Renderer::gl_clear_depth_buffer();

  if (game_data_.stage.selected_scene_index > 0) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(
        game_data_.stage.selected_scene_index));
    pixel_renderer_.render(ground_entity_, atlas_.meshes(), 381);
  }

  if (path_finder_.has_path()) {
    for (const auto index : path_finder_.path()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(index));
      pixel_renderer_.render(ground_entity_, atlas_.meshes(), 386);
    }
  }

  if (app_data_.debug.show_tile_info) {
    Renderer::gl_clear_depth_buffer();

    for (const auto& tile : scene_.tiles()) {
      ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));

      if (tile.north_index) {
        pixel_renderer_.render(ground_entity_, atlas_.meshes(), 382);
      }

      if (tile.south_index) {
        pixel_renderer_.render(ground_entity_, atlas_.meshes(), 383);
      }

      if (tile.east_index) {
        pixel_renderer_.render(ground_entity_, atlas_.meshes(), 384);
      }

      if (tile.west_index) {
        pixel_renderer_.render(ground_entity_, atlas_.meshes(), 385);
      }
    }
  }
}


void wis::Stage::render_sprites()
{
  for (const auto& sprite : scene_.sprites()) {
    sprite_entity_.transform().set_position(sprite.position);
    pixel_renderer_.set_tile_position({sprite.scene_coords.x, sprite.scene_coords.y});
    pixel_renderer_.render(sprite_entity_, atlas_.meshes(), sprite.mesh_index);

    if (sprite.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.meshes(), sprite.mesh_index + 20);
    }
  }

  pixel_renderer_.enable_breathe();

  // Player
  {
    pixel_renderer_.set_breathe_amplitude(player_.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(player_.breathe_speed);
    pixel_renderer_.set_breathe_phase(player_.breathe_phase);

    sprite_entity_.transform().set_position(player_.position);
    pixel_renderer_.render(sprite_entity_, atlas_.meshes(), player_.mesh_index);

    if (player_.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.meshes(), player_.mesh_index + 20);
    }
  }

  // Slimes
  for (const auto& slime : scene_.slimes()) {
    pixel_renderer_.set_breathe_amplitude(slime.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(slime.breathe_speed);
    pixel_renderer_.set_breathe_phase(slime.breathe_phase);

    sprite_entity_.transform().set_position(slime.position);
    pixel_renderer_.render(sprite_entity_, atlas_.meshes(), slime.mesh_index);

    if (slime.scene_index == game_data_.stage.selected_scene_index) {
      pixel_renderer_.render(sprite_entity_, atlas_.meshes(), slime.mesh_index + 20);
    }
  }

  pixel_renderer_.enable_breathe(false);
}


void wis::Stage::render_debug()
{
  renderer_.use();

  if (app_data_.debug.show_grid) {
    renderer_.render(grid_);
  }
}


void wis::Stage::update_ego_camera(const apeiron::engine::Input* input)
{
  if (app_data_.debug.noclip) {
    using Direction = apeiron::engine::Camera::Direction;
    auto distance = 10.0f * app_data_.timing.delta_s;

    if (input->forward) { camera_.move(Direction::Forward, distance); }
    if (input->backward) { camera_.move(Direction::Backward, distance); }
    if (input->left) { camera_.move(Direction::Left, distance); }
    if (input->right) { camera_.move(Direction::Right, distance); }

    camera_.orient(input->mouse_x_rel, input->mouse_y_rel, 0.025f);
  }
}


void wis::Stage::drag_camera(float dx, float dy)
{
  camera_.move(dx, 0.0f, dy);
}


void wis::Stage::setup_view()
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


std::optional<glm::vec3> wis::Stage::ground_point(float screen_x, float screen_y)
{
  using namespace apeiron::engine::collision;

  float nx = screen_x / static_cast<float>(app_data_.window.logical_width) * 2.0f - 1.0f;
  float ny = (screen_y / static_cast<float>(app_data_.window.logical_height) * 2.0f - 1.0f) * -1.0f;

  Ray ray = screen_raycast(nx, ny, renderer_.inverse_view_projection());
  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

  return intersection_point(ray, plane);
}
