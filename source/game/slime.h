#ifndef WIS_GAME_SLIME_H
#define WIS_GAME_SLIME_H


#include <cstdint>


namespace wis {


struct Slime
{
  glm::vec3 position;
  std::uint32_t scene_index = 0;
  std::uint32_t mesh_index = 0;
  float breathe_amplitude = 0.0f;
  float breathe_speed = 0.0f;
  float breathe_phase = 0.0f;
};


}  // namespace wis


#endif  // WIS_GAME_SLIME_H
