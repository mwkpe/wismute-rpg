#ifndef WIS_GAME_SPRITE_H
#define WIS_GAME_SPRITE_H



#include <cstdint>


namespace wis {


struct Sprite
{
  std::uint32_t mesh_index = 0;
  glm::vec3 position = glm::vec3{0};
  glm::uvec2 scene_coords = glm::uvec2{0};
};


}  // namespace wis


#endif  // WIS_GAME_SPRITE_H
