#include "utility.h"


#include <fstream>
#include <sstream>
#include "app/error.h"


nlohmann::json wis::util::read_json(const std::filesystem::path& filepath)
{
  std::ifstream fs{filepath, std::ios::binary};  // Make sure UTF-8 is read properly

  if (!fs.is_open()) {
    throw wis::Error::format("Could not open file: {}", filepath.string());
  }

  std::stringstream ss;
  ss << fs.rdbuf();
  fs.close();

  auto j = nlohmann::json::parse(ss);

  if (j.empty()) {
    throw wis::Error::format("File is empty: {}", filepath.string());
  }

  return j;
}
