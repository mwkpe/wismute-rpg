#ifndef WIS_UTIL_UTILITY_H
#define WIS_UTIL_UTILITY_H


#include <cstdint>
#include <filesystem>
#include <tuple>
#include "nlohmann/json.hpp"


namespace wis::util {


template<class... Ts> struct match : Ts... { using Ts::operator()...; };


inline auto as_ndc(float x, float y, std::uint32_t w, std::uint32_t h) -> std::tuple<float, float>
{
  return {x / static_cast<float>(w) * 2.0f - 1.0f, 1.0f - y / static_cast<float>(h) * 2.0f};
}


nlohmann::json read_json(const std::filesystem::path& filepath);


}  // namespace wis::util


#endif  // WIS_UTIL_UTILITY_H
