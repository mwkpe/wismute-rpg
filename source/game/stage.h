#ifndef WIS_GAME_STAGE_H
#define WIS_GAME_STAGE_H


#include <optional>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "apeiron/engine/camera.h"
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
#include "game/path_finder.h"
#include "game/player.h"
#include "game/scene.h"


namespace wis {


class Stage final
{
public:
  Stage() = delete;
  Stage(entt::registry& registry, entt::dispatcher& dispatcher,
      const App_data& app_data, Game_data& game_data, const Atlas& atlas);

  void init();
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
  void on_enemy_hit(const event::Enemy_hit& event);

private:
  // Update
  void update_ego_camera(const apeiron::engine::Input* input);
  void drag_camera(float dx, float dy);
  void setup_view();

  // Render
  void render_ground();
  void render_overlay();
  void render_sprites();
  void render_debug();

  // Helper
  std::optional<glm::vec3> ground_point(float screen_x, float screen_y);

  // ECS
  entt::registry& registry_;
  entt::dispatcher& dispatcher_;

  // Data
  const App_data& app_data_;
  Game_data& game_data_;

  // Resources
  const Atlas& atlas_;

  // Content
  Scene scene_;

  // Rendering
  Renderer renderer_;
  Pixel_renderer pixel_renderer_;
  apeiron::engine::Camera camera_;
  apeiron::engine::Entity ground_entity_;
  apeiron::engine::Entity sprite_entity_;

  // Tools
  Lattice lattice_;
  Path_finder path_finder_;

  // Debug
  apeiron::prefab::Grid grid_;
  Player player_;
};


}  // namespace wis


#endif  // WIS_GAME_STAGE_H
