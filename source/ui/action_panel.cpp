#include "action_panel.h"


#include <print>
#include <ranges>
#include "apeiron/engine/collision.h"
#include "game/constants.h"
#include "game/events.h"
#include "util/utility.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto is_not_spent = std::views::filter([](const auto& a) { return !a.is_spent; });


wis::ui::Action_widget* get_widget(std::span<wis::ui::Action_widget> widgets, std::uint32_t id)
{
  auto it = std::ranges::find(widgets, id, &wis::ui::Action_widget::id);
  return it != widgets.end() ? std::to_address(it) : nullptr;
}


wis::ui::Action_widget* get_hovered(std::span<wis::ui::Action_widget> widgets)
{
  auto it = std::ranges::find(widgets, true, &wis::ui::Action_widget::hovered);
  return it != widgets.end() ? std::to_address(it) : nullptr;
}


}


wis::ui::Action_panel::Action_panel(entt::dispatcher& dispatcher) : dispatcher_{dispatcher}
{
  dispatcher_.sink<event::Action_triggered>().connect<&Action_panel::on_action_triggered>(this);
  dispatcher_.sink<event::Action_deselected>().connect<&Action_panel::on_action_deselected>(this);
}


void wis::ui::Action_panel::init(std::span<const Card> cards)
{
  reset();

  float x = 0.0f;
  std::uint32_t id = 1;

  for (const auto& card : cards) {
    std::visit(util::match{
        [&](Move c){ actions_.emplace_back(id, c, 9 + c.steps, x, 0.0f); },
        [&](Fireball c){ actions_.emplace_back(id, c, 0, x, 0.0f); },
        [&](Inferno c){ actions_.emplace_back(id, c, 1, x, 0.0f); },
        [&](Jet c){ actions_.emplace_back(id, c, 2, x, 0.0f); },
        [&](Splash c){ actions_.emplace_back(id, c, 3, x, 0.0f); },
        [&](Lightning){ },
        [&](Gust){ },
        [&](Missile c){ actions_.emplace_back(id, c, 4, x, 0.0f); },
        [&](Teleport c){ actions_.emplace_back(id, c, 5, x, 0.0f); }
    }, card);

    x += val::tile_size_ui();
    id++;
  }
}


void wis::ui::Action_panel::reset()
{
  clear_hover();
  clear_selection();

  decorations_.clear();
  actions_.clear();
}


void wis::ui::Action_panel::update(float delta_s)
{
  float d = 3.0f * delta_s;

  for (auto& widget : actions_ | is_not_spent) {
    if (widget.hovered) {
      // Raise hovered widget
      widget.position.y = std::max(-0.4f, widget.position.y - d);
    }
    else if (!widget.selected) {
      // Lower widgets that lose hover
      widget.position.y = std::min(0.0f, widget.position.y + d);
    }
  }
}


void wis::ui::Action_panel::hover(const glm::vec2& point)
{
  clear_hover();

  for (auto& widget : actions_ | is_not_spent) {
    if (engine::collision::within({point.x, point.y}, widget.rect)) {
      widget.hovered = true;
    }
  }
}


void wis::ui::Action_panel::click([[maybe_unused]] const glm::vec2& point)
{
  if (auto* widget = get_hovered(actions_); widget) {
    clear_selection();
    widget->selected = true;
    dispatcher_.trigger(event::Action_selected{widget->id, widget->card});
  }
}


void wis::ui::Action_panel::clear_actions()
{
  actions_.clear();
}


void wis::ui::Action_panel::clear_hover()
{
  for (auto& widget : actions_) {
    widget.hovered = false;
  }
}


void wis::ui::Action_panel::clear_selection()
{
  for (auto& widget : actions_) {
    widget.selected = false;
  }
}


void wis::ui::Action_panel::on_action_triggered(const event::Action_triggered& event)
{
  if (auto* widget = get_widget(actions_, event.id); widget) {
    widget->is_spent = true;
    widget->mesh_index += 20u;
    widget->position.y = 0.0f;

    std::print("Action {} used\n", event.id);
  }

  clear_selection();
}


void wis::ui::Action_panel::on_action_deselected(const event::Action_deselected& event)
{
  if (auto* widget = get_widget(actions_, event.id); widget) {
    widget->position.y = 0.0f;

    std::print("Action {} deselected\n", event.id);
  }

  clear_selection();
}
