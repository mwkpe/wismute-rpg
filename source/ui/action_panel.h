#ifndef WIS_UI_ACTION_PANEL_H
#define WIS_UI_ACTION_PANEL_H


#include <span>
#include <vector>

#include <entt/entt.hpp>

#include "game/cards.h"

#include "ui/panel.h"
#include "ui/widget.h"


namespace wis::ui {


struct Action_widget final : Widget
{
  using Widget::Widget;
};


class Action_panel final : public Panel
{
public:
  explicit Action_panel(entt::dispatcher& dispatcher);
  void init(std::span<const Card> cards);
  void update(std::uint64_t elapsed_ns);

  [[nodiscard]] std::span<const Widget> decoration_widgets() const {
      return decoration_widgets_; }
  [[nodiscard]] std::span<const Action_widget> action_widgets() const {
      return action_widgets_; }

private:
  entt::dispatcher& dispatcher_;
  std::vector<Widget> decoration_widgets_;
  std::vector<Action_widget> action_widgets_;
};


}  // namespace wis::ui


#endif  // WIS_UI_ACTION_PANEL_H
