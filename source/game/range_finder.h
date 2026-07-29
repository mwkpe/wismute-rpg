#ifndef WIS_GAME_RANGE_FINDER_H
#define WIS_GAME_RANGE_FINDER_H


#include <cstdint>
#include <span>
#include <vector>

#include "game/scene.h"
#include "game/spells.h"
#include "game/tile.h"


namespace wis {


class Range_finder
{
public:
  bool find(Spell spell, const Scene& scene, std::uint32_t index);
  void clear() { full_range_.clear(); valid_range_.clear(); };

  bool within_full_range(std::uint32_t index) const;
  bool within_valid_range(std::uint32_t index) const;

  std::span<const std::uint32_t> full_range() const { return full_range_; }
  std::span<const std::uint32_t> valid_range() const { return valid_range_; }

private:
  std::vector<std::uint32_t> full_range_;
  std::vector<std::uint32_t> valid_range_;
};


}  // namespace wis


#endif  // WIS_GAME_RANGE_FINDER_H
