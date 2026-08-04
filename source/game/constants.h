#ifndef WIS_GAME_CONSTANTS_H
#define WIS_GAME_CONSTANTS_H


#include <cstdint>
#include <glm/glm.hpp>


namespace wis::val {


constexpr float pixel_size() { return 0.1f; }
constexpr float tile_size() { return 2.0f; }
constexpr std::uint32_t sprite_size() { return 20u; }
constexpr float tile_size_ui() { return 2.4f; }
constexpr std::uint32_t sprite_size_ui() { return 24u; }

constexpr glm::vec3 sprite_offset{0.0f, 0.0f, 0.3f};


}  // namespace wis


#endif  // WIS_GAME_CONSTANTS_H
