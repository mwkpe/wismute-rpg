#include "range_finder.h"


#include <algorithm>
#include <array>
#include <ranges>
#include "util/utility.h"


namespace {


struct Range_tiles
{
  std::vector<std::uint32_t>& target;
  std::vector<std::uint32_t>& invalid;
  std::vector<std::uint32_t>& empty;
  std::vector<std::uint32_t>& blocked;
};


constexpr auto not_nil = std::views::filter([](const auto& e) { return !e.is_nil; });
constexpr auto not_wall = std::views::filter([](const auto& e) { return !e.is_wall; });


auto get_tile(auto tiles, std::uint32_t index) -> const wis::Tile*
{
  if (index >= tiles.size()) {
    return &tiles[0];  // nil tile
  }

  return &tiles[index];
}


const wis::Slime* get_slime(auto slimes, std::uint32_t target_index)
{
  auto it = std::ranges::find(slimes, target_index, &wis::Slime::scene_index);

  if (it != slimes.end() && it->health > 0) {
    return std::to_address(it);
  }

  return nullptr;
}


void find_fireball_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
  // Pattern
  //  xxx
  // xxxxx
  // xxoxx
  // xxxxx
  //  xxx

  const auto cols = scene.size().x;

  const std::array indices{
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
    const auto* tile = get_tile(scene.tiles(), i);

    if (get_slime(scene.slimes(), i)) {
      range_tiles.target.push_back(i);
    }
    else if (!tile->is_nil && !tile->is_wall) {
      range_tiles.empty.push_back(i);
    }
    else if (!tile->is_nil && tile->is_wall) {
      range_tiles.invalid.push_back(i);
    }
  }
}


void find_inferno_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
  // Pattern
  //  xxx
  // xx xx
  // x o x
  // xx xx
  //  xxx

  const auto cols = scene.size().x;

  const std::array indices{
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

  const std::array cardinals{
      index - cols,
      index - 1u,
      index + 1u,
      index + cols
  };

  auto slime_on_cardinal = [&scene](std::uint32_t index){
    for (auto i : get_tile(scene.tiles(), index)->cardinals()) {
      if (get_slime(scene.slimes(), i)) {
        return true;
      }
    }

    return false;
  };

  for (auto i : indices) {
    const auto* tile = get_tile(scene.tiles(), i);

    const bool hits_slime = get_slime(scene.slimes(), i) != nullptr || slime_on_cardinal(i);
    const bool is_cardinal = std::ranges::find(cardinals, i) != cardinals.end();

    if (!tile->is_nil) {
      if (!tile->is_wall && hits_slime && !is_cardinal) {
        range_tiles.target.push_back(i);
      }
      else if (tile->is_wall || is_cardinal) {
        range_tiles.invalid.push_back(i);
      }
      else {
        range_tiles.empty.push_back(i);
      }
    }
  }

  for (auto i : cardinals) {
    range_tiles.invalid.push_back(i);
  }
}


void find_jet_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
  // Pattern (reach may be blocked by walls)
  //    x
  //    x
  //    x
  // xxxoxxx
  //    x
  //    x
  //    x

  const auto cols = scene.size().x;

  const std::array up_indices{ index - cols, index - cols * 2u, index - cols * 3u };
  const std::array down_indices{ index + cols, index + cols * 2u, index + cols * 3u };
  const std::array left_indices{ index - 1u, index - 2u, index - 3u };
  const std::array right_indices{ index + 1u, index + 2u, index + 3u, };

  auto add = [&scene, &range_tiles](std::span<const std::uint32_t> indices) {
    for (auto i : indices) {
      const auto* tile = get_tile(scene.tiles(), i);

      const bool is_wall = tile->is_wall && tile->element != wis::Element::Water;
      const bool has_slime = get_slime(scene.slimes(), i) != nullptr;

      if (!tile->is_nil && !is_wall && !has_slime) {
        range_tiles.empty.push_back(i);
      }
      else if (!tile->is_nil && has_slime) {
        range_tiles.target.push_back(i);
        return;
      }
      else if (!tile->is_nil && is_wall) {
        range_tiles.invalid.push_back(i);
        return;
      }
    }
  };

  add(up_indices);
  add(down_indices);
  add(left_indices);
  add(right_indices);
}


