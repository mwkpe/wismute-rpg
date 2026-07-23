#ifndef WIS_UTIL_UTILITY_H
#define WIS_UTIL_UTILITY_H


#include <filesystem>
#include "nlohmann/json.hpp"


namespace wis::util {


nlohmann::json read_json(const std::filesystem::path& filepath);


}  // namespace wis::util


#endif  // WIS_UTIL_UTILITY_H
