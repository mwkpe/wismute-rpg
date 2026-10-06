#ifndef WIS_GAME_PLAYER_H
#define WIS_GAME_PLAYER_H


#include <cstdint>
#include <glm/glm.hpp>
#include "core/animation.h"
#include "game/types.h"


namespace wis {


struct Player
{
  glm::vec3 position;
  std::uint32_t scene_index = 0;
  std::uint32_t mesh_index = 0;
  Oscillation breathing = {};
  Animation animation;
};


}  // namespace wis


#endif  // WIS_GAME_PLAYER_H
