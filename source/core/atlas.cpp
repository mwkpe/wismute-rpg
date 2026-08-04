#include "atlas.h"

#include <iostream>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>
#include "lodepng.h"

#include "apeiron/engine/vertex.h"

#include "app/error.h"


namespace {


auto load_image(std::string_view filepath)
    -> std::tuple<std::vector<std::uint8_t>, std::uint32_t, std::uint32_t>
{
  std::vector<std::uint8_t> file;

  if (auto error = lodepng::load_file(file, std::string{filepath}); error) {
    throw wis::Error::format("Could not read image {}: {}", filepath, lodepng_error_text(error));
  }

  lodepng::State state;
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;

  std::vector<std::uint8_t> indices;
  std::uint32_t image_w;
  std::uint32_t image_h;

  if (auto error = lodepng::decode(indices, image_w, image_h, state, file); error) {
    throw wis::Error::format("Could not decode image {}: {}", filepath, lodepng_error_text(error));
  }

  return {std::move(indices), image_w, image_h};
}


auto load_spritesheet(std::string_view filepath, std::uint32_t rows, std::uint32_t cols,
    bool apply_material)
{
  auto&& [indices, image_w, image_h] = load_image(filepath);

  const std::uint32_t tile_w = image_w / cols;
  const std::uint32_t tile_h = image_h / rows;
  std::vector<apeiron::engine::Vertex_index> vertices;

  auto read_tile = [&](std::uint32_t index, std::uint32_t position, std::uint32_t stride)
      -> std::uint32_t {
    std::uint32_t pixel_index = 0;
    std::uint32_t pixel_count = 0;

    for (std::uint32_t i=0; i<tile_h; i++) {
      for (std::uint32_t j=0; j<tile_w; j++) {
        if (std::uint8_t color_index = indices[position + j]; color_index != 0) {
          std::uint8_t material = 0;

          if (apply_material) {
            if (index == 25) {
              material = 1;  // Water
            }

            if (index == 61) {
              material = 2;  // Foliage
            }
          }

          vertices.emplace_back(pixel_index, color_index, material);
          pixel_count++;
        }

        pixel_index++;
      }

      position += stride;
    }

    return pixel_count;
  };

  std::vector<apeiron::opengl::Meshset_entry> entries;

  for (std::uint32_t index=0; index<rows*cols; index++) {
    const std::uint32_t col_offset = (index % cols) * tile_w;
    const std::uint32_t row_offset = (index / cols) * tile_h * image_w;
    const std::uint32_t position = col_offset + row_offset;
    const std::uint32_t stride = image_w;

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
    auto&& [vertices, entries] = load_spritesheet("assets/stage.png", 20, 20, true);
    stage_.init(vertices, std::move(entries));
  }

  {
    auto&& [vertices, entries] = load_spritesheet("assets/ui.png", 10, 10, false);
    ui_.init(vertices, std::move(entries));
  }
}
