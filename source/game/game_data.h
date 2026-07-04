#ifndef WIS_GAME_GAME_DATA_H
#define WIS_GAME_GAME_DATA_H


#include <cstdint>
#include <glm/glm.hpp>


namespace wis {


struct Cursor_data
{
  std::uint32_t scene_index = 0;
  glm::uvec2 scene_coords = glm::uvec2{0};
  glm::vec3 scene_position = glm::vec3{0.0f};
  glm::vec3 ground_position = glm::vec3{0.0f};
};


struct Camera_data
{
  bool drag = false;
  float fov = 45.0f;
  float cur_zoom = 0.0f;
  const float min_zoom = -6.0f;
  const float max_zoom = 12.0f;
};


struct Stage_data
{
  std::uint32_t selected_tile_index = 0;
};


struct Game_data
{
  Cursor_data cursor;
  Camera_data camera;
  Stage_data stage;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_DATA_H
