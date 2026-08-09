#ifndef WIS_UI_BUTTON_PANEL_H
#define WIS_UI_BUTTON_PANEL_H


#include <vector>

#include <entt/entt.hpp>

#include "game/events.h"

#include "ui/panel.h"
#include "ui/widget.h"


namespace wis::ui {


enum class Button_type : std::uint32_t { Spacer, Menu, Undo, Reset, Quit };


struct Button_widget final : Widget
{
  Button_widget(Button_type type,
      std::uint32_t mesh_index,
      float x,
      float y,
      float w = cval::tile_size_ui,
      float h = cval::tile_size_ui)
      :
      Widget{mesh_index, x, y, w, h},
      type{type},
      is_enabled{type != Button_type::Spacer},
      is_visible{is_enabled} {}

  Button_type type;
  bool is_enabled;
  bool is_visible;
  bool is_pressed = false;
};


class Button_panel final : public Panel
{
public:
  explicit Button_panel(entt::dispatcher& dispatcher);

  void init();
  void enable_undo(bool enable = true);

  void hover(const glm::vec2& point);
  void click(const glm::vec2& point);
  void declick(const glm::vec2& point);

  void clear_hover();
  void clear_pressed();

  [[nodiscard]] std::span<Button_widget> buttons() { return buttons_; }

private:
  entt::dispatcher& dispatcher_;
  std::vector<Button_widget> buttons_;
};


}  // namespace wis::ui


#endif  // WIS_UI_BUTTON_PANEL_H
