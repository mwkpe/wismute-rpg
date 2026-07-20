#ifndef WIS_GAME_TYPES_H
#define WIS_GAME_TYPES_H


#include <cstdint>


namespace wis {


enum class Cardinal : std::uint8_t
{
  North,
  South,
  East,
  West
};


enum class Element : std::uint8_t
{
  None,
  Nature,
  Water,
  Fire,
  Aether
};


}  // namespace wis


#endif  // WIS_GAME_TYPES_H
