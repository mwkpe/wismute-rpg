#ifndef WIS_GAME_CARDS_H
#define WIS_GAME_CARDS_H


#include <cstdint>
#include <variant>
#include "game/types.h"


namespace wis {


struct Move
{
  std::uint8_t steps = 0;
};

struct Fireball
{
  Element element = Element::Fire;
  std::uint8_t damage = 20;
};

struct Inferno
{
  Element element = Element::Fire;
  std::uint8_t damage = 10;
};

struct Jet
{
  Element element = Element::Water;
  std::uint8_t damage = 5;
};

struct Splash
{
  Element element = Element::Water;
  std::uint8_t primary_damage = 20;
  std::uint8_t secondary_damage = 10;
};

struct Missile
{
  Element element = Element::Aether;
  std::uint8_t damage = 10;
  std::uint8_t projectiles = 3;
};

struct Teleport
{
  Element element = Element::Aether;
};

struct Lightning
{
  Element element = Element::Wind;
  std::uint8_t primary_damage = 20;
  std::uint8_t secondary_damage = 10;
  std::uint8_t forks = 3;
};

struct Gust
{
  Element element = Element::Wind;
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
