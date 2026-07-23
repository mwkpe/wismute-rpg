#ifndef WIS_GAME_TILE_H
#define WIS_GAME_TILE_H


#include <bitset>
#include <cstdint>
#include "game/types.h"


namespace wis {


class Tile
{
public:
  std::uint32_t index = 0;
  std::uint32_t col = 0;
  std::uint32_t row = 0;
  std::uint32_t mesh_index = 0;
  Element element = Element::None;
  bool is_nil = false;
  bool is_wall = true;

  void set_north_index(std::uint32_t index) { neighbors_[0] = index; }
  void set_south_index(std::uint32_t index) { neighbors_[1] = index; }
  void set_west_index(std::uint32_t index) { neighbors_[2] = index; }
  void set_east_index(std::uint32_t index) { neighbors_[3] = index; }
  void set_north_west_index(std::uint32_t index) { neighbors_[4] = index; }
  void set_north_east_index(std::uint32_t index) { neighbors_[5] = index; }
  void set_south_west_index(std::uint32_t index) { neighbors_[6] = index; }
  void set_south_east_index(std::uint32_t index) { neighbors_[7] = index; }

  std::uint32_t north_index() const { return neighbors_[0]; }
  std::uint32_t south_index() const { return neighbors_[1]; }
  std::uint32_t west_index() const { return neighbors_[2]; }
  std::uint32_t east_index() const { return neighbors_[3]; }
  std::uint32_t north_west_index() const { return neighbors_[4]; }
  std::uint32_t north_east_index() const { return neighbors_[5]; }
  std::uint32_t south_west_index() const { return neighbors_[6]; }
  std::uint32_t south_east_index() const { return neighbors_[7]; }

  bool is_free() const { return !is_nil && !is_wall; }

  std::span<const std::uint32_t> neighbors() const { return neighbors_; }
  std::span<const std::uint32_t> cardinals() const { return std::span{neighbors_}.first(4); }

private:
  std::array<std::uint32_t, 8> neighbors_ = { 0 };
};


}  // namespace wis


#endif  // WIS_GAME_TILE_H
