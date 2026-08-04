#ifndef WIS_CORE_COLOR_RAMP_H
#define WIS_CORE_COLOR_RAMP_H


#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>


namespace wis {


struct Color_ramp
{
  std::string name;
  std::uint32_t steps = 3;
  glm::vec3 anchor = glm::vec3{0.1f, 0.06f, 0.18f};
  float hue_rotation = 0.0f;
  float chroma_low_scale = 0.7f;
  float chroma_high_scale = 0.45f;
  float lightness_low = 0.2f;
  float lightness_high = 0.92f;
};


std::vector<Color_ramp> read_color_ramps(std::string_view filepath);
void write_color_ramps(std::span<const Color_ramp> color_ramps, std::string_view filepath);


}  // namespace wis


#endif  // WIS_CORE_COLOR_RAMP_H
