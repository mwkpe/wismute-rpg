#include "portrait_panel.h"


#include "game/constants.h"


wis::ui::Portrait_panel::Portrait_panel()
{
  constexpr float tile_size = cval::tile_size_ui;

  decorations_.emplace_back(48, 0.0f, 0.0f);
  decorations_.emplace_back(49, tile_size, 0.0f);
  decorations_.emplace_back(58, 0.0f, tile_size);
  decorations_.emplace_back(59, tile_size, tile_size);
}
