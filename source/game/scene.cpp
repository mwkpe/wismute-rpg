#include "scene.h"


#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>

#include "app/error.h"
#include "core/constants.h"
#include "core/lattice.h"
#include "game/constants.h"
#include "util/utility.h"


namespace {


std::uint32_t reindex(std::uint32_t index, std::uint32_t size_x, std::uint32_t margin_x)
{
  return (index / size_x + margin_x) * (size_x + 2u * margin_x) + (index % size_x + margin_x);
}


void assign_neighbors(std::span<wis::Tile> tiles, const glm::uvec2& size)
{
  auto size_x = static_cast<std::int32_t>(size.x);
  auto size_y = static_cast<std::int32_t>(size.y);

  // East and west
  for (int i=0; i<size_y; i++) {
    for (int j=1; j<size_x; j++) {
      int index = i * size_x + j;

      if (!tiles[index].is_nil && !tiles[index - 1].is_nil) {
        tiles[index].set_west_index(index - 1);
      }
    }

    for (int j=0; j<size_x-1; j++) {
      int index = i * size_x + j;

      if (!tiles[index].is_nil && !tiles[index + 1].is_nil) {
        tiles[index].set_east_index(index + 1);
      }
    }
  }

  // North and south
  for (int i=0; i<size_x; i++) {
    for (int j=1; j<size_y; j++) {
      int index = j * size_x + i;

      if (!tiles[index].is_nil && !tiles[index - size_x].is_nil) {
        tiles[index].set_north_index(index - size_x);
      }
    }

    for (int j=0; j<size_y-1; j++) {
      int index = j * size_x + i;

      if (!tiles[index].is_nil && !tiles[index + size_x].is_nil) {
        tiles[index].set_south_index(index + size_x);
      }
    }
  }
}


}  // namespace


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


auto wis::Scene::spell_slot(std::uint8_t id) const -> const Spell_slot*
{
  auto it = std::ranges::find(spell_slots_, id, &wis::Spell_slot::id);

  if (it != spell_slots_.end()) {
    return std::to_address(it);
  }

  return nullptr;
}


auto wis::Scene::spell_slot(std::uint8_t id) -> Spell_slot*
{
  auto it = std::ranges::find(spell_slots_, id, &wis::Spell_slot::id);

  if (it != spell_slots_.end()) {
    return std::to_address(it);
  }

  return nullptr;
}


void wis::Scene::load_scene(std::string_view filepath)
{
  clear();

  auto scene_data = util::read_json(filepath);

  const auto& attributes = scene_data["attributes"];
  name_ = attributes["name"];

  margin_ = {3u, 3u};
  const glm::uvec2 map_size{attributes["cols"], attributes["rows"]};

  size_ = map_size + margin_ * 2u;
  start_index_ = reindex(attributes["start_index"], map_size.x, margin_.x);

  const Lattice lattice{size_, cval::tile_size};

  // Spells
  {
    spell_slots_.reserve(20);

    std::uint8_t id = 0;

    for (const auto& spell_data : scene_data["spells"]) {
      switch (spell_data["key"].get<std::uint32_t>()) {
        case Fireball::key: { spell_slots_.emplace_back(Fireball{}, ++id); } break;
        case Inferno::key: { spell_slots_.emplace_back(Inferno{}, ++id); } break;
        case Jet::key: { spell_slots_.emplace_back(Jet{}, ++id); } break;
        case Splash::key: { spell_slots_.emplace_back(Splash{}, ++id); } break;
        case Lightning::key: { spell_slots_.emplace_back(Lightning{}, ++id); } break;
        case Gust::key: { spell_slots_.emplace_back(Gust{}, ++id); } break;
        case Missile::key: { spell_slots_.emplace_back(Missile{}, ++id); } break;
        case Blink::key: { spell_slots_.emplace_back(Blink{spell_data["steps"]}, ++id); } break;
        case Teleport::key: { spell_slots_.emplace_back(Teleport{}, ++id); } break;
        default:;
      }
    }
  }

  // Tiles
  {
    tiles_.resize(size_.x * size_.y, Tile{});

    std::uint32_t index = 0;

    for (auto& tile : tiles_) {
      tile.index = index++;
      tile.col = tile.index % size_.x;
      tile.row = tile.index / size_.x;
      tile.mesh_index = 20;  // Empty tile
    }

    index = 0;

    for (const auto& tile_data : scene_data["tiles"]) {
      std::uint32_t map_index = tile_data["index"];
      std::uint32_t scene_index = reindex(map_index, map_size.x, margin_.x);

      auto& tile = tiles_[scene_index];

      tile.map_index = map_index;
      tile.mesh_index = tile_data["mesh_index"];
      tile.is_nil = tile_data.value("is_nil", false);
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
      sprites_.emplace_back(lattice.as_position_xz(tile.index, cval::sprite_offset), tile.index,
          glm::uvec2{tile.col, tile.row}, 60);
      sprites_.emplace_back(lattice.as_position_xz(tile.index, cval::sprite_offset), tile.index,
          glm::uvec2{tile.col, tile.row}, 61);
    }
  }

  // Slimes
  {
    std::uint32_t id = 0;
    float offset = 0.15f;

    for (const auto& slime : scene_data["slimes"]) {
      std::uint32_t index = reindex(slime["index"], map_size.x, margin_.x);
      std::uint32_t weight = slime["weight"];
      std::uint32_t mesh = weight == 2 ? 141 : 143;

      slimes_.emplace_back(id++,
          slime["health"],
          weight,
          index,
          mesh,
          0.06f,
          5.0f,
          cval::tau() * offset);

      offset += 0.1f;
    }
  }

  assign_neighbors(tiles_, size_);
}


void wis::Scene::clear()
{
  name_ = {};
  size_ = {1, 1};
  spell_slots_.clear();
  tiles_.clear();
  sprites_.clear();
  slimes_.clear();
}


void wis::Scene::set_spell_slots(std::span<const Spell_slot> spell_slots)
{
  spell_slots_.assign_range(spell_slots);
}


void wis::Scene::set_slimes(std::span<const Slime> slimes)
{
  slimes_.assign_range(slimes);
}
