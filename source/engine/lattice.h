#ifndef WIS_ENGINE_LATTICE_H
#define WIS_ENGINE_LATTICE_H


#include <cstdint>
#include <cmath>
#include <optional>
#include <glm/glm.hpp>


namespace wis {


class Lattice
{
public:
  Lattice() = default;
  explicit Lattice(const glm::uvec2& size, float cell_size = 1.0f) :
      size_{size},
      field_size_{glm::vec2{size} * glm::vec2{cell_size}},
      cell_size_{cell_size} {}

  void init(const glm::uvec2& size, float cell_size = 1.0f)
  {
    size_ = size;
    cell_size_ = cell_size;
    field_size_ = glm::vec2{size_} * glm::vec2{cell_size};
  }

  [[nodiscard]] inline glm::vec2 size() const { return size_; }
  [[nodiscard]] inline glm::vec2 field_size() const { return field_size_; }
  [[nodiscard]] inline float cell_size() const { return cell_size_; }

  [[nodiscard]] inline glm::vec3 as_position_xy(std::uint32_t index, float z = 0.0f) const
  {
    float x = (static_cast<float>(index % size_.x) * cell_size() + cell_size() * 0.5f);
    float y = (static_cast<float>(static_cast<std::uint32_t>(index / size_.x)) *
        cell_size() + cell_size() * 0.5f);
    return {x, y, z};
  }

  [[nodiscard]] inline glm::vec3 as_position_xz(std::uint32_t index, float y = 0.0f) const
  {
    float x = (static_cast<float>(index % size_.x) * cell_size() + cell_size() * 0.5f);
    float z = (static_cast<float>(static_cast<std::uint32_t>(index / size_.x)) *
        cell_size() + cell_size() * 0.5f);
    return {x, y, z};
  }

  [[nodiscard]] inline glm::vec3 as_position_xy(std::uint32_t index, const glm::vec3& offset) const
  {
    float x = (static_cast<float>(index % size_.x) * cell_size() + cell_size() * 0.5f);
    float y = (static_cast<float>(static_cast<std::uint32_t>(index / size_.x)) *
        cell_size() + cell_size() * 0.5f);
    return glm::vec3{x, y, 0.0f} + offset;
  }

  [[nodiscard]] inline glm::vec3 as_position_xz(std::uint32_t index, const glm::vec3& offset) const
  {
    float x = (static_cast<float>(index % size_.x) * cell_size() + cell_size() * 0.5f);
    float z = (static_cast<float>(static_cast<std::uint32_t>(index / size_.x)) *
        cell_size() + cell_size() * 0.5f);
    return glm::vec3{x, 0.0f, z} + offset;
  }

  [[nodiscard]] inline std::optional<std::uint32_t> as_index(float x, float y) const
  {
    x /= cell_size();
    y /= cell_size();

    if (x > 0.0f && x < static_cast<float>(size_.x) &&
        y > 0.0f && y < static_cast<float>(size_.y)) {
      const auto col = static_cast<std::uint32_t>(std::floor(x));
      const auto row = static_cast<std::uint32_t>(std::floor(y));
      return row * size_.x + col;
    }

    return std::nullopt;
  }

  [[nodiscard]] inline std::optional<std::uint32_t> as_index_xy(const glm::vec3& position) const
  {
    return as_index(position.x, position.y);
  }

  [[nodiscard]] inline std::optional<std::uint32_t> as_index_xz(const glm::vec3& position) const
  {
    return as_index(position.x, position.z);
  }

  [[nodiscard]] inline glm::uvec2 as_coords(std::uint32_t index) const
  {
    return glm::uvec2{index % size_.x, index / size_.x};
  }

private:
  glm::uvec2 size_ = {1, 1};
  glm::vec2 field_size_ = glm::vec2{1.0};
  float cell_size_ = 1.0f;
};


}  // namespace wis


#endif  // WIS_ENGINE_LATTICE_H
