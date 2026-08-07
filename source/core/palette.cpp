#include "palette.h"


#include <charconv>
#include <filesystem>
#include <fstream>

#include "ctre.hpp"

#include "apeiron/engine/color_converter.h"


namespace {


constexpr auto header_pattern = ctll::fixed_string{R"(^GIMP Palette$)"};
constexpr auto color_pattern = ctll::fixed_string{R"(^(\d+)\s+(\d+)\s+(\d+).+$)"};


std::uint8_t as_uint8(std::string_view text)
{
  std::uint8_t value = 0;
  const auto* const end = text.data() + text.size();
  const auto [stop, error] = std::from_chars(text.data(), end, value);

  if (error == std::errc{} && stop == end) {
    return value;
  }

  return 0;
}


}  // namespace


[[nodiscard]] std::vector<glm::vec4> wis::read_palette(std::string_view filepath)
{
  std::ifstream fs{std::filesystem::path{filepath}, std::ios::binary};

  if (!fs) {
    return {};
  }

  std::vector<glm::vec4> palette;
  palette.resize(21, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

  std::string line;
  std::size_t index = 0;
  bool header_seen = false;

  while (std::getline(fs, line)) {
    if (!header_seen && ctre::match<header_pattern>(line)) {
      header_seen = true;
    }

    if (const auto entry = ctre::match<color_pattern>(line); entry && header_seen) {
      auto r = as_uint8(entry.get<1>().to_view());
      auto g = as_uint8(entry.get<2>().to_view());
      auto b = as_uint8(entry.get<3>().to_view());

      if (index < palette.size()) {
        palette[index++] = apeiron::engine::as_rgba_norm(r, g, b, 255u);
      }
    }
  }

  return palette;
}


void wis::write_palette(std::span<glm::vec4> palette, std::string_view filepath,
    std::string_view name)
{
  if (std::ofstream fs{std::filesystem::path{filepath}}; fs) {
    fs << std::format("GIMP Palette\nName: {}\nColumns: {}\n", name, palette.size());

    for (auto color : palette) {
      auto bytes = apeiron::engine::as_rgba_bytes(color);
      auto hex_code = apeiron::engine::as_hex_code(color);
      fs << std::format("{}\t{}\t{}\t{}\n", bytes.r, bytes.g, bytes.b, hex_code.view());
    }
  }
}
