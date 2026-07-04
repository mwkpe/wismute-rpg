#include "scene.h"


#include <cstdint>
#include "app/error.h"


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
    25, 25, 25, 20, 20, 23, 23, 20, 25, 25, 20, 22,
    25, 25, 20, 20, 20, 20, 22, 20, 20, 25, 20, 20,
    25, 22, 20, 23, 20, 20, 20, 22, 20, 25, 23, 20,
    25, 23, 20, 20, 22, 20, 20, 20, 20, 20, 20, 20,
    25, 25, 22, 20, 20, 20, 20, 20, 22, 20, 20, 22
  };

  {
    std::uint32_t index = 0;

    for (auto& tile : tiles_) {
      tile.mesh_index = test[index];
      tile.index = index++;
      tile.col = tile.index % size_.x;
      tile.row = tile.index / size_.x;
      tile.set_nil(tile.index == 0);

      if (tile.mesh_index == 25) {
        tile.element = Element::Water;
      }
    }
  }

  sprites_.clear();
  sprites_.emplace_back(60, glm::vec3{2.4f, 0.0f, 9.2f}, glm::uvec2{3, 5});
  sprites_.emplace_back(61, glm::vec3{2.4f, 0.0f, 9.2f}, glm::uvec2{3, 5});
  sprites_.emplace_back(60, glm::vec3{4.0f, 0.0f, 7.6f}, glm::uvec2{2, 4});
  sprites_.emplace_back(61, glm::vec3{4.0f, 0.0f, 7.6f}, glm::uvec2{2, 4});
  sprites_.emplace_back(60, glm::vec3{2.4f, 0.0f, 11.0f}, glm::uvec2{1, 6});
  sprites_.emplace_back(61, glm::vec3{2.4f, 0.0f, 11.0f}, glm::uvec2{1, 6});
  sprites_.emplace_back(60, glm::vec3{10.4f, 0.0f, 4.0f}, glm::uvec2{6, 2});
  sprites_.emplace_back(61, glm::vec3{10.4f, 0.0f, 4.0f}, glm::uvec2{6, 2});

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

      if (!tiles_[index].is_nil() && !tiles_[index-1].is_nil()) {
        tiles_[index].west_index = index-1;
      }
    }
    for (int j=0; j<size_x-1; j++) {
      int index = i * size_x + j;

      if (!tiles_[index].is_nil() && !tiles_[index+1].is_nil()) {
        tiles_[index].east_index = index+1;
      }
    }
  }

  // North and south
  for (int i=0; i<size_x; i++) {
    for (int j=1; j<size_y; j++) {
      int index = j * size_x + i;

      if (!tiles_[index].is_nil() && !tiles_[index-size_x].is_nil()) {
        tiles_[index].north_index = index-size_x;
      }
    }
    for (int j=0; j<size_y-1; j++) {
      int index = j * size_x + i;

      if (!tiles_[index].is_nil() && !tiles_[index+size_x].is_nil()) {
        tiles_[index].south_index = index+size_x;
      }
    }
  }
}