void find_splash_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
}


void find_missile_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
}


void find_blink_range(const wis::Blink& blink, const auto& scene, std::uint32_t index,
    auto& range_tiles)
{
  // Pattern (reach depends on step value)
  //    x
  //    x
  //    x
  // xxxoxxx
  //    x
  //    x
  //    x

  const auto cols = scene.size().x;

  std::array<std::uint32_t, 12> indices{0};
  std::size_t pos = 0;

  if (blink.steps == 0 || blink.steps > 5) {
    return;
  }

  switch (blink.steps) {
    case 5: {
      indices[pos++] = index - cols * 5u;
      indices[pos++] = index + cols * 5u;
      indices[pos++] = index - 5u;
      indices[pos++] = index + 5u;
    }
    [[fallthrough]];
    case 4: {
      indices[pos++] = index - cols * 4u;
      indices[pos++] = index + cols * 4u;
      indices[pos++] = index - 4u;
      indices[pos++] = index + 4u;
    }
    [[fallthrough]];
    case 3: {
      indices[pos++] = index - cols * 3u;
      indices[pos++] = index + cols * 3u;
      indices[pos++] = index - 3u;
      indices[pos++] = index + 3u;
    }
    [[fallthrough]];
    case 2: {
      indices[pos++] = index - cols * 2u;
      indices[pos++] = index + cols * 2u;
      indices[pos++] = index - 2u;
      indices[pos++] = index + 2u;
    }
    [[fallthrough]];
    case 1: {
      indices[pos++] = index - cols;
      indices[pos++] = index + cols;
      indices[pos++] = index - 1u;
      indices[pos++] = index + 1u;
    }
    break;
    default: return;
  }

  for (auto i : std::span{indices}.subspan(4)) {
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil) {
      range_tiles.invalid.push_back(i);
    }
  }

  for (auto i : std::span{indices}.first(4)) {
    const auto* tile = get_tile(scene.tiles(), i);

    if (!tile->is_nil && !tile->is_wall && !get_slime(scene.slimes(), i)) {
      range_tiles.target.push_back(i);
    }
    else if (!tile->is_nil) {
      range_tiles.blocked.push_back(i);
    }
  }
}


void find_teleport_range(const auto& scene, std::uint32_t index, auto& range_tiles)
{
  // No pattern, global reach

  for (const auto& tile : scene.tiles() | not_nil) {
    bool has_slime = get_slime(scene.slimes(), tile.index) != nullptr;

    if (tile.index != index && !tile.is_wall && !has_slime) {
      range_tiles.target.push_back(tile.index);
    }
    else if (tile.index == index || tile.is_wall || has_slime) {
      range_tiles.invalid.push_back(tile.index);
    }
    else {
      range_tiles.empty.push_back(tile.index);
    }
  }
}


}  // namespace


void wis::Range_finder::find(Spell spell, const Scene& scene, std::uint32_t index)
{
  clear();

  if (auto* tile = get_tile(scene.tiles(), index); tile->is_nil) {
    return;
  }

  Range_tiles range_tiles{target_tiles_, invalid_tiles_, empty_tiles_, blocked_tiles_};

  std::visit(util::match{
      [&](Fireball) { find_fireball_range(scene, index, range_tiles); },
      [&](Inferno) { find_inferno_range(scene, index, range_tiles); },
      [&](Jet) { find_jet_range(scene, index, range_tiles); },
      [&](Splash) { find_splash_range(scene, index, range_tiles); },
      [&](Lightning) {},
      [&](Gust) {},
      [&](Missile) { find_missile_range(scene, index, range_tiles); },
      [&](Blink blink) { find_blink_range(blink, scene, index, range_tiles); },
      [&](Teleport) { find_teleport_range(scene, index, range_tiles); }
  }, spell);
}


void wis::Range_finder::clear()
{
  target_tiles_.clear();
  empty_tiles_.clear();
  invalid_tiles_.clear();
  blocked_tiles_.clear();
};


bool wis::Range_finder::within_target_tiles(std::uint32_t index) const
{
  return std::ranges::contains(target_tiles_, index);
}
