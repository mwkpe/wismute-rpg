#include "color_ramp.h"


#include <cmath>
#include <fstream>
#include "app/error.h"
#include "util/utility.h"

#include "nlohmann/json.hpp"


namespace {


double round(float value)
{
  const double rounded = std::round(static_cast<double>(value) * 1000.0) / 1000.0;
  return rounded == 0.0 ? 0.0 : rounded;  // Avoids writing -0.0
}


}  // namespace


auto wis::read_color_ramps(std::string_view filepath) -> std::vector<Color_ramp>
{
  auto json_data = util::read_json(filepath);

  try {
    std::vector<Color_ramp> color_ramps;
    color_ramps.reserve(json_data["color_ramps"].size());

    for (const auto& ramp_data : json_data["color_ramps"]) {
      auto& color_ramp = color_ramps.emplace_back();

      const auto& anchor_data = ramp_data["anchor"];
      color_ramp.anchor = glm::vec3{anchor_data[0], anchor_data[1], anchor_data[2]};

      color_ramp.name = ramp_data["name"];
      color_ramp.steps = ramp_data["steps"];
      color_ramp.hue_rotation = ramp_data["hue_rotation"];
      color_ramp.chroma_low_scale = ramp_data["chroma_low_scale"];
      color_ramp.chroma_high_scale = ramp_data["chroma_high_scale"];
      color_ramp.lightness_low = ramp_data["lightness_low"];
      color_ramp.lightness_high = ramp_data["lightness_high"];
    }

    return color_ramps;
  }
  catch (const nlohmann::json::exception& e) {
    throw Error::format("Error reading color ramps: {}", e.what());
  }
}


void wis::write_color_ramps(std::span<const Color_ramp> color_ramps, std::string_view filepath)
{
  nlohmann::json json_data;
  json_data["color_ramps"] = nlohmann::json::array();

  for (const auto& color_ramp : color_ramps) {
    nlohmann::json ramp_data;
    const auto& anchor = color_ramp.anchor;

    ramp_data["name"] = color_ramp.name;
    ramp_data["steps"] = color_ramp.steps;
    ramp_data["anchor"] = {round(anchor.r), round(anchor.g), round(anchor.b)};
    ramp_data["hue_rotation"] = round(color_ramp.hue_rotation);
    ramp_data["chroma_low_scale"] = round(color_ramp.chroma_low_scale);
    ramp_data["chroma_high_scale"] = round(color_ramp.chroma_high_scale);
    ramp_data["lightness_low"] = round(color_ramp.lightness_low);
    ramp_data["lightness_high"] = round(color_ramp.lightness_high);

    json_data["color_ramps"].push_back(std::move(ramp_data));
  }

  if (std::ofstream fs{std::string{filepath}}; fs) {
    fs << json_data.dump(2) << '\n';
  }
}
