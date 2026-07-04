#ifndef WIS_GAME_CONSTANTS_H
#define WIS_GAME_CONSTANTS_H


#include <cstdint>


namespace wis {


constexpr float pixel_size() { return 0.1f; }
constexpr float tile_size() { return 1.6f; }
constexpr std::uint32_t sprite_size() { return 16u; }


}  // namespace wis


#endif  // WIS_GAME_CONSTANTS_H
