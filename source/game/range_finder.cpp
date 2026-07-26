#include "range_finder.h"


#include <algorithm>
#include <array>
#include "util/utility.h"


namespace {


auto get_tile(std::span<const wis::Tile> tiles, std::uint32_t index) -> const wis::Tile*
{
  if (index >= tiles.size()) {
    return &tiles[0];  // nil tile
  }

  return &tiles[index];
}


void find_fireball(auto tiles, std::uint32_t cols, std::uint32_t index, auto& range)
{
  // Pattern
  //  xxx
  // xxxxx
  // xxoxx
  // xxxxx
  //  xxx

  const std::array<std::uint32_t, 20> indices{
      index - cols * 2u - 1u,
      index - cols * 2u,
      index - cols * 2u + 1u,

      index - cols - 2u,
      index - cols - 1u,
      index - cols,
      index - cols + 1u,
      index - cols + 2u,

      index - 2u,
      index - 1u,
      index + 1u,
      index + 2u,

      index + cols - 2u,
      index + cols - 1u,
      index + cols,
      index + cols + 1u,
      index + cols + 2u,

      index + cols * 2u - 1u,
      index + cols * 2u,
      index + cols * 2u + 1u
  };

  for (auto i : indices) {
    if (const auto* tile = get_tile(tiles, i); !tile->is_nil) {
      range.push_back(i);
    }
  }
}


void find_inferno(auto tiles, std::uint32_t cols, std::uint32_t index, auto& range)
{
  // Pattern
  //  xxx
  // xx xx
  // x o x
  // xx xx
  //  xxx

  const std::array<std::uint32_t, 20> indices{
      index - cols * 2u - 1u,
      index - cols * 2u,
      index - cols * 2u + 1u,

      index - cols - 2u,
      index - cols - 1u,
      index - cols + 1u,
      index - cols + 2u,

      index - 2u,
      index + 2u,

      index + cols - 2u,
      index + cols - 1u,
      index + cols + 1u,
      index + cols + 2u,

      index + cols * 2u - 1u,
      index + cols * 2u,
      index + cols * 2u + 1u
  };

  for (auto i : indices) {
    if (const auto* tile = get_tile(tiles, i); !tile->is_nil) {
      range.push_back(i);
    }
  }
}


void find_jet(auto tiles, std::uint32_t cols, std::uint32_t index, auto& range)
{
  // Pattern
  //    x
  //    x
  //    x
  // xxxoxxx
  //    x
  //    x
  //    x

  const std::array up_indices{ index - cols, index - cols * 2u, index - cols * 3u };
  const std::array down_indices{ index + cols, index + cols * 2u, index + cols * 3u };
  const std::array left_indices{ index - 1u, index - 2u, index - 3u };
  const std::array right_indices{ index + 1u, index + 2u, index + 3u, };

  auto add = [&tiles, &range](std::span<const std::uint32_t> indices) {
    for (auto i : indices) {
      if (const auto* tile = get_tile(tiles, i); !tile->is_nil &&
          (!tile->is_wall || tile->element == wis::Element::Water)) {
        range.push_back(i);
      }
      else {
        return;
      }
    }
  };

  add(up_indices);
  add(down_indices);
  add(left_indices);
  add(right_indices);
}


void find_splash(auto tiles, std::uint32_t cols, std::uint32_t index, auto& range)
{
  // Pattern
  //  xxx
  // xx xx
  // x o x
  // xx xx
  //  xxx

  const std::array<std::uint32_t, 20> indices{
      index - cols * 2u - 1u,
      index - cols * 2u,
      index - cols * 2u + 1u,

      index - cols - 2u,
      index - cols - 1u,
      index - cols + 1u,
      index - cols + 2u,

      index - 2u,
      index + 2u,

      index + cols - 2u,
      index + cols - 1u,
      index + cols + 1u,
      index + cols + 2u,

      index + cols * 2u - 1u,
      index + cols * 2u,
      index + cols * 2u + 1u
  };

  for (auto i : indices) {
    if (const auto* tile = get_tile(tiles, i); !tile->is_nil) {
      range.push_back(i);
    }
  }
}


void find_missile(auto tiles, std::uint32_t cols, std::uint32_t index, auto& range)
{
  // Pattern
  //  xxx
  // xxxxx
  // xxoxx
  // xxxxx
  //  xxx

  const std::array<std::uint32_t, 20> indices{
      index - cols * 2u - 1u,
      index - cols * 2u,
      index - cols * 2u + 1u,

      index - cols - 2u,
      index - cols - 1u,
      index - cols,
      index - cols + 1u,
      index - cols + 2u,

      index - 2u,
      index - 1u,
      index + 1u,
      index + 2u,

      index + cols - 2u,
      index + cols - 1u,
      index + cols,
      index + cols + 1u,
      index + cols + 2u,

      index + cols * 2u - 1u,
      index + cols * 2u,
      index + cols * 2u + 1u
  };

  for (auto i : indices) {
    if (const auto* tile = get_tile(tiles, i); !tile->is_nil) {
      range.push_back(i);
    }
  }
}


void find_teleport(auto tiles, std::uint32_t index, auto& range)
{
  // No pattern, global reach

  for (const auto& tile : tiles) {
    if (!tile.is_nil && !tile.is_wall && tile.index != index) {
      range.push_back(tile.index);
    }
  }
}


}  // namespace


bool wis::Range_finder::find(std::span<const Tile> tiles, std::uint32_t cols, std::uint32_t index,
    Card card)
{
  range_.clear();

  if (auto* tile = get_tile(tiles, index); tile->is_nil) {
    return false;
  }

  std::visit(util::match{
      [&](Move) {},
      [&](Fireball) { find_fireball(tiles, cols, index, range_); },
      [&](Inferno){ find_inferno(tiles, cols, index, range_); },
      [&](Jet){ find_jet(tiles, cols, index, range_); },
      [&](Splash){ find_splash(tiles, cols, index, range_); },
      [&](Lightning){},
      [&](Gust){},
      [&](Missile){ find_missile(tiles, cols, index, range_); },
      [&](Teleport){ find_teleport(tiles, index, range_); }
  }, card);

  if (range_.empty()) {
    return false;
  }

  return true;
}
