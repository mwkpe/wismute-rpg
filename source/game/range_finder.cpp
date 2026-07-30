#include "range_finder.h"


#include <algorithm>
#include <array>
#include <ranges>
#include "util/utility.h"


namespace {


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


void find_fireball_range(const auto& scene, std::uint32_t index, auto& full_range,
    auto& valid_range)
{
  // Pattern
  //  xxx
  // xxxxx
  // xxoxx
  // xxxxx
  //  xxx

  const auto cols = scene.size().x;

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
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil) {
      full_range.push_back(i);
    }

    if (get_slime(scene.slimes(), i)) {
      valid_range.push_back(i);
    }
  }
}


void find_inferno_range(const auto& scene, std::uint32_t index, auto& full_range,
    auto& valid_range)
{
  // Pattern
  //  xxx
  // xx xx
  // x o x
  // xx xx
  //  xxx

  const auto cols = scene.size().x;

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
    const auto* tile = get_tile(scene.tiles(), i);

    if (!tile->is_nil) {
      full_range.push_back(i);
    }

    if (!tile->is_wall) {
      // Slime on tile?
      if (get_slime(scene.slimes(), i)) {
        valid_range.push_back(i);
      }
      else {
        // Slime in any of the cardinal directions?
        for (auto j : tile->cardinals()) {
          if (get_slime(scene.slimes(), j)) {
            valid_range.push_back(i);
            break;
          }
        }
      }
    }
  }
}


void find_jet_range(const auto& scene, std::uint32_t index, auto& full_range,
    auto& valid_range)
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

  auto add = [&scene, &full_range, &valid_range](std::span<const std::uint32_t> indices) {
    for (auto i : indices) {
      const auto* tile = get_tile(scene.tiles(), i);
      const auto* slime = get_slime(scene.slimes(), i);
      bool is_wall = tile->is_wall && tile->element != wis::Element::Water;

      if (!tile->is_nil && !is_wall && !slime) {
        full_range.push_back(i);
      }
      else if (slime) {
        full_range.push_back(i);
        valid_range.push_back(i);
        return;
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


void find_splash_range(const auto& scene, std::uint32_t index, auto& full_range)
{
  // Pattern
  //  xxx
  // xx xx
  // x o x
  // xx xx
  //  xxx

  const auto cols = scene.size().x;

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
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil) {
      full_range.push_back(i);
    }
  }
}


void find_missile_range(const auto& scene, std::uint32_t index, auto& full_range)
{
  // Pattern
  //  xxx
  // xxxxx
  // xxoxx
  // xxxxx
  //  xxx

  const auto cols = scene.size().x;

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
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil) {
      full_range.push_back(i);
    }
  }
}


void find_blink_range(const wis::Blink& blink, const auto& scene, std::uint32_t index,
    auto& full_range, auto& valid_range)
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

  std::array<std::uint32_t, 20> indices{0};
  std::size_t i = 0;

  if (blink.steps == 0 || blink.steps > 5) {
    return;
  }

  switch (blink.steps) {
    case 5: {
      indices[i++] = index - cols * 5u;
      indices[i++] = index + cols * 5u;
      indices[i++] = index - 5u;
      indices[i++] = index + 5u;
    }
    [[fallthrough]];
    case 4: {
      indices[i++] = index - cols * 4u;
      indices[i++] = index + cols * 4u;
      indices[i++] = index - 4u;
      indices[i++] = index + 4u;
    }
    [[fallthrough]];
    case 3: {
      indices[i++] = index - cols * 3u;
      indices[i++] = index + cols * 3u;
      indices[i++] = index - 3u;
      indices[i++] = index + 3u;
    }
    [[fallthrough]];
    case 2: {
      indices[i++] = index - cols * 2u;
      indices[i++] = index + cols * 2u;
      indices[i++] = index - 2u;
      indices[i++] = index + 2u;
    }
    [[fallthrough]];
    case 1: {
      indices[i++] = index - cols;
      indices[i++] = index + cols;
      indices[i++] = index - 1u;
      indices[i++] = index + 1u;
    }
    break;
    default: return;
  }

  for (auto i : indices) {
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil) {
      full_range.push_back(i);
    }
  }

  for (auto i : std::span{indices}.first(4)) {
    const auto* tile = get_tile(scene.tiles(), i);

    if (!tile->is_nil && !tile->is_wall && !get_slime(scene.slimes(), i)) {
      valid_range.push_back(i);
    }
  }
}


void find_teleport_range(const auto& scene, std::uint32_t index, auto& full_range,
    auto& valid_range)
{
  // No pattern, global reach

  for (const auto& tile : scene.tiles() | not_nil) {
    full_range.push_back(tile.index);

    if (tile.index != index && !tile.is_wall && !get_slime(scene.slimes(), tile.index)) {
      valid_range.push_back(tile.index);
    }
  }
}


}  // namespace


bool wis::Range_finder::find(Spell spell, const Scene& scene, std::uint32_t index)
{
  clear();

  if (auto* tile = get_tile(scene.tiles(), index); tile->is_nil) {
    return false;
  }

  std::visit(util::match{
      [&](Fireball) { find_fireball_range(scene, index, full_range_, valid_range_); },
      [&](Inferno) { find_inferno_range(scene, index, full_range_, valid_range_); },
      [&](Jet) { find_jet_range(scene, index, full_range_, valid_range_); },
      [&](Splash) { find_splash_range(scene, index, full_range_); },
      [&](Lightning) {},
      [&](Gust) {},
      [&](Missile) { find_missile_range(scene, index, full_range_); },
      [&](Blink blink) { find_blink_range(blink, scene, index, full_range_, valid_range_); },
      [&](Teleport) { find_teleport_range(scene, index, full_range_, valid_range_); }
  }, spell);

  if (full_range_.empty()) {
    return false;
  }

  return true;
}


bool wis::Range_finder::within_full_range(std::uint32_t index) const
{
  return std::ranges::contains(full_range_, index);
}


bool wis::Range_finder::within_valid_range(std::uint32_t index) const
{
  return std::ranges::contains(valid_range_, index);
}
