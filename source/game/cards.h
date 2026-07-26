#ifndef WIS_GAME_CARDS_H
#define WIS_GAME_CARDS_H


#include <cstdint>
#include <variant>
#include "game/types.h"


namespace wis {


struct Move
{
  static constexpr auto element = Element::None;
  static constexpr std::uint32_t mesh_index = 10;
  std::uint8_t id = 0;
  std::uint8_t steps = 0;
  bool was_played = false;
};

struct Fireball
{
  static constexpr auto element = Element::Fire;
  static constexpr std::uint32_t mesh_index = 0;
  static constexpr std::uint32_t damage = 20;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Inferno
{
  static constexpr auto element = Element::Fire;
  static constexpr std::uint32_t mesh_index = 1;
  static constexpr std::uint32_t damage = 10;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Jet
{
  static constexpr auto element = Element::Water;
  static constexpr std::uint32_t mesh_index = 2;
  static constexpr std::uint32_t damage = 5;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Splash
{
  static constexpr auto element = Element::Water;
  static constexpr std::uint32_t mesh_index = 3;
  static constexpr std::uint32_t primary_damage = 20;
  static constexpr std::uint32_t secondary_damage = 10;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Lightning
{
  static constexpr auto element = Element::Wind;
  static constexpr std::uint32_t mesh_index = 4;
  static constexpr std::uint32_t primary_damage = 20;
  static constexpr std::uint32_t secondary_damage = 10;
  static constexpr std::uint32_t forks = 3;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Gust
{
  static constexpr auto element = Element::Wind;
  static constexpr std::uint32_t mesh_index = 5;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Missile
{
  static constexpr auto element = Element::Aether;
  static constexpr std::uint32_t mesh_index = 6;
  static constexpr std::uint32_t damage = 10;
  static constexpr std::uint32_t projectiles = 3;
  std::uint8_t id = 0;
  bool was_played = false;
};

struct Teleport
{
  static constexpr auto element = Element::Aether;
  static constexpr std::uint32_t mesh_index = 7;
  std::uint8_t id = 0;
  bool was_played = false;
};


using Card = std::variant<Move,
    Fireball,
    Inferno,
    Jet,
    Splash,
    Lightning,
    Gust,
    Missile,
    Teleport>;


}  // namespace wis


#endif  // WIS_GAME_CARDS_H
