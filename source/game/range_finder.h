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
  void find(Spell spell, const Scene& scene, std::uint32_t index);
  void clear();

  bool within_target_tiles(std::uint32_t index) const;

  std::span<const std::uint32_t> target_tiles() const { return target_tiles_; }
  std::span<const std::uint32_t> invalid_tiles() const { return invalid_tiles_; }
  std::span<const std::uint32_t> empty_tiles() const { return empty_tiles_; }
  std::span<const std::uint32_t> marker_tiles() const { return marker_tiles_; }

private:
  std::vector<std::uint32_t> target_tiles_;
  std::vector<std::uint32_t> invalid_tiles_;
  std::vector<std::uint32_t> empty_tiles_;
  std::vector<std::uint32_t> marker_tiles_;
};


}  // namespace wis


#endif  // WIS_GAME_RANGE_FINDER_H
