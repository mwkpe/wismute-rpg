#ifndef WIS_CORE_PALETTE_H
#define WIS_CORE_PALETTE_H


#include <array>
#include <map>
#include <glm/glm.hpp>
#include "apeiron/engine/color_converter.h"


namespace wis {


struct Palette
{
  static constexpr std::uint32_t color_count = 53;

  static constexpr std::array<glm::vec4, color_count> colors = {
      apeiron::engine::as_rgba_norm("#ff00ffff"),  // Invalid
      apeiron::engine::as_rgba_norm("#3f4328ff"),
      apeiron::engine::as_rgba_norm("#5f7132ff"),
      apeiron::engine::as_rgba_norm("#94ad39ff"),
      apeiron::engine::as_rgba_norm("#c2d64fff"),
      apeiron::engine::as_rgba_norm("#eff37cff"),
      apeiron::engine::as_rgba_norm("#e3e6acff"),
      apeiron::engine::as_rgba_norm("#a5c67cff"),
      apeiron::engine::as_rgba_norm("#739a70ff"),
      apeiron::engine::as_rgba_norm("#4d6659ff"),
      apeiron::engine::as_rgba_norm("#343f41ff"),
      apeiron::engine::as_rgba_norm("#282e3bff"),
      apeiron::engine::as_rgba_norm("#1a1f2eff"),
      apeiron::engine::as_rgba_norm("#1e314bff"),
      apeiron::engine::as_rgba_norm("#2f4c6cff"),
      apeiron::engine::as_rgba_norm("#3d80a3ff"),
      apeiron::engine::as_rgba_norm("#63c4ccff"),
      apeiron::engine::as_rgba_norm("#9ae5d5ff"),
      apeiron::engine::as_rgba_norm("#e5efefff"),
      apeiron::engine::as_rgba_norm("#bac9cdff"),
      apeiron::engine::as_rgba_norm("#8d99a4ff"),
      apeiron::engine::as_rgba_norm("#696f80ff"),
      apeiron::engine::as_rgba_norm("#414453ff"),
      apeiron::engine::as_rgba_norm("#b8a1c2ff"),
      apeiron::engine::as_rgba_norm("#7e659bff"),
      apeiron::engine::as_rgba_norm("#5c3a6fff"),
      apeiron::engine::as_rgba_norm("#39275eff"),
      apeiron::engine::as_rgba_norm("#2f193eff"),
      apeiron::engine::as_rgba_norm("#4e1a49ff"),
      apeiron::engine::as_rgba_norm("#7b234cff"),
      apeiron::engine::as_rgba_norm("#b23657ff"),
      apeiron::engine::as_rgba_norm("#d16974ff"),
      apeiron::engine::as_rgba_norm("#edaaa3ff"),
      apeiron::engine::as_rgba_norm("#eecb90ff"),
      apeiron::engine::as_rgba_norm("#e1a845ff"),
      apeiron::engine::as_rgba_norm("#c57835ff"),
      apeiron::engine::as_rgba_norm("#8d4830ff"),
      apeiron::engine::as_rgba_norm("#e47259ff"),
      apeiron::engine::as_rgba_norm("#c33c40ff"),
      apeiron::engine::as_rgba_norm("#8d3649ff"),
      apeiron::engine::as_rgba_norm("#5c2b34ff"),
      apeiron::engine::as_rgba_norm("#3c252bff"),
      apeiron::engine::as_rgba_norm("#684039ff"),
      apeiron::engine::as_rgba_norm("#825646ff"),
      apeiron::engine::as_rgba_norm("#b77862ff"),
      apeiron::engine::as_rgba_norm("#7d595dff"),
      apeiron::engine::as_rgba_norm("#533b41ff"),
      apeiron::engine::as_rgba_norm("#3f333bff"),
      apeiron::engine::as_rgba_norm("#2b222aff"),
      apeiron::engine::as_rgba_norm("#6d4e4bff"),
      apeiron::engine::as_rgba_norm("#867066ff"),
      apeiron::engine::as_rgba_norm("#b49d7eff"),
      apeiron::engine::as_rgba_norm("#c4c6b8ff")
  };

  std::map<std::uint32_t, std::uint8_t> color_map = {
      {0xff00ffff, 0},
      {0x3f4328ff, 1},
      {0x5f7132ff, 2},
      {0x94ad39ff, 3},
      {0xc2d64fff, 4},
      {0xeff37cff, 5},
      {0xe3e6acff, 6},
      {0xa5c67cff, 7},
      {0x739a70ff, 8},
      {0x4d6659ff, 9},
      {0x343f41ff, 10},
      {0x282e3bff, 11},
      {0x1a1f2eff, 12},
      {0x1e314bff, 13},
      {0x2f4c6cff, 14},
      {0x3d80a3ff, 15},
      {0x63c4ccff, 16},
      {0x9ae5d5ff, 17},
      {0xe5efefff, 18},
      {0xbac9cdff, 19},
      {0x8d99a4ff, 20},
      {0x696f80ff, 21},
      {0x414453ff, 22},
      {0xb8a1c2ff, 23},
      {0x7e659bff, 24},
      {0x5c3a6fff, 25},
      {0x39275eff, 26},
      {0x2f193eff, 27},
      {0x4e1a49ff, 28},
      {0x7b234cff, 29},
      {0xb23657ff, 30},
      {0xd16974ff, 31},
      {0xedaaa3ff, 32},
      {0xeecb90ff, 33},
      {0xe1a845ff, 34},
      {0xc57835ff, 35},
      {0x8d4830ff, 36},
      {0xe47259ff, 37},
      {0xc33c40ff, 38},
      {0x8d3649ff, 39},
      {0x5c2b34ff, 40},
      {0x3c252bff, 41},
      {0x684039ff, 42},
      {0x825646ff, 43},
      {0xb77862ff, 44},
      {0x7d595dff, 45},
      {0x533b41ff, 46},
      {0x3f333bff, 47},
      {0x2b222aff, 48},
      {0x6d4e4bff, 49},
      {0x867066ff, 50},
      {0xb49d7eff, 51},
      {0xc4c6b8ff, 52}
  };
};


}  // namespace wis


#endif  // WIS_CORE_PALETTE_H
