#ifndef WIS_GAME_RANGE_FINDER_H
#define WIS_GAME_RANGE_FINDER_H


#include <cstdint>
#include <span>
#include <vector>

#include "game/cards.h"
#include "game/tile.h"


namespace wis {


class Range_finder
{
public:
  bool find(std::span<const Tile> tiles, std::uint32_t cols, std::uint32_t index, Card card);
  void clear() { range_.clear(); };
  bool has_range() const { return !range_.empty(); }
  std::span<const std::uint32_t> range() const { return range_; }

private:
  std::vector<std::uint32_t> range_;
};


}  // namespace wis


#endif  // WIS_GAME_RANGE_FINDER_H
