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


void find_inferno_range(const auto& scene, std::uint32_t index, auto& full_range)
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
    if (const auto* tile = get_tile(scene.tiles(), i); !tile->is_nil &&
        tile->element != wis::Element::Water) {
      full_range.push_back(i);
    }
  }
}


void find_jet_range(const auto& scene, std::uint32_t index, auto& full_range)
{
  // Pattern
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

  auto add = [&scene, &full_range](std::span<const std::uint32_t> indices) {
    for (auto i : indices) {
      const auto* tile = get_tile(scene.tiles(), i);
      const auto* slime = get_slime(scene.slimes(), i);
      bool not_wall = !tile->is_wall || tile->element == wis::Element::Water;

      if (!tile->is_nil && not_wall && !slime) {
        full_range.push_back(i);
      }
      else if (slime) {
        full_range.push_back(i);
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


bool wis::Range_finder::find(const Scene& scene, std::uint32_t index, Card card)
{
  clear();

  if (auto* tile = get_tile(scene.tiles(), index); tile->is_nil) {
    return false;
  }

  std::visit(util::match{
      [&](Move) {},
      [&](Fireball) { find_fireball_range(scene, index, full_range_, valid_range_); },
      [&](Inferno) { find_inferno_range(scene, index, full_range_); },
      [&](Jet) { find_jet_range(scene, index, full_range_); },
      [&](Splash) { find_splash_range(scene, index, full_range_); },
      [&](Lightning) {},
      [&](Gust) {},
      [&](Missile) { find_missile_range(scene, index, full_range_); },
      [&](Teleport) { find_teleport_range(scene, index, full_range_, valid_range_); }
  }, card);

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
