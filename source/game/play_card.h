#ifndef WIS_GAME_PLAY_CARD_H
#define WIS_GAME_PLAY_CARD_H


#include <cstdint>
#include <span>

#include "game/cards.h"
#include "game/player.h"
#include "game/slime.h"
#include "game/tile.h"


namespace wis {


void play_card(const Card card, Player& player, std::span<const Tile> tiles,
    std::span<Slime> slimes, std::uint32_t target_index);


}  // namespace wis


#endif  // WIS_GAME_PLAY_CARD_H
