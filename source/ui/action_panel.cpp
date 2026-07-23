#include "action_panel.h"


#include "game/constants.h"


namespace {


template<class... Ts> struct match : Ts... { using Ts::operator()...; };


}  // namespace


wis::ui::Action_panel::Action_panel(entt::dispatcher& dispatcher) : dispatcher_{dispatcher}
{
}


void wis::ui::Action_panel::init(std::span<const Card> cards)
{
  float pos = 0.0f;

  for (const auto& card : cards) {
    std::visit(match{
        [&](Move m){ decoration_widgets_.emplace_back(9 + m.steps, pos, 0.0f); },
        [&](Fireball){ decoration_widgets_.emplace_back(0, pos, 0.0f); },
        [&](Inferno){ decoration_widgets_.emplace_back(1, pos, 0.0f); },
        [&](Jet){ decoration_widgets_.emplace_back(2, pos, 0.0f); },
        [&](Splash){ decoration_widgets_.emplace_back(3, pos, 0.0f); },
        [&](Lightning){ },
        [&](Gust){ },
        [&](Missile){ decoration_widgets_.emplace_back(4, pos, 0.0f); },
        [&](Teleport){ decoration_widgets_.emplace_back(5, pos, 0.0f); }
    }, card);

    pos += val::tile_size_ui();
  }
}


void wis::ui::Action_panel::update(std::uint64_t elapsed_ns)
{
}
