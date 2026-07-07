#include "atlas.h"

#include <iostream>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>

#include "apeiron/engine/color_converter.h"
#include "apeiron/engine/image_loader.h"
#include "apeiron/engine/vertex.h"

#include "app/error.h"
#include "engine/palette.h"


namespace {


struct Pixel
{
  auto operator<=>(const Pixel&) const = default;
  [[nodiscard]] glm::vec4 rgba_norm() const {
      return apeiron::engine::as_rgba_norm(r, g, b, a); }
  [[nodiscard]] std::uint32_t rgba_uint() const {
      return apeiron::engine::as_rgba_uint(r, g, b, a); }
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;
  std::uint8_t a;
};


auto load_spritesheet(std::string_view filename, std::uint32_t rows, std::uint32_t cols)
{
  auto&& [bytes, image_w, image_h, channel_count] = apeiron::engine::load_image(filename, false);

  if (channel_count != 4) {
    throw wis::Error::format("Image must be RGBA: {}", filename);
  }

  const std::uint32_t tile_w = image_w / cols;
  const std::uint32_t tile_h = image_h / rows;
  std::vector<apeiron::engine::Vertex_index> vertices;

  auto read_tile = [&](std::uint32_t index, std::uint32_t position, std::uint32_t stride)
      -> std::uint32_t {
    std::uint32_t pixel_index = 0;
    std::uint32_t pixel_count = 0;
    wis::Palette palette;

    for (std::uint32_t i=0; i<tile_h; i++) {
      for (std::uint32_t j=0; j<tile_w; j++) {
        Pixel pixel{bytes[position], bytes[position + 1], bytes[position + 2], bytes[position + 3]};

        if (pixel.a == 255) {
          auto it = palette.color_map.find(pixel.rgba_uint());

          if (it != std::end(palette.color_map)) {
            std::uint8_t material = 0;

            if (index == 25) {
              material = 1;  // Water
            }

            if (index == 61) {
              material = 2;  // Foliage
            }

            vertices.emplace_back(pixel_index, it->second, material);
          }
          else {
            // Invalid color
            vertices.emplace_back(pixel_index, 0);
          }

          pixel_count++;
        }
        else if (pixel.a > 0) {
          throw wis::Error::format("Translucent pixels not allowed: {}", filename);
        }
        // Pixels with alpha 0 are ignored (true transparency)

        position += channel_count;
        pixel_index++;
      }

      position += stride - tile_w * channel_count;
    }

    return pixel_count;
  };

  std::vector<apeiron::opengl::Meshset_entry> entries;

  for (std::uint32_t index=0; index<rows*cols; index++) {
    const std::uint32_t col_offset = (index % cols) * tile_w * channel_count;
    const std::uint32_t row_offset = (index / cols) * tile_h * image_w * channel_count;
    const std::uint32_t position = col_offset + row_offset;
    const std::uint32_t stride = image_w * channel_count;

    const auto vertex_count = read_tile(index, position, stride);

    if (index == 0) {
      entries.emplace_back(0, vertex_count);
    }
    else {
      const auto& last = entries.back();
      entries.emplace_back(last.first_vertex + last.vertex_count, vertex_count);
    }
  }

  return std::tuple{vertices, entries};
}


}  // namespace


void wis::Atlas::init()
{
  {
    auto&& [vertices, entries] = load_spritesheet("assets/stage.png", 20, 20);
    stage_.init(vertices, std::move(entries));
  }

  {
    auto&& [vertices, entries] = load_spritesheet("assets/ui.png", 10, 10);
    ui_.init(vertices, std::move(entries));
  }
}
