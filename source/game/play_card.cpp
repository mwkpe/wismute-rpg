#include "play_card.h"


#include <algorithm>
#include <cmath>
#include <memory>
#include <print>
#include <ranges>
#include "util/utility.h"


namespace {


bool is_amplified(std::span<const wis::Tile> tiles, std::uint32_t index, wis::Element element)
{
  for (const auto i : tiles[index].cardinals()) {
    if (tiles[i].element == element) {
      return true;
    }
  }

  return false;
}


wis::Slime* get_slime(std::span<wis::Slime> slimes, std::uint32_t target_index)
{
  auto it = std::ranges::find(slimes, target_index, &wis::Slime::scene_index);
  return it != slimes.end() ? std::to_address(it) : nullptr;
}


void play(const wis::Fireball card, std::span<const wis::Tile> tiles, std::span<wis::Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::uint32_t damage = card.damage;

  if (is_amplified(tiles, player_index, card.element)) {
    damage *= 2u;
  }

  if (auto* slime = get_slime(slimes, target_index); slime) {
    damage = std::min(slime->health, damage);
    slime->health -= damage;

    std::print("Slime {} received {} fire damage\n", slime->id, damage);
  }
}


}  // namespace


void wis::play_card(const Card card, std::span<const Tile> tiles, std::span<Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::visit(util::match{
      [&](const Move){},
      [&](const Fireball fireball){ play(fireball, tiles, slimes, player_index, target_index); },
      [&](const Inferno){},
      [&](const Jet){},
      [&](const Splash){},
      [&](const Lightning){},
      [&](const Gust){},
      [&](const Missile){},
      [&](const Teleport){}
  }, card);
}
