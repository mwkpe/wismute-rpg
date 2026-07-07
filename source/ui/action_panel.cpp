#include "action_panel.h"


wis::ui::Action_panel::Action_panel(entt::dispatcher& dispatcher) : dispatcher_{dispatcher}
{
}


void wis::ui::Action_panel::init()
{
  decoration_widgets_.emplace_back(30, 0.0f, 0.0f, 2.4f, 2.4f);
}


void wis::ui::Action_panel::update(std::uint64_t elapsed_ns)
{
}
