#include "settings.h"


#include <fstream>
#include <string>
#include "toml.hpp"
#include "app/error.h"


auto wis::load_settings(std::string_view filepath) -> Settings
{
  std::ifstream fs{std::string{filepath}};
  if (!fs.is_open()) {
    throw Warning::format("Could not open settings file: {}", filepath);
  }

  toml::table t;
  try {
    t = toml::parse_file(filepath);
  }
  catch (const toml::parse_error& e) {
    throw Error::format("Settings file parse error: {}", e.what());
  }

  Settings s;  // Contains default values

  if (auto* section = t["window"].as_table(); section) {
    auto sec = *section;
    s.window.width = sec["width"].value_or(s.window.width);
    s.window.height = sec["height"].value_or(s.window.height);
    s.window.fullscreen = sec["fullscreen"].value_or(s.window.fullscreen);
    s.window.ignore_scaling = sec["ignore_scaling"].value_or(s.window.ignore_scaling);
  }

  if (auto* section = t["render"].as_table(); section) {
    auto sec = *section;
    s.render.msaa_samples = sec["msaa_samples"].value_or(s.render.msaa_samples);
    s.render.max_fps = sec["max_fps"].value_or(s.render.max_fps);
    s.render.vsync = sec["vsync"].value_or(s.render.vsync);
    s.render.limit_fps = sec["limit_fps"].value_or(s.render.limit_fps);
  }

  return s;
}


void wis::save_settings(const Settings& s, std::string_view filepath)
{
  toml::table t;

  toml::table window;
  window.insert("width", s.window.width);
  window.insert("height", s.window.height);
  window.insert("fullscreen", s.window.fullscreen);
  window.insert("ignore_scaling", s.window.ignore_scaling);
  t.insert("window", window);

  toml::table render;
  render.insert("msaa_samples", s.render.msaa_samples);
  render.insert("max_fps", s.render.max_fps);
  render.insert("vsync", s.render.vsync);
  render.insert("limit_fps", s.render.limit_fps);
  t.insert("render", render);

  if (std::ofstream fs{std::string{filepath}, std::ios::binary}; fs.is_open()) {
    fs << t << '\n';
  }
}
