#ifndef WIS_GAME_CONSTANTS_H
#define WIS_GAME_CONSTANTS_H


#include <cstdint>


namespace wis::constants {


constexpr float pixel_size() { return 0.1f; }
constexpr float tile_size() { return 2.0f; }
constexpr std::uint32_t sprite_size() { return 20u; }
constexpr float tile_size_ui() { return 2.4f; }
constexpr std::uint32_t sprite_size_ui() { return 24u; }


}  // namespace wis


#endif  // WIS_GAME_CONSTANTS_H
