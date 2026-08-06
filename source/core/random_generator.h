#ifndef GDL_RANDOM_GENERATOR_H
#define GDL_RANDOM_GENERATOR_H


#include <array>
#include <cstdint>
#include <random>


namespace gdl {


class Random_int32_generator
{
public:
  Random_int32_generator() { init(); }
  explicit Random_int32_generator(std::uint32_t seed) { init(seed); }
  Random_int32_generator(std::int32_t a, std::int32_t b) { init(a, b); }
  Random_int32_generator(std::int32_t a, std::int32_t b, std::uint32_t seed) { init(a, b, seed); }

  void init();
  void init(std::uint32_t seed);
  void init(std::int32_t a, std::int32_t b);
  void init(std::int32_t a, std::int32_t b, std::uint32_t seed);

  [[nodiscard]] std::mt19937& engine() { return engine_; }
  [[nodiscard]] std::int32_t value() { return distribution_(engine_); }

private:
  std::mt19937 engine_;
  std::uniform_int_distribution<std::int32_t> distribution_;
};


class Random_uint32_generator
{
public:
  Random_uint32_generator() { init(); }
  explicit Random_uint32_generator(std::uint32_t seed) { init(seed); }
  Random_uint32_generator(std::uint32_t a, std::uint32_t b) { init(a, b); }
  Random_uint32_generator(std::uint32_t a, std::uint32_t b, std::uint32_t seed) { init(a, b, seed); }

  void init();
  void init(std::uint32_t seed);
  void init(std::uint32_t a, std::uint32_t b);
  void init(std::uint32_t a, std::uint32_t b, std::uint32_t seed);

  [[nodiscard]] std::mt19937& engine() { return engine_; }
  [[nodiscard]] std::uint32_t value() { return distribution_(engine_); }
  [[nodiscard]] std::uint32_t between(std::uint32_t from, std::uint32_t to) {
      return from + (distribution_(engine_) % (to - from + 1)); }
  [[nodiscard]] float between(std::uint32_t from, std::uint32_t to, float factor) {
      return static_cast<float>(between(from, to)) * factor; }
  [[nodiscard]] float between(std::uint32_t from, std::uint32_t to, float factor, float offset) {
      return static_cast<float>(between(from, to)) * factor + offset; }
  [[nodiscard]] std::uint32_t upto(std::uint32_t max) { return distribution_(engine_) % (max + 1); }
  [[nodiscard]] bool flip_coin() { return distribution_(engine_) % 2 == 0; }
  [[nodiscard]] bool roll_dice() { return between(1, 6); }

private:
  std::mt19937 engine_;
  std::uniform_int_distribution<std::uint32_t> distribution_;
};


class Random_float_generator
{
public:
  Random_float_generator() = delete;
  Random_float_generator(float a, float b) { init(a, b); }
  Random_float_generator(float a, float b, std::uint32_t seed) { init(a, b, seed); }

  void init(float a, float b);
  void init(float a, float b, std::uint32_t seed);

  float value() { return distribution_(engine_); }

private:
  std::mt19937 engine_;
  std::uniform_real_distribution<float> distribution_;
};


}  // namespace gdl


#endif  // GDL_RANDOM_GENERATOR_H
