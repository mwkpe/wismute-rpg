#ifndef WIS_GAME_SCENE_H
#define WIS_GAME_SCENE_H


#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <glm/glm.hpp>

#include "game/cards.h"
#include "game/slime.h"
#include "game/sprite.h"
#include "game/tile.h"



namespace wis {


class Scene final
{
public:
  void load_scene(std::string_view filepath);
  void reset();

  [[nodiscard]] const glm::uvec2& size() const { return size_; }
  [[nodiscard]] const glm::uvec2& margin() const { return margin_; }

  [[nodiscard]] std::uint32_t start_index() const { return start_index_; }

  [[nodiscard]] Tile* tile(std::uint32_t index);
  [[nodiscard]] const Tile* tile(std::uint32_t index) const;

  [[nodiscard]] std::optional<Card> card(std::uint8_t id) const;
  [[nodiscard]] std::span<Card> cards() { return cards_; }
  [[nodiscard]] std::span<const Card> cards() const { return cards_; }

  [[nodiscard]] std::span<Tile> tiles() { return tiles_; }
  [[nodiscard]] std::span<const Tile> tiles() const { return tiles_; }

  [[nodiscard]] std::span<Sprite> sprites() { return sprites_; }
  [[nodiscard]] std::span<const Sprite> sprites() const { return sprites_; }

  [[nodiscard]] std::span<Slime> slimes() { return slimes_; }
  [[nodiscard]] std::span<const Slime> slimes() const { return slimes_; }

private:
  std::string name_;
  glm::uvec2 size_ = glm::uvec2{1, 1};
  glm::uvec2 margin_ = glm::uvec2{1, 1};
  std::uint32_t start_index_ = 0;
  std::vector<Card> cards_;
  std::vector<Tile> tiles_;
  std::vector<Sprite> sprites_;
  std::vector<Slime> slimes_;
};


}  // namespace wis


#endif  // WIS_GAME_SCENE_H
