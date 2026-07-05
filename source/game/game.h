#ifndef WIS_GAME_GAME_H
#define WIS_GAME_GAME_H


#include <entt/entt.hpp>

#include "apeiron/engine/event.h"
#include "apeiron/engine/input.h"
#include "app/app_data.h"
#include "engine/atlas.h"
#include "game/events.h"
#include "game/game_data.h"
#include "game/game_ui.h"
#include "game/stage.h"


namespace wis {


class Game final
{
public:
  Game() = delete;
  Game(const App_data& app_data, Game_data& game_data);

  void init();
  void update(const apeiron::engine::Event_queue& engine_events,
      const apeiron::engine::Input* input = nullptr);
  void render();

  // Engine event handling
  template<typename T> void operator()(const T& event)
  {
    if constexpr (requires { this->handle_event(event); }) {
      this->handle_event(event);
    }
  }

  // Game event sinks
  void on_achievement_unlocked(const event::Achievement_unlocked& event);

private:
  void handle_event(const apeiron::engine::Key_down_event& event);
  void handle_event(const apeiron::engine::Key_up_event& event);
  void handle_event(const apeiron::engine::Mouse_button_down_event& event);
  void handle_event(const apeiron::engine::Mouse_button_up_event& event);
  void handle_event(const apeiron::engine::Mouse_motion_event& event);
  void handle_event(const apeiron::engine::Mouse_wheel_event& event);

  entt::registry registry_;
  entt::dispatcher dispatcher_;
  const App_data& app_data_;
  Game_data& game_data_;
  Atlas atlas_;
  Stage stage_;
  Game_ui ui_;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_H
