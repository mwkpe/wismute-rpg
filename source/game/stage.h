#ifndef WIS_GAME_STAGE_H
#define WIS_GAME_STAGE_H


#include <optional>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "apeiron/engine/camera.h"
#include "apeiron/engine/camera_controller.h"
#include "apeiron/engine/event.h"
#include "apeiron/engine/input.h"
#include "apeiron/prefab/grid.h"

#include "app/app_data.h"

#include "core/atlas.h"
#include "core/lattice.h"
#include "core/pixel_renderer.h"
#include "core/renderer.h"

#include "game/events.h"
#include "game/game_data.h"
#include "game/player.h"
#include "game/range_finder.h"
#include "game/scene.h"
#include "game/game_state.h"


namespace wis {


class Stage final
{
public:
  Stage() = delete;
  Stage(entt::registry& registry, entt::dispatcher& dispatcher,
      const App_data& app_data, Game_data& game_data, const Atlas& atlas, Scene& scene);

  void init();
  void init_scene();
  void update();
  void update_input(const apeiron::engine::Input* input = nullptr);
  void render();

  // Engine event handlers
  void handle_event(const apeiron::engine::Key_down_event& event);
  void handle_event(const apeiron::engine::Key_up_event& event);
  void handle_event(const apeiron::engine::Mouse_button_down_event& event);
  void handle_event(const apeiron::engine::Mouse_button_up_event& event);
  void handle_event(const apeiron::engine::Mouse_motion_event& event);
  void handle_event(const apeiron::engine::Mouse_wheel_event& event);

  // Game event sinks
  void on_action_selected(const event::Action_selected& event);

private:
  void init_renderer();
  void init_camera_controllers();
  void init_scene_grid();
  void init_debug_grid();

  void reset_orbit_controller();

  void update_ego_camera(const apeiron::engine::Input* input);
  void drag_camera(float dx, float dy);
  void setup_view();

  void render_ground();
  void render_water();
  void render_shadows();
  void render_overlay();
  void render_sprites();
  void render_debug();
  void render_debug_overlay();

  // Helper
  std::optional<glm::vec3> ground_point(float screen_x, float screen_y);

  // ECS
  entt::registry& registry_;
  entt::dispatcher& dispatcher_;

  // Data
  const App_data& app_data_;
  Game_data& game_data_;
  Game_state game_state_;

  // Resources
  const Atlas& atlas_;

  // Content
  Scene& scene_;

  // Rendering
  Renderer renderer_;
  Pixel_renderer pixel_renderer_;
  apeiron::engine::Camera camera_;
  apeiron::engine::Entity ground_entity_;
  apeiron::engine::Entity sprite_entity_;
  apeiron::prefab::Grid grid_;

  // Controller
  apeiron::engine::Free_controller free_controller_;
  apeiron::engine::Orbit_controller orbit_controller_;

  // Tools
  Lattice lattice_;
  Range_finder range_finder_;

  // Debug
  apeiron::prefab::Grid debug_grid_;
  Player player_;
  std::uint32_t selected_action_id_ = 0;
  bool success_ = false;
  bool failure_ = false;
};


}  // namespace wis


#endif  // WIS_GAME_STAGE_H
