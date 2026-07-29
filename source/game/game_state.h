#ifndef WIS_GAME_GAME_STATE_H
#define WIS_GAME_GAME_STATE_H


#include <cstdint>
#include <vector>

#include "game/slime.h"
#include "game/spells.h"


namespace wis {


struct Game_state
{
  std::uint32_t player_index;
  std::vector<Spell> spells;
  std::vector<Slime> slimes;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_STATE_H
