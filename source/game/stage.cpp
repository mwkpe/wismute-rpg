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
#include "game/spells.h"

#include "util/utility.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto is_alive = std::views::filter([](const auto& e) { return e.health > 0; });
constexpr auto has_mesh = std::views::filter([](const auto& e) { return e.mesh_index != 20; });

constexpr auto is_water = std::views::filter([](const auto& e) { return e.mesh_index == 25; });
constexpr auto not_water = std::views::filter([](const auto& e) { return e.mesh_index != 25; });


bool is_amplified(std::span<const wis::Tile> tiles, std::uint32_t index, wis::Element element)
{
  for (const auto i : tiles[index].cardinals()) {
    if (tiles[i].element == element) {
      return true;
    }
  }

  return false;
}


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

  sprite_entity_.transform().set_origin(0.0f, 0.0f, -cval::tile_size * 0.5f)
      .set_rotation_deg(45.0f, 0.0f, 0.0f)
      .set_rotation_pivot(engine::Axis::X, 0.0f, 0.0f, cval::tile_size * 0.5f);
}


void wis::Stage::init_scene()
{
  lattice_.init(scene_.size(), cval::tile_size);

  init_camera_controllers();
  init_scene_grid();
  init_debug_grid();

  const auto start_index = scene_.start_index();
  const auto start_position = lattice_.as_position_xz(start_index, glm::vec3{0.0f, 0.0f, 0.4f});

  player_ = Player{start_position, start_index, 102, 0.03f, 4.0f, cval::tau()};
  player_.animation.init(102, 104, 75);

  success_ = false;
}


void wis::Stage::update()
{
  if (game_data_.control.use_orbit_camera) {
    orbit_controller_.apply(camera_);
  }
  else {
    free_controller_.apply(camera_);
  }

  player_.animation.update(app_data_.timing.elapsed_ns);

  if (!failure_ && !success_) {
    const auto health = std::ranges::fold_left(scene_.slimes() |
        std::views::transform(&Slime::health), 0u, std::plus{});

    if (health == 0) {
      std::print("All slimes defeated\n");
      success_ = true;
    }
  }

  if (!success_ && !failure_ &&
      std::ranges::none_of(scene_.spell_slots(), std::identity{}, &Spell_slot::is_available)) {
    std::print("Failed to defeat all slimes\n");
    failure_ = true;
  }

  if (game_data_.color.live_update_palette) {
    pixel_renderer_.use();
    pixel_renderer_.set_palette(game_data_.color.palette);
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

  if (game_data_.render.shadows) {
    render_shadows();
  }

  render_overlay();
  render_water();
  render_debug_overlay();

  Renderer::set_gl_depth_test(true);
  render_sprites();
  render_debug();

  game_data_.stats.stage_draw_calls = pixel_renderer_.draw_calls();
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
        auto spell_slot = scene_.spell_slot(selected_action_id_);

        if (game_data_.cursor.type == Cursor_type::Cross && spell_slot) {
          cast_spell(spell_slot->spell, player_, scene_.tiles(), scene_.slimes(),
              cursor.scene_index);
          spell_slot->is_available = false;
          dispatcher_.trigger(event::Action_triggered{selected_action_id_});

          if (std::holds_alternative<Blink>(spell_slot->spell)) {
            auto& amplification = game_data_.stage.amplification;
            auto index = player_.scene_index;
            auto tiles = scene_.tiles();

            amplification[Element::Fire] = is_amplified(tiles, index, Element::Fire);
            amplification[Element::Water] = is_amplified(tiles, index, Element::Water);
            amplification[Element::Wind] = is_amplified(tiles, index, Element::Wind);
            amplification[Element::Aether] = is_amplified(tiles, index, Element::Aether);
          }
        }
        else {
          dispatcher_.trigger(event::Action_deselected{selected_action_id_});
        }

        range_finder_.clear();
        selected_action_id_ = 0;
        game_data_.cursor.type = Cursor_type::White;
      }

      player_.animation.reset();
    }
    break;
    case engine::Mouse_button::Right: {
      game_data_.camera.drag = true;
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
        cursor.map_index = tile->map_index;
        stage.hovered_index = cursor.scene_index;

        if (selected_action_id_ > 0) {
          if (range_finder_.within_target_tiles(cursor.scene_index)) {
            game_data_.cursor.type = Cursor_type::Cross;
          }
        }
      }
      else {
        cursor.map_index = 0;
        stage.hovered_index = 0;
      }
    }
    else {
      cursor.map_index = 0;
      cursor.scene_index = 0;
      cursor.scene_coords = glm::uvec2{0};
      cursor.scene_position = glm::vec3{0.0f};
      stage.hovered_index = 0;
    }
  }
  else {
    cursor.map_index = 0;
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
  if (selected_action_id_ == 0) {
    player_.animation.start();
  }

  selected_action_id_ = event.id;

  if (auto spell_slot = scene_.spell_slot(selected_action_id_); spell_slot) {
    range_finder_.find(spell_slot->spell, scene_, player_.scene_index);
  }

  std::print("Action {} selected\n", event.id);
}


