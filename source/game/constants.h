#ifndef WIS_GAME_CONSTANTS_H
#define WIS_GAME_CONSTANTS_H


#include <cstdint>
#include <glm/glm.hpp>


namespace wis::cval {


constexpr float pixel_size = 0.1f;

constexpr float tile_size = 2.0f;
constexpr std::uint32_t sprite_size = 20;

constexpr float tile_size_ui = 2.4f;
constexpr std::uint32_t sprite_size_ui = 24;

constexpr glm::vec3 sprite_offset = {0.0f, 0.0f, 0.3f};


}  // namespace wis::cval


#endif  // WIS_GAME_CONSTANTS_H
