#ifndef WIS_CORE_CONSTANTS_H
#define WIS_CORE_CONSTANTS_H


#include <glm/gtc/constants.hpp>


namespace wis::cval {


constexpr float pi() { return glm::pi<float>(); }
constexpr float tau() { return glm::pi<float>() * 2.0f; }
constexpr float half_pi() { return glm::half_pi<float>(); }
constexpr float quarter_pi() { return glm::quarter_pi<float>(); }

constexpr std::uint32_t palette_size = 21;


}  // namespace wis::cval


#endif  // WIS_CORE_CONSTANTS_H
