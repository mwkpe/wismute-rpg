#ifndef WIS_GAME_SLIME_H
#define WIS_GAME_SLIME_H


#include <cstdint>


namespace wis {


struct Slime
{
  std::uint32_t id = 0;
  std::uint32_t health = 0;
  std::uint32_t weight = 0;
  glm::vec3 position = glm::vec3{0.0f};
  std::uint32_t scene_index = 0;
  std::uint32_t mesh_index = 0;
  float breathe_amplitude = 0.0f;
  float breathe_speed = 0.0f;
  float breathe_phase = 0.0f;
};


}  // namespace wis


#endif  // WIS_GAME_SLIME_H
