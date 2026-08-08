#include "button_panel.h"


#include <ranges>
#include "apeiron/engine/collision.h"
#include "game/constants.h"
#include "game/events.h"


namespace {


namespace engine = apeiron::engine;


constexpr auto is_enabled = std::views::filter([](const auto& e) { return e.is_enabled; });


wis::ui::Button_widget* get_button(std::span<wis::ui::Button_widget> buttons, wis::ui::Button_type type)
{
  auto it = std::ranges::find(buttons, type, &wis::ui::Button_widget::type);
  return it != buttons.end() ? std::to_address(it) : nullptr;
}


wis::ui::Button_widget* get_hovered(std::span<wis::ui::Button_widget> buttons)
{
  auto it = std::ranges::find(buttons, true, &wis::ui::Button_widget::is_hovered);
  return it != buttons.end() ? std::to_address(it) : nullptr;
}


}


wis::ui::Button_panel::Button_panel(entt::dispatcher& dispatcher) : dispatcher_{dispatcher}
{
}


void wis::ui::Button_panel::init()
{
  buttons_.clear();

  constexpr float y = 0.0f;

  float x = 0.0f;
  buttons_.emplace_back(Button_type::Undo, 60, x, y, cval::tile_size_ui, 15 * cval::pixel_size);

  x += cval::tile_size_ui;
  buttons_.emplace_back(Button_type::Reset, 61, x, y, cval::tile_size_ui, 15 * cval::pixel_size);
}


void wis::ui::Button_panel::enable_undo(bool enable)
{
  if (auto* undo_button = get_button(buttons_, Button_type::Undo); undo_button) {
    undo_button->is_enabled = enable;
  }

  if (auto* reset_button = get_button(buttons_, Button_type::Reset); reset_button) {
    reset_button->is_enabled = enable;
  }
}


void wis::ui::Button_panel::hover(const glm::vec2& point)
{
  clear_hover();

  for (auto& button : buttons_ | is_enabled) {
    if (engine::collision::within({point.x, point.y}, button.rect)) {
      button.is_hovered = true;
    }
  }
}


void wis::ui::Button_panel::click([[maybe_unused]] const glm::vec2& point)
{
  if (auto* button = get_hovered(buttons_); button) {
    button->is_pressed = true;
  }
}


void wis::ui::Button_panel::declick([[maybe_unused]] const glm::vec2& point)
{
  if (auto* button = get_hovered(buttons_); button) {
    switch (button->type) {
      case Button_type::Undo:
        dispatcher_.trigger(event::Undo_pressed{});
      break;
      case Button_type::Reset:
        dispatcher_.trigger(event::Reset_pressed{});
      break;
      default:;
    }

    button->is_pressed = false;
  }
}


void wis::ui::Button_panel::clear_hover()
{
  for (auto& button : buttons_) {
    button.is_hovered = false;
  }
}


void wis::ui::Button_panel::clear_pressed()
{
  for (auto& button : buttons_) {
    button.is_pressed = false;
  }
}
