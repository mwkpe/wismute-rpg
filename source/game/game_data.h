#ifndef WIS_GAME_GAME_DATA_H
#define WIS_GAME_GAME_DATA_H


#include <cstdint>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "core/color_ramp.h"
#include "game/types.h"


namespace wis {


struct Stage_cursor_data
{
  std::uint32_t scene_index = 0u;
  std::uint32_t map_index = 0u;
  glm::uvec2 scene_coords = glm::uvec2{0};
  glm::vec3 scene_position = glm::vec3{0.0f};
  glm::vec3 ground_position = glm::vec3{0.0f};
};

struct Ui_cursor_data
{
  glm::vec3 screen_position = glm::vec3{0.0f};
  glm::vec2 panel_position = glm::vec2{-1.0f, -1.0f};
  bool on_panel = false;
};

struct Cursor_data
{
  Stage_cursor_data stage;
  Ui_cursor_data ui;
  Cursor_type type = Cursor_type::White;
};

struct Camera_data
{
  bool drag = false;
  float fov = 45.0f;
  float height = 20.0f;
};


struct Render_data
{
  bool shadows = true;
};


struct Color_data
{
  std::vector<Color_ramp> ramps;
  std::vector<glm::vec4> palette;
  bool live_update_palette = false;
};

struct Control_data
{
  bool use_orbit_camera;
  float sensitivity = 0.025f;
};

struct Stage_data
{
  std::uint32_t hovered_index = 0u;
  std::uint32_t selected_index = 0u;
  std::map<Element, bool> amplification;
};


struct Stats_data
{
  std::uint32_t stage_draw_calls = 0u;
  std::uint32_t ui_draw_calls = 0u;
};


struct Game_data
{
  Cursor_data cursor;
  Camera_data camera;
  Render_data render;
  Color_data color;
  Control_data control;
  Stage_data stage;
  Stats_data stats;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_DATA_H
