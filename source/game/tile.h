#ifndef WIS_GAME_TILE_H
#define WIS_GAME_TILE_H


#include <bitset>
#include <cstdint>
#include "game/types.h"


namespace wis {


struct Tile
{
  void set_nil(bool b = true) { flags.set(0u, b); }
  void set_marked(bool b = true) { flags.set(1u, b); }
  void set_void(bool b = true) { flags.set(2u, b); }
  void set_ground(bool b = true) { flags.set(3u, b); }

  [[nodiscard]] bool is_nil() const { return flags.test(0u); }  // Null
  [[nodiscard]] bool is_marked() const { return flags.test(1u); }  // Algorithm use
  [[nodiscard]] bool is_void() const { return flags.test(2u); }  // No sprite or functionality
  [[nodiscard]] bool is_ground() const { return flags.test(3u); }  // Playable tile

  std::uint32_t index = 0u;
  std::uint32_t row = 0u;
  std::uint32_t col = 0u;
  std::uint32_t mesh_index = 0u;

  std::bitset<32> flags = 0b0000'0000;

  // A zero-index means there is no neighbor (never use tile 0, always set it to nil)
  std::uint32_t north_index = 0u;
  std::uint32_t south_index = 0u;
  std::uint32_t west_index = 0u;
  std::uint32_t east_index = 0u;

  Element element = Element::None;
};


}  // namespace wis


#endif  // WIS_GAME_TILE_H
