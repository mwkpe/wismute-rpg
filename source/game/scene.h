#ifndef WIS_GAME_SCENE_H
#define WIS_GAME_SCENE_H


#include <span>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "game/sprite.h"
#include "game/tile.h"


namespace wis {


class Scene final
{
public:
  void create_test();
  void reset();

  [[nodiscard]] const glm::uvec2& size() const { return size_; }
  [[nodiscard]] Tile* tile(std::uint32_t index);
  [[nodiscard]] const Tile* tile(std::uint32_t index) const;
  [[nodiscard]] std::span<Tile> tiles() { return tiles_; }
  [[nodiscard]] std::span<const Tile> tiles() const { return tiles_; }
  [[nodiscard]] std::span<Sprite> sprites() { return sprites_; }
  [[nodiscard]] std::span<const Sprite> sprites() const { return sprites_; }

private:
  void connect_neighbors();

  std::string name_;
  glm::uvec2 size_ = glm::uvec2{1, 1};
  std::vector<Tile> tiles_;
  std::vector<Sprite> sprites_;
};


}  // namespace wis


#endif  // WIS_GAME_SCENE_H
