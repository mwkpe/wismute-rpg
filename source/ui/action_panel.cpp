#include "action_panel.h"


#include <ranges>
#include "apeiron/engine/collision.h"
#include "game/constants.h"
#include "game/events.h"
#include "util/utility.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto is_available = std::views::filter([](const auto& e) { return e.is_available; });


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


void wis::ui::Action_panel::init(std::span<const Spell_slot> spell_slots)
{
  reset();

  float x = 0.0f;
  constexpr float y = 0.0f;

  for (const auto& slot : spell_slots) {
    std::visit(util::match{
        [&](Blink blink){
            const auto mesh_index = blink.mesh_index + blink.steps - 1;
            actions_.emplace_back(slot.id, slot.is_available, blink.element, mesh_index, x, y);
        },
        [&](auto s){
            actions_.emplace_back(slot.id, slot.is_available, s.element, s.mesh_index, x, y);
        }
    }, slot.spell);

    x += cval::tile_size_ui;
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

  for (auto& widget : actions_ | is_available) {
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

  for (auto& widget : actions_ | is_available) {
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
    dispatcher_.trigger(event::Action_selected{widget->id});
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
    widget->is_available = false;
    widget->position.y = 0.0f;
  }

  clear_selection();
}


void wis::ui::Action_panel::on_action_deselected(const event::Action_deselected& event)
{
  if (auto* widget = get_widget(actions_, event.id); widget) {
    widget->position.y = 0.0f;
  }

  clear_selection();
}
