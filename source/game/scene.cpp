#include "scene.h"


#include <cstdint>
#include "app/error.h"
#include "core/constants.h"
#include "core/lattice.h"
#include "game/constants.h"


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


void wis::Scene::create_test()
{
  name_ = "test";
  size_ = glm::uvec2{12, 8};

  tiles_.clear();
  tiles_.resize(size_.x * size_.y, Tile{});

  std::vector<std::uint32_t> test = {
    23, 20, 22, 20, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 20, 20, 20, 23, 22, 25, 25, 25, 25, 25, 20,
    25, 25, 20, 20, 22, 22, 23, 25, 25, 25, 20, 23,
    25, 25, 25, 20, 20, 20, 23, 40, 25, 25, 20, 22,
    25, 25, 40, 20, 20, 20, 22, 20, 20, 25, 20, 20,
    25, 40, 20, 23, 20, 20, 20, 22, 20, 25, 23, 20,
    25, 40, 20, 20, 22, 20, 20, 20, 20, 20, 20, 20,
    25, 25, 22, 20, 20, 20, 20, 20, 22, 20, 20, 22
  };

  {
    std::uint32_t index = 0;

    for (auto& tile : tiles_) {
      tile.mesh_index = test[index];
      tile.index = index++;
      tile.col = tile.index % size_.x;
      tile.row = tile.index / size_.x;
      tile.is_nil = tile.index == 0;

      if (tile.mesh_index == 25) {
        tile.element = Element::Water;
      }

      if (tile.mesh_index == 40) {
        tile.element = Element::Nature;
      }

      if (tile.mesh_index >= 20 &&  tile.mesh_index <= 23) {
        tile.is_wall = false;
      }
    }
  }

  Lattice lattice{size_, val::tile_size()};

  sprites_.clear();
  slimes_.clear();

  const glm::vec3 o{0.0f, 0.0f, 0.4f};
  const glm::vec3 o2{0.0f, 0.0f, 0.6f};

  // Trees
  // sprites_.emplace_back(lattice.as_position_xz(61, o), 61, glm::uvec2{1, 5}, 60);
  // sprites_.emplace_back(lattice.as_position_xz(61, o), 61, glm::uvec2{1, 5}, 61);
  // sprites_.emplace_back(lattice.as_position_xz(50, o), 50, glm::uvec2{2, 4}, 60);
  // sprites_.emplace_back(lattice.as_position_xz(50, o), 50, glm::uvec2{2, 4}, 61);
  // sprites_.emplace_back(lattice.as_position_xz(73, o), 73, glm::uvec2{1, 6}, 60);
  // sprites_.emplace_back(lattice.as_position_xz(73, o), 73, glm::uvec2{1, 6}, 61);
  // sprites_.emplace_back(lattice.as_position_xz(43, o), 43, glm::uvec2{7, 3}, 60);
  // sprites_.emplace_back(lattice.as_position_xz(43, o), 43, glm::uvec2{7, 3}, 61);

  for (const auto& tile : tiles_) {
    if (tile.element == Element::Nature) {
      sprites_.emplace_back(lattice.as_position_xz(tile.index, o), tile.index,
          glm::uvec2{tile.col, tile.row}, 60);
      sprites_.emplace_back(lattice.as_position_xz(tile.index, o), tile.index,
          glm::uvec2{tile.col, tile.row}, 61);
    }
  }

  // Stones
  sprites_.emplace_back(lattice.as_position_xz(78, o), 78, glm::uvec2{7, 6}, 66);
  sprites_.emplace_back(lattice.as_position_xz(78, o2), 78, glm::uvec2{7, 6}, 67);
  sprites_.emplace_back(lattice.as_position_xz(62, o), 62, glm::uvec2{2, 5}, 66);
  sprites_.emplace_back(lattice.as_position_xz(62, o2), 62, glm::uvec2{2, 5}, 67);

  for (const auto& sprite : sprites_) {
    tiles_[sprite.scene_index].is_wall = true;
  }

  // Slimes
  slimes_.emplace_back(1, lattice.as_position_xz(15, o), 15 , 140, 0.06f, 4.8f, val::tau() * 0.15f);
  slimes_.emplace_back(2, lattice.as_position_xz(52, o), 52 , 140, 0.06f, 4.8f, val::tau() * 0.65f);
  slimes_.emplace_back(3, lattice.as_position_xz(46, o), 46 , 141, 0.06f, 4.8f, val::tau() * 0.35f);
  slimes_.emplace_back(4, lattice.as_position_xz(56, o), 56, 143, 0.05f, 3.4f, val::tau() * 0.65f);

  for (const auto& slime : slimes_) {
    tiles_[slime.scene_index].slime_id = slime.id;
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
