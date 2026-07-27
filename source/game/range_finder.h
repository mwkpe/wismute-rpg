#ifndef WIS_GAME_RANGE_FINDER_H
#define WIS_GAME_RANGE_FINDER_H


#include <cstdint>
#include <span>
#include <vector>

#include "game/scene.h"
#include "game/tile.h"


namespace wis {


class Range_finder
{
public:
  bool find(const Scene& scene, std::uint32_t index, Card card);
  void clear() { range_.clear(); };
  //bool has_range() const { return !range_.empty(); }
  std::span<const std::uint32_t> range() const { return range_; }

private:
  std::vector<std::uint32_t> range_;
  std::vector<std::uint32_t> valid_range_;
  std::vector<std::uint32_t> targets_;
};


}  // namespace wis


#endif  // WIS_GAME_RANGE_FINDER_H
