#ifndef WIS_GAME_PATH_FINDER_H
#define WIS_GAME_PATH_FINDER_H


#include <cstdint>
#include <span>
#include <tuple>
#include <vector>

#include "game/tile.h"


namespace wis {


class Path_finder
{
public:
  bool search(std::span<const Tile> tiles, std::uint32_t origin, std::uint32_t target);
  void clear() { path_.clear(); };
  bool has_path() const { return !path_.empty(); }
  std::span<const std::uint32_t> path() const { return path_; }
  std::tuple<std::uint32_t, std::uint32_t> endpoints() const;

private:
  std::vector<std::uint32_t> path_;
};


}  // namespace wis


#endif  // WIS_GAME_PATH_FINDER_H
