#ifndef WIS_GAME_CAST_SPELL_H
#define WIS_GAME_CAST_SPELL_H


#include <cstdint>
#include <span>

#include "game/player.h"
#include "game/slime.h"
#include "game/spells.h"
#include "game/tile.h"


namespace wis {


void cast_spell(const Spell spell, Player& player, std::span<const Tile> tiles,
    std::span<Slime> slimes, std::uint32_t target_index);


}  // namespace wis


#endif  // WIS_GAME_CAST_SPELL_H