void wis::Stage::init_renderer()
{
  renderer_.init();
  pixel_renderer_.init(cval::pixel_size, cval::sprite_size);
  pixel_renderer_.set_palette(game_data_.color.palette);
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


void wis::Stage::init_scene_grid()
{
  Lattice stage_lattice;
  stage_lattice.init(scene_.size() - scene_.margin() * 2u - glm::uvec2{2, 2}, cval::tile_size);

  auto size = stage_lattice.field_size();

  auto x = (size.x + static_cast<float>(scene_.margin().x + 1u) * 2.0f * cval::tile_size) * 0.5f;
  auto z = (size.y + static_cast<float>(scene_.margin().y + 1u) * 2.0f * cval::tile_size) * 0.5f;

  grid_.init(size, stage_lattice.size(), game_data_.color.palette[3], false);
  grid_.transform().set_position(x, 0.0f, z).set_rotation_deg(-90.0f, 0.0f, 0.0f);
}


void wis::Stage::init_debug_grid()
{
  auto size = lattice_.field_size();

  debug_grid_.init(size, lattice_.size(), game_data_.color.palette[3]);
  debug_grid_.transform().set_position(size.x * 0.5f, 0.0f, size.y * 0.5f)
      .set_rotation_deg(-90.0f, 0.0f, 0.0f);
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
  if (game_data_.render.grid) {
    renderer_.use();
    renderer_.render(grid_);
    pixel_renderer_.use();
  }

  for (const auto& tile : scene_.tiles() | has_mesh | not_water) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), tile.mesh_index);
  }
}


void wis::Stage::render_water()
{
  for (const auto& tile : scene_.tiles() | has_mesh | is_water) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(tile.index));
    pixel_renderer_.set_tile_position({tile.col, tile.row});
    pixel_renderer_.render(ground_entity_, atlas_.stage(), tile.mesh_index);
  }
}


void wis::Stage::render_shadows()
{
  pixel_renderer_.enable_tile_tilt();
  pixel_renderer_.set_tile_tilt(-4.0f);

  Renderer::set_gl_blend(true);
  pixel_renderer_.enable_blending();
  pixel_renderer_.set_blending_alpha(0.7f);


  ground_entity_.transform().set_scale(1.0f, 1.0f, 0.8f);
  auto offset = cval::sprite_offset - glm::vec3{0.0f, 0.0f, cval::tile_size * 0.4f};

  for (const auto& sprite : scene_.sprites()) {
    auto mesh_index = sprite.mesh_index == 61 ? 62 : sprite.mesh_index;
    mesh_index = sprite.mesh_index == 60 ? 63 : mesh_index;
    ground_entity_.transform().set_position(lattice_.as_position_xz(sprite.scene_index, offset));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), mesh_index, 1);
  }

  pixel_renderer_.enable_breathe();

  // Player
  {
    pixel_renderer_.set_breathe_amplitude(player_.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(player_.breathe_speed);
    pixel_renderer_.set_breathe_phase(player_.breathe_phase);

    ground_entity_.transform().set_position(lattice_.as_position_xz(player_.scene_index, offset));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), player_.animation.current_frame(), 1);
  }

  // Slimes
  for (const auto& slime : scene_.slimes() | is_alive) {
    pixel_renderer_.set_breathe_amplitude(slime.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(slime.breathe_speed);
    pixel_renderer_.set_breathe_phase(slime.breathe_phase);

    ground_entity_.transform().set_position(lattice_.as_position_xz(slime.scene_index, offset));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), slime.mesh_index, 1);
  }

  ground_entity_.transform().set_scale(1.0f, 1.0f, 1.0f);

  Renderer::set_gl_blend(false);
  pixel_renderer_.enable_breathe(false);
  pixel_renderer_.enable_tile_tilt(false);
  pixel_renderer_.enable_blending(false);
}


