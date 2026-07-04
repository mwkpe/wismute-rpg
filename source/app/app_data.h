#ifndef WIS_APP_APP_DATA_H
#define WIS_APP_APP_DATA_H


#include <cstdint>
#include <string>


namespace wis {


enum class App_state { Menu, Game, Quit };


struct Timing_data
{
  std::uint64_t runtime_ns = 0;
  std::uint64_t elapsed_ns = 8;
  float delta_s = 0.00833f;
  float runtime_s = 0.0f;
};


struct Window_data
{
  std::uint32_t logical_width = 1280;
  std::uint32_t logical_height = 720;
  std::uint32_t pixel_width = 1280;
  std::uint32_t pixel_height = 720;
  float density = 1.0;
  float scale = 1.0;
  float aspect_ratio = 16.0f / 9.0f;
  std::string video_driver;
};


struct Debug_data
{
  bool show_ui = false;
  bool noclip = false;
  bool wireframe = false;
  bool show_axes = false;
  bool show_grid = false;
};


struct App_data
{
  App_state state = App_state::Menu;
  Timing_data timing;
  Window_data window;
  Debug_data debug;
};


}  // namespace wis


#endif  // WIS_APP_APP_DATA_H
