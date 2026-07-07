#include "action_panel.h"


wis::ui::Action_panel::Action_panel(entt::dispatcher& dispatcher) : dispatcher_{dispatcher}
{
}


void wis::ui::Action_panel::init()
{
  decoration_widgets_.emplace_back(0, 0.0f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(1, 2.4f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(2, 4.8f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(3, 7.2f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(4, 9.6f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(5, 12.0f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(20, 14.4f, 0.0f, 3.2f, 2.4f);
  decoration_widgets_.emplace_back(26, 16.8f, 0.0f, 3.2f, 2.4f);
}


void wis::ui::Action_panel::update(std::uint64_t elapsed_ns)
{
}
