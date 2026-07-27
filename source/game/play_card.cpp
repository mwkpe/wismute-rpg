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


void play(const wis::Move move, wis::Player& player, std::span<const wis::Tile> tiles,
    std::span<wis::Slime> slimes, std::uint32_t target_index)
{
  if (auto* slime = get_slime(slimes, target_index); !tiles[target_index].is_wall && !slime) {
    player.scene_index = target_index;
  }
  else {
    std::print("Can't move there\n");
  }
}


void play(wis::Fireball fireball, std::span<const wis::Tile> tiles, std::span<wis::Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::uint32_t damage = fireball.damage;

  if (is_amplified(tiles, player_index, fireball.element)) {
    damage *= 2u;
  }

  if (auto* slime = get_slime(slimes, target_index); slime) {
    damage = std::min(slime->health, damage);
    slime->health -= damage;

    std::print("Slime {} received {} fire damage\n", slime->id, damage);
  }
  else {
    std::print("Nothing hit\n");
  }
}


}  // namespace


void wis::play_card(Card card, Player& player, std::span<const Tile> tiles,
    std::span<Slime> slimes, std::uint32_t target_index)
{
  std::visit(util::match{
      [&](Move move) {
        play(move, player, tiles, slimes, target_index);
      },
      [&](Fireball fireball) {
        play(fireball, tiles, slimes, player.scene_index, target_index);
      },
      [&](Inferno) { std::print("Inferno not implemented\n"); },
      [&](Jet) { std::print("Jet not implemented\n"); },
      [&](Splash) { std::print("Splash not implemented\n"); },
      [&](Lightning) { std::print("Lightning not implemented\n"); },
      [&](Gust) { std::print("Gust not implemented\n"); },
      [&](Missile) { std::print("Missile not implemented\n"); },
      [&](Teleport) { std::print("Teleport not implemented\n"); }
  }, card);
}
