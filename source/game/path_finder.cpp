#include "path_finder.h"


#include <algorithm>
#include <array>


bool wis::Path_finder::search(std::span<const Tile> tiles, std::uint32_t origin,
    std::uint32_t target)
{
  path_.clear();
  path_.reserve(tiles.size());

  if (origin == target) {
    return false;
  }

  std::vector<std::uint32_t> came_from;
  came_from.assign(tiles.size(), 0u);

  came_from[origin] = origin;

  std::vector<std::uint32_t> queue;
  queue.reserve(tiles.size());
  queue.push_back(origin);

  std::size_t head = 0u;

  while (head < queue.size()) {
    const Tile& tile = tiles[queue[head++]];

    const std::array<std::uint32_t, 4> neighbors = {
        tile.north_index(),
        tile.south_index(),
        tile.east_index(),
        tile.west_index()
    };

    for (const auto next : neighbors) {
      // The nil tile at index 0 is not ground, this catches missing neighbors
      if (!tiles[next].is_free() || came_from[next] != 0u) {
        continue;
      }

      came_from[next] = tile.index;

      if (next == target) {
        // Construct path by walking back
        for (std::uint32_t i=target; i!=origin; i=came_from[i]) {
          path_.push_back(i);
        }

        std::ranges::reverse(path_);

        return true;
      }

      queue.push_back(next);
    }
  }

  return false;
}


std::tuple<std::uint32_t, std::uint32_t> wis::Path_finder::endpoints() const
{
  if (has_path()) {
    return {path_.front(), path_.back()};
  }

  return {0, 0};
}
