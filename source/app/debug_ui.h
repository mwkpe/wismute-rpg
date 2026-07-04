#ifndef WIS_APP_DEBUG_UI_H
#define WIS_APP_DEBUG_UI_H


#include "apeiron/opengl/gui.h"
#include "app/app_data.h"
#include "app/settings.h"
#include "game/game_data.h"


namespace wis {


class Debug_ui final : public apeiron::opengl::Gui
{
public:
  Debug_ui(SDL_Window* window, SDL_GLContext context)
      : apeiron::opengl::Gui{window, context} {}
  void setup();
  void build(Settings& settings, App_data& app_data, Game_data& game_data);
  [[nodiscard]] bool has_mouse() const { return has_mouse_; }

private:
  bool has_mouse_ = false;
};


}  // namespace wis


#endif  // WIS_APP_DEBUG_UI_H
