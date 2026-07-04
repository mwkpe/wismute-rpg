#ifndef WIS_APP_SETTINGS_H
#define WIS_APP_SETTINGS_H


#include <string_view>


namespace wis {


struct Window_settings
{
  int width = 1280;
  int height = 800;
  bool fullscreen = false;
  bool ignore_scaling = false;
};


struct Render_settings
{
  int msaa_samples = 4;
  int max_fps = 120;
  bool vsync = false;
  bool limit_fps = true;
};


struct Settings
{
  Window_settings window;
  Render_settings render;
};


Settings load_settings(std::string_view filepath);
void save_settings(const Settings& s, std::string_view filepath);


}  // namespace wis


#endif  // WIS_APP_SETTINGS_H
