#ifndef WIS_CORE_PALETTE_H
#define WIS_CORE_PALETTE_H


#include <span>
#include <string_view>
#include <vector>
#include <glm/glm.hpp>



namespace wis {


[[nodiscard]] std::vector<glm::vec4> read_palette(std::string_view filepath);
void write_palette(std::span<glm::vec4> palette, std::string_view filepath, std::string_view name);


}  // namespace wis


#endif  // WIS_CORE_PALETTE_H
