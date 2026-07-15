#ifndef WIS_GAME_GAME_UI_H
#define WIS_GAME_GAME_UI_H


#include <cstdint>
#include <optional>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "apeiron/engine/camera.h"
#include "apeiron/engine/collision.h"
#include "apeiron/engine/event.h"
#include "apeiron/prefab/grid.h"

#include "app/app_data.h"

#include "core/atlas.h"
#include "core/lattice.h"
#include "core/pixel_renderer.h"
#include "core/renderer.h"

#include "game/events.h"
#include "game/game_data.h"

#include "ui/action_panel.h"
#include "ui/panel.h"


namespace wis {


class Game_ui final
{
public:
  Game_ui() = delete;
  Game_ui(entt::registry& registry, entt::dispatcher& dispatcher,
      const App_data& app_data, Game_data& game_data, const Atlas& atlas);

  void init();
  void update();
  void render();

  // Engine event handlers
  bool handle_event(const apeiron::engine::Mouse_button_down_event& event);
  bool handle_event(const apeiron::engine::Mouse_button_up_event& event);
  bool handle_event(const apeiron::engine::Mouse_motion_event& event);

  // Game event sinks
  void on_action_selected(const event::Action_selected& event);

private:
  // Update
  void setup_view();
  void set_screen_limits();

  // Render
  void render_panel();
  void render_debug();

  // Helper
  std::optional<glm::vec3> screen_point(float screen_x, float screen_y);
  std::optional<glm::vec2> panel_point(float screen_x, float screen_y,
      const apeiron::engine::collision::Quad& panel);

  // ECS
  entt::registry& registry_;
  entt::dispatcher& dispatcher_;

  // Data
  const App_data& app_data_;
  Game_data& game_data_;

  // Resources
  const Atlas& atlas_;

  // Rendering
  Renderer renderer_;
  Pixel_renderer pixel_renderer_;
  const apeiron::engine::Camera camera_;
  apeiron::engine::Entity entity_;

  // Tools
  Lattice lattice_;

  // Panels
  ui::Panel undo_panel_;
  ui::Action_panel action_panel_;

  // Constants
  float top_ = -1.0f;
  float bottom_ = 1.0f;
  float left_ = top_;
  float right_ = bottom_;

  // Debug
  apeiron::prefab::Grid grid_;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_UI_H
