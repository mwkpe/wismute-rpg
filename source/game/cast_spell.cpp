#include "cast_spell.h"


#include <algorithm>
#include <cmath>
#include <memory>
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


void cast_fireball(auto fireball, std::span<const wis::Tile> tiles, std::span<wis::Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::uint32_t spell_damage = fireball.damage;

  if (is_amplified(tiles, player_index, fireball.element)) {
    spell_damage *= 2u;
  }

  if (auto* slime = get_slime(slimes, target_index); slime) {
    auto damage = std::min(slime->health, spell_damage);
    slime->health -= damage;
  }
}


void cast_inferno(auto inferno, std::span<const wis::Tile> tiles, std::span<wis::Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::uint32_t spell_damage = inferno.damage;

  if (is_amplified(tiles, player_index, inferno.element)) {
    spell_damage *= 2u;
  }

  if (auto* slime = get_slime(slimes, target_index); slime) {
    auto damage = std::min(slime->health, spell_damage);
    slime->health -= damage;
  }

  for (const auto i : tiles[target_index].cardinals()) {
    if (auto* slime = get_slime(slimes, i); slime) {
      auto damage = std::min(slime->health, spell_damage);
      slime->health -= damage;
    }
  }
}


void cast_jet(auto jet, std::span<const wis::Tile> tiles, std::span<wis::Slime> slimes,
    std::uint32_t player_index, std::uint32_t target_index)
{
  std::uint32_t spell_damage = jet.damage;

  if (is_amplified(tiles, player_index, jet.element)) {
    spell_damage *= 2u;
  }

  if (auto* slime = get_slime(slimes, target_index); slime) {
    auto damage = std::min(slime->health, spell_damage);
    slime->health -= damage;
  }
}


void cast_blink(wis::Player& player, std::uint32_t target_index)
{
  player.scene_index = target_index;
}


void cast_teleport(wis::Player& player, std::uint32_t target_index)
{
  player.scene_index = target_index;
}


}  // namespace


void wis::cast_spell(Spell spell, Player& player, std::span<const Tile> tiles,
    std::span<Slime> slimes, std::uint32_t target_index)
{
  std::visit(util::match{
      [&](Fireball fireball) {
        cast_fireball(fireball, tiles, slimes, player.scene_index, target_index);
      },
      [&](Inferno inferno) {
        cast_inferno(inferno, tiles, slimes, player.scene_index, target_index);
      },
      [&](Jet jet) {
        cast_jet(jet, tiles, slimes, player.scene_index, target_index);
      },
      [&](Splash) {},
      [&](Lightning) {},
      [&](Gust) {},
      [&](Missile) {},
      [&](Blink) {
        cast_blink(player, target_index);
      },
      [&](Teleport) {
        cast_teleport(player, target_index);
      }
  }, spell);
}
