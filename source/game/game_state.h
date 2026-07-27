#ifndef WIS_GAME_GAME_STATE_H
#define WIS_GAME_GAME_STATE_H


#include <cstdint>
#include <vector>
#include "game/cards.h"
#include "game/slime.h"


namespace wis {


struct Game_state
{
  std::uint32_t player_index;
  std::vector<Card> cards;
  std::vector<Slime> slimes;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_STATE_H
