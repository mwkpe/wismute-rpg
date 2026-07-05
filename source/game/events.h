#ifndef WIS_GAME_EVENTS_H
#define WIS_GAME_EVENTS_H


#include <cstdint>
#include <entt/entt.hpp>


namespace wis::event {


struct Achievement_unlocked
{
  std::uint32_t id = 0;
};


struct Action_selected
{
  std::uint32_t id = 0;
};


struct Enemy_hit
{
  entt::entity entity;
  std::uint32_t value = 0;
};


}  // namespace wis::event


#endif  // WIS_GAME_EVENTS_H
