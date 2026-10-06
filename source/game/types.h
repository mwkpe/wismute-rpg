#ifndef WIS_GAME_TYPES_H
#define WIS_GAME_TYPES_H


#include <cstdint>


namespace wis {


enum class Cursor_type : std::uint32_t { White, Green, Red, Check, Verboten, Wait, Cross };


enum class Cardinal : std::uint32_t
{
  North,
  South,
  East,
  West
};


enum class Element : std::uint32_t
{
  None,
  Fire,
  Water,
  Aether
};


struct Oscillation
{
  float amplitude = 0.0f;
  float frequency = 0.0f;
  float phase = 0.0f;
};


}  // namespace wis


#endif  // WIS_GAME_TYPES_H
