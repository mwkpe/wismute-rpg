#ifndef WIS_UI_ACTION_PANEL_H
#define WIS_UI_ACTION_PANEL_H


#include <span>
#include <vector>

#include <entt/entt.hpp>

#include "game/cards.h"
#include "game/events.h"
#include "ui/panel.h"
#include "ui/widget.h"


namespace wis::ui {


struct Action_widget final : Widget
{
  Action_widget(std::uint32_t id,
      Card card,
      std::uint16_t mesh_index,
      float x,
      float y,
      float w = val::tile_size_ui(),
      float h = val::tile_size_ui())
      :
      Widget{mesh_index, x, y, w, h},
      id{id},
      card{card} {}

  std::uint32_t id = 0;
  Card card;
  bool is_available = true;
};


class Action_panel final : public Panel
{
public:
  explicit Action_panel(entt::dispatcher& dispatcher);

  void init(std::span<const Card> cards);
  void reset();

  void update(float delta_s);
  void hover(const glm::vec2& point);
  void click(const glm::vec2& point);

  void clear_actions();
  void clear_hover();
  void clear_selection();

  void on_action_triggered(const event::Action_triggered& event);
  void on_action_deselected(const event::Action_deselected& event);

  [[nodiscard]] std::span<const Widget> decorations() const { return decorations_; }
  [[nodiscard]] std::span<Action_widget> actions() { return actions_; }

private:
  entt::dispatcher& dispatcher_;
  std::vector<Widget> decorations_;
  std::vector<Action_widget> actions_;
};


}  // namespace wis::ui


#endif  // WIS_UI_ACTION_PANEL_H
