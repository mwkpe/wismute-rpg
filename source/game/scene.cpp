#include "scene.h"


#include <cstdint>
#include "app/error.h"
#include "core/constants.h"
#include "core/lattice.h"
#include "game/constants.h"
#include "util/utility.h"


auto wis::Scene::tile(std::uint32_t index) -> Tile*
{
  if (index >= tiles_.size()) {
    return &tiles_[0];  // nil tile
  }

  return &tiles_[index];
}


auto wis::Scene::tile(std::uint32_t index) const -> const Tile*
{
  if (index >= tiles_.size()) {
    return &tiles_[0];  // nil tile
  }

  return &tiles_[index];
}


void wis::Scene::load_scene(std::string_view filepath)
{
  auto j = util::read_json(filepath);

  const auto& attributes = j["attributes"];

  name_ = attributes["name"];
  size_ = glm::uvec2{attributes["cols"], attributes["rows"]};

  Lattice lattice{size_, val::tile_size()};
  glm::vec3 sprite_offset{0.0f, 0.0f, 0.4f};

  // Tiles
  {
    tiles_.clear();
    tiles_.resize(size_.x * size_.y, Tile{});

    std::uint32_t index = 0;

    for (auto& tile : tiles_) {
      const auto& tile_data = j["tiles"][index];

      tile.mesh_index = tile_data["mesh_index"];
      tile.index = index++;
      tile.col = tile.index % size_.x;
      tile.row = tile.index / size_.x;
      tile.is_nil = tile.index == 0;
      tile.is_wall = tile_data["is_wall"];

      switch (tile_data["element"].get<std::uint32_t>()) {
        case 1: tile.element = Element::Fire; break;
        case 2: tile.element = Element::Water; break;
        case 3: tile.element = Element::Wind; break;
        case 4: tile.element = Element::Aether; break;
      }
    }
  }

  // Sprites
  for (const auto& tile : tiles_) {
    if (tile.element == Element::Wind) {
      sprites_.emplace_back(lattice.as_position_xz(tile.index, sprite_offset), tile.index,
          glm::uvec2{tile.col, tile.row}, 60);
      sprites_.emplace_back(lattice.as_position_xz(tile.index, sprite_offset), tile.index,
          glm::uvec2{tile.col, tile.row}, 61);
    }
  }

  // Slimes
  {
    slimes_.clear();
    std::uint32_t id = 0;
    float offset = 0.15f;

    for (const auto& slime : j["slimes"]) {
      std::uint32_t index = slime["index"];
      std::uint32_t weight = slime["weight"];
      std::uint32_t mesh = weight == 2 ? 141 : 143;

      slimes_.emplace_back(id++,
          slime["health"],
          weight,
          lattice.as_position_xz(index, sprite_offset),
          index,
          mesh,
          0.06f,
          5.0f,
          val::tau() * offset);

      offset += 0.1f;
    }
  }

  connect_neighbors();
}


void wis::Scene::reset()
{
  name_ = {};
  size_ = glm::vec2{1.0f, 1.0f};
  tiles_.clear();
  sprites_.clear();
}


void wis::Scene::connect_neighbors()
{
  auto size_x = static_cast<std::int32_t>(size_.x);
  auto size_y = static_cast<std::int32_t>(size_.y);

  // East and west
  for (int i=0; i<size_y; i++) {
    for (int j=1; j<size_x; j++) {
      int index = i * size_x + j;

      if (!tiles_[index].is_nil && !tiles_[index - 1].is_nil) {
        tiles_[index].set_west_index(index - 1);
      }
    }

    for (int j=0; j<size_x-1; j++) {
      int index = i * size_x + j;

      if (!tiles_[index].is_nil && !tiles_[index + 1].is_nil) {
        tiles_[index].set_east_index(index + 1);
      }
    }
  }

  // North and south
  for (int i=0; i<size_x; i++) {
    for (int j=1; j<size_y; j++) {
      int index = j * size_x + i;

      if (!tiles_[index].is_nil && !tiles_[index - size_x].is_nil) {
        tiles_[index].set_north_index(index - size_x);
      }
    }

    for (int j=0; j<size_y-1; j++) {
      int index = j * size_x + i;

      if (!tiles_[index].is_nil && !tiles_[index + size_x].is_nil) {
        tiles_[index].set_south_index(index + size_x);
      }
    }
  }
}
