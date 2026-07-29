#ifndef WIS_GAME_EVENTS_H
#define WIS_GAME_EVENTS_H


#include <cstdint>


namespace wis::event {


struct Achievement_unlocked
{
  std::uint32_t id = 0;
};


struct Action_selected
{
  std::uint32_t id = 0;
};


struct Action_deselected
{
  std::uint32_t id = 0;
};


struct Action_triggered
{
  std::uint32_t id = 0;
};


}  // namespace wis::event


#endif  // WIS_GAME_EVENTS_H
