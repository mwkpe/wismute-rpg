#ifndef WIS_CORE_COLOR_MATH_H
#define WIS_CORE_COLOR_MATH_H


#include <cmath>
#include <cstdint>
#include <numbers>
#include <optional>

#include <glm/glm.hpp>


namespace wis {


inline float srgb_to_linear(float c)
{
  if (c <= 0.04045f) {
    return c / 12.92f;
  }

  return std::pow((c + 0.055f) / 1.055f, 2.4f);
}


inline float linear_to_srgb(float c)
{
  if (c <= 0.0031308f) {
    return c * 12.92f;
  }

  return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}


inline glm::vec3 srgb_to_linear(glm::vec3 c)
{
  return {srgb_to_linear(c.r), srgb_to_linear(c.g), srgb_to_linear(c.b)};
}


inline glm::vec3 linear_to_srgb(glm::vec3 c)
{
  return {linear_to_srgb(c.r), linear_to_srgb(c.g), linear_to_srgb(c.b)};
}


inline glm::vec3 linear_to_oklab(glm::vec3 c)
{
  // Linear sRGB to LMS cone response (Ottosson)
  const auto l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
  const auto m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
  const auto s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

  // Cube root compresses cone response into a perceptually uniform scale
  const auto l_ = std::cbrt(l);
  const auto m_ = std::cbrt(m);
  const auto s_ = std::cbrt(s);

  return {
    0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
    1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
    0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_
  };
}


inline glm::vec3 oklab_to_linear(glm::vec3 c)
{
  const auto l_ = c.x + 0.3963377774f * c.y + 0.2158037573f * c.z;
  const auto m_ = c.x - 0.1055613458f * c.y - 0.0638541728f * c.z;
  const auto s_ = c.x - 0.0894841775f * c.y - 1.2914855480f * c.z;

  const auto l = l_ * l_ * l_;
  const auto m = m_ * m_ * m_;
  const auto s = s_ * s_ * s_;

  return {
    4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
    -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
    -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s
  };
}


glm::vec3 oklab_to_oklch(glm::vec3 c)
{
  // Polar form: chroma is the radius, hue the angle in degrees
  const auto chroma = std::hypot(c.y, c.z);
  const auto hue = std::atan2(c.z, c.y) * 180.0f / std::numbers::pi_v<float>;

  return {c.x, chroma, hue};
}


glm::vec3 oklch_to_oklab(glm::vec3 c)
{
  const auto radians = c.z * std::numbers::pi_v<float> / 180.0f;
  return {c.x, c.y * std::cos(radians), c.y * std::sin(radians)};
}


glm::vec3 srgb_to_oklch(glm::vec3 c)
{
  return oklab_to_oklch(linear_to_oklab(srgb_to_linear(c)));
}


glm::vec3 oklch_to_srgb(glm::vec3 c)
{
  return linear_to_srgb(oklab_to_linear(oklch_to_oklab(c)));
}


float chroma_scale_at(const Color_ramp& ramp, float t)
{
  // The anchor chroma sits at the midtone, both ends scale away from it
  if (t < 0.5f) {
    return std::lerp(ramp.chroma_low_scale, 1.0f, t * 2.0f);
  }

  return std::lerp(1.0f, ramp.chroma_high_scale, (t - 0.5f) * 2.0f);
}


inline std::optional<glm::vec4> calculate_color_step(const Color_ramp& ramp, std::uint32_t step)
{
  if (step >= ramp.steps || ramp.steps < 2) {
    return std::nullopt;
  }

  const auto base = srgb_to_oklch(ramp.anchor);
  const auto t = static_cast<float>(step) / static_cast<float>(ramp.steps - 1);
  const auto lightness = std::lerp(ramp.lightness_low, ramp.lightness_high, t);
  const auto chroma = base.y * chroma_scale_at(ramp, t);
  const auto hue = base.z + ramp.hue_rotation * (t - 0.5f);
  const auto rgb = glm::clamp(oklch_to_srgb({lightness, chroma, hue}), 0.0f, 1.0f);

  return glm::vec4{rgb, 1.0f};
}


}  // namespace wis


#endif  // WIS_CORE_COLOR_MATH_H
