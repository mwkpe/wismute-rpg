#ifndef WIS_GAME_SPRITE_H
#define WIS_GAME_SPRITE_H



#include <cstdint>


namespace wis {


struct Sprite
{
  glm::vec3 position;
  std::uint32_t scene_index = 0;
  glm::uvec2 scene_coords = glm::uvec2{0};
  std::uint32_t mesh_index = 0;
};


}  // namespace wis


#endif  // WIS_GAME_SPRITE_H
