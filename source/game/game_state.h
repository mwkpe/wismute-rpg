#ifndef WIS_GAME_GAME_STATE_H
#define WIS_GAME_GAME_STATE_H


#include <cstdint>
#include <stack>
#include <vector>

#include "game/slime.h"
#include "game/spells.h"


namespace wis {


struct Game_state
{
  std::uint32_t player_index;
  std::vector<Spell_slot> spell_slots;
  std::vector<Slime> slimes;
};


class Game_history
{
public:
  void add(Game_state&& state) { stack_.push(std::move(state)); }
  const Game_state& undo() { if (stack_.size() > 1) stack_.pop(); return stack_.top(); }
  const Game_state& reset() { while (stack_.size() > 1) stack_.pop(); return stack_.top(); }
  std::size_t size() const { return stack_.size(); }

private:
  std::stack<Game_state> stack_;
};


}  // namespace wis


#endif  // WIS_GAME_GAME_STATE_H
