#ifndef WIS_GAME_SPELLS_H
#define WIS_GAME_SPELLS_H


#include <cstdint>
#include <variant>
#include "game/types.h"


namespace wis {


struct Fireball
{
  static constexpr auto element = Element::Fire;
  static constexpr std::uint32_t mesh_index = 0;
  static constexpr std::uint32_t damage = 20;
};

struct Inferno
{
  static constexpr auto element = Element::Fire;
  static constexpr std::uint32_t mesh_index = 1;
  static constexpr std::uint32_t damage = 10;
};

struct Jet
{
  static constexpr auto element = Element::Water;
  static constexpr std::uint32_t mesh_index = 2;
  static constexpr std::uint32_t damage = 5;
};

struct Splash
{
  static constexpr auto element = Element::Water;
  static constexpr std::uint32_t mesh_index = 3;
  static constexpr std::uint32_t primary_damage = 20;
  static constexpr std::uint32_t secondary_damage = 10;
};

struct Lightning
{
  static constexpr auto element = Element::Wind;
  static constexpr std::uint32_t mesh_index = 4;
  static constexpr std::uint32_t primary_damage = 20;
  static constexpr std::uint32_t secondary_damage = 10;
  static constexpr std::uint32_t forks = 3;
};

struct Gust
{
  static constexpr auto element = Element::Wind;
  static constexpr std::uint32_t mesh_index = 5;
};

struct Missile
{
  static constexpr auto element = Element::Aether;
  static constexpr std::uint32_t mesh_index = 6;
  static constexpr std::uint32_t damage = 10;
  static constexpr std::uint32_t projectiles = 3;
};

struct Blink
{
  static constexpr auto element = Element::None;
  static constexpr std::uint32_t mesh_index = 10;
  std::uint8_t steps = 0;
};

struct Teleport
{
  static constexpr auto element = Element::Aether;
  static constexpr std::uint32_t mesh_index = 7;
};


using Spell = std::variant<Fireball,
    Inferno,
    Jet,
    Splash,
    Lightning,
    Gust,
    Missile,
    Blink,
    Teleport>;


struct Spell_slot
{
  Spell spell;
  std::uint8_t id = 0;
  bool is_available = true;
};


}  // namespace wis


#endif  // WIS_GAME_SPELLS_H
