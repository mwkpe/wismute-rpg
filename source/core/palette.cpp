#include "palette.h"


#include <filesystem>
#include <fstream>

#include "apeiron/engine/color_converter.h"


[[nodiscard]] std::vector<glm::vec4> wis::read_palette(std::string_view filepath)
{
  return {};
}


void wis::write_palette(std::span<glm::vec4> palette, std::string_view filepath, std::string_view name)
{
  if (std::ofstream fs{std::string{filepath}}; fs) {
    fs << std::format("GIMP Palette\nName: {}\nColumns: {}\n", name, palette.size());

    for (auto color : palette) {
      auto bytes = apeiron::engine::as_rgba_bytes(color);
      auto hex_code = apeiron::engine::as_hex_code(color);
      fs << std::format("{}\t{}\t{}\t{}\n", bytes.r, bytes.g, bytes.b, hex_code.view());
    }
  }
}
