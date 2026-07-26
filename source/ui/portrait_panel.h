#ifndef WIS_UI_PORTRAIT_PANEL_H
#define WIS_UI_PORTRAIT_PANEL_H


#include <span>
#include <vector>

#include "ui/panel.h"
#include "ui/widget.h"


namespace wis::ui {


class Portrait_panel final : public Panel
{
public:
  Portrait_panel();
  [[nodiscard]] std::span<const Widget> decorations() const { return decorations_; }

private:
  std::vector<Widget> decorations_;
};


}  // namespace wis::ui


#endif  // WIS_UI_PORTRAIT_PANEL_H
