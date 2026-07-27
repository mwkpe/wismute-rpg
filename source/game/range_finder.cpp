#include "range_finder.h"


#include <algorithm>
#include <array>
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
  return it != slimes.end() ? std::to_address(it) : nullptr;
}


void find_fireball(const auto& scene, std::uint32_t index, auto& range)
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
      range.push_back(i);
    }
  }
}


void find_inferno(const auto& scene, std::uint32_t index, auto& range)
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
      range.push_back(i);
    }
  }
}


void find_jet(const auto& scene, std::uint32_t index, auto& range)
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

  auto add = [&scene, &range](std::span<const std::uint32_t> indices) {
    for (auto i : indices) {
      const auto* tile = get_tile(scene.tiles(), i);
      const auto* slime = get_slime(scene.slimes(), i);
      bool not_wall = !tile->is_wall || tile->element == wis::Element::Water;

      if (!tile->is_nil && not_wall && !slime) {
        range.push_back(i);
      }
      else if (slime) {
        range.push_back(i);
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


void find_splash(const auto& scene, std::uint32_t index, auto& range)
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
      range.push_back(i);
    }
  }
}


void find_missile(const auto& scene, std::uint32_t index, auto& range)
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
      range.push_back(i);
    }
  }
}


void find_teleport(const auto& scene, std::uint32_t index, auto& range)
{
  // No pattern, global reach

  for (const auto& tile : scene.tiles() | not_nil | not_wall) {
    if (tile.index != index && !get_slime(scene.slimes(), tile.index)) {
      range.push_back(tile.index);
    }
  }
}


}  // namespace


bool wis::Range_finder::find(const Scene& scene, std::uint32_t index, Card card)
{
  range_.clear();

  if (auto* tile = get_tile(scene.tiles(), index); tile->is_nil) {
    return false;
  }

  std::visit(util::match{
      [&](Move) {},
      [&](Fireball) { find_fireball(scene, index, range_); },
      [&](Inferno) { find_inferno(scene, index, range_); },
      [&](Jet) { find_jet(scene, index, range_); },
      [&](Splash) { find_splash(scene, index, range_); },
      [&](Lightning) {},
      [&](Gust) {},
      [&](Missile) { find_missile(scene, index, range_); },
      [&](Teleport) { find_teleport(scene, index, range_); }
  }, card);

  if (range_.empty()) {
    return false;
  }

  return true;
}
