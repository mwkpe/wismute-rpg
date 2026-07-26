#include "portrait_panel.h"


#include "game/constants.h"


wis::ui::Portrait_panel::Portrait_panel()
{
  constexpr float tile_size = val::tile_size_ui();

  decorations_.emplace_back(8, 0.0f, 0.0f);
  decorations_.emplace_back(9, tile_size, 0.0f);
  decorations_.emplace_back(18, 0.0f, tile_size);
  decorations_.emplace_back(19, tile_size, tile_size);
}
