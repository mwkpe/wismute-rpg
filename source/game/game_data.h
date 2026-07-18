#ifndef WIS_GAME_GAME_DATA_H
#define WIS_GAME_GAME_DATA_H


#include <cstdint>
#include <glm/glm.hpp>


namespace wis {


struct Stage_cursor_data
{
  std::uint32_t scene_index = 0;
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
};


struct Camera_data
{
  bool drag = false;
  float fov = 45.0f;
  float height = 24.0f;
};


struct Stage_data
{
  std::uint32_t selected_scene_index = 0;
};


struct Control_data
{
  bool use_orbit_camera;
  float sensitivity = 0.025f;
};


struct Game_data
{
  Cursor_data cursor;
  Camera_data camera;
  Control_data control;
  Stage_data stage;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_DATA_H