void wis::Stage::render_overlay()
{
  const auto hovered_index = game_data_.stage.hovered_index;
  const auto selected_index = game_data_.stage.selected_index;

  if (hovered_index > 0 && hovered_index != selected_index) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(hovered_index));
    //pixel_renderer_.render(ground_entity_, atlas_.stage(), 380);
  }

  if (selected_index > 0) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(selected_index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 381);
  }

  // Range finder
  for (const auto index : range_finder_.target_tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 383, 7);
  }

  for (const auto index : range_finder_.empty_tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 386);
  }

  for (const auto index : range_finder_.invalid_tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 388);
  }

  for (const auto index : range_finder_.marker_tiles()) {
    ground_entity_.transform().set_position(lattice_.as_position_xz(index));
    pixel_renderer_.render(ground_entity_, atlas_.stage(), 389);
  }
}


void wis::Stage::render_sprites()
{
  for (const auto& sprite : scene_.sprites()) {
    sprite_entity_.transform().set_position(sprite.position);
    pixel_renderer_.set_tile_position({sprite.scene_coords.x, sprite.scene_coords.y});
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), sprite.mesh_index);
  }

  pixel_renderer_.enable_breathe();

  // Player
  {
    pixel_renderer_.set_breathe_amplitude(player_.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(player_.breathe_speed);
    pixel_renderer_.set_breathe_phase(player_.breathe_phase);

    sprite_entity_.transform()
        .set_position(lattice_.as_position_xz(player_.scene_index, cval::sprite_offset));
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), player_.animation.current_frame());
  }

  // Slimes
  for (const auto& slime : scene_.slimes() | is_alive) {
    pixel_renderer_.set_breathe_amplitude(slime.breathe_amplitude);
    pixel_renderer_.set_breathe_speed(slime.breathe_speed);
    pixel_renderer_.set_breathe_phase(slime.breathe_phase);

    sprite_entity_.transform()
        .set_position(lattice_.as_position_xz(slime.scene_index, cval::sprite_offset));
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), slime.mesh_index);
  }

  pixel_renderer_.enable_breathe(false);

  // Health bars
  for (const auto& slime : scene_.slimes() | is_alive) {
    std::uint32_t health_index = 220;

    switch (slime.health) {
      case 10: { health_index += 0; } break;
      case 20: { health_index += 1; } break;
      case 30: { health_index += 2; } break;
      case 40: { health_index += 3; } break;
    }

    switch (slime.mesh_index) {
      case 140: { health_index += 0; } break;
      case 141: { health_index += 20; } break;
      case 142: { health_index += 4; } break;
      case 143: { health_index += 24; } break;
    }

    sprite_entity_.transform()
        .set_position(lattice_.as_position_xz(slime.scene_index, cval::sprite_offset));
    pixel_renderer_.render(sprite_entity_, atlas_.stage(), health_index);
  }
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
      if (tile.north_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 340); }
      if (tile.south_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 341); }
      if (tile.east_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 342); }
      if (tile.west_index()) { pixel_renderer_.render(ground_entity_, atlas_.stage(), 343); }
    }
  }

  if (app_data_.debug.show_stage_grid) {
    renderer_.use();
    renderer_.render(debug_grid_);
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
