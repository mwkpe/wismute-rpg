#include "game.h"


#include <print>

#include "apeiron/engine/color_converter.h"
#include "core/color_math.h"
#include "core/color_ramp.h"
#include "core/palette.h"
#include "core/renderer.h"
#include "game/events.h"


wis::Game::Game(const App_data& app_data,
    Game_data& game_data)
    :
    app_data_{app_data},
    game_data_{game_data},
    stage_{registry_, dispatcher_, app_data, game_data, atlas_, scene_},
    stage_ui_{registry_, dispatcher_, app_data, game_data, atlas_}
{
  dispatcher_.sink<event::Achievement_unlocked>().connect<&Game::on_achievement_unlocked>(*this);
}


void wis::Game::init()
{
  Renderer::gl_init();
  Renderer::set_gl_frame_buffer(0);
  Renderer::set_gl_viewport(0, 0, app_data_.window.pixel_width, app_data_.window.pixel_height);

  init_palette();

  atlas_.init();
  stage_.init();
  stage_ui_.init();

  scene_.load_scene("assets/test_scene.json");
  stage_.init_scene();
  stage_ui_.set_spells(scene_.spell_slots());
}


void wis::Game::update(const apeiron::engine::Event_queue& engine_events,
    [[maybe_unused]] const apeiron::engine::Input* input)
{
  for (const auto& event : engine_events) {
    std::visit(*this, event);
  }

  stage_.update_input(input);
  stage_.update();
  stage_ui_.update(app_data_.timing.delta_s);

  dispatcher_.update();

  if (game_data_.color.live_update_palette) {
    update_palette();
  }
}


void wis::Game::render()
{
  Renderer::gl_clear(game_data_.color.palette[2]);

  stage_.render();
  stage_ui_.render();
}


void wis::Game::init_palette()
{
  game_data_.color.palette.fill(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
  game_data_.color.ramps = wis::read_color_ramps("color_ramps.json");
  update_palette();

  //auto& color_ramps = game_data_.color.ramps;

  //color_ramps.clear();
  //color_ramps.emplace_back("A", 4);
  //color_ramps.emplace_back("B", 4);
  //color_ramps.emplace_back("C", 4);
  //color_ramps.emplace_back("D", 4);
  //color_ramps.emplace_back("E", 4);
}


void wis::Game::update_palette()
{
  auto& palette = game_data_.color.palette;
  auto& color_ramps = game_data_.color.ramps;

  std::size_t palette_index = 1;  // Jump over invalid color

  for (const auto& ramp : color_ramps) {
    for (std::uint32_t i=0; i<ramp.steps; ++i) {
      if (auto color = calculate_color_step(ramp, i); color && palette_index < palette.size()) {
        palette[palette_index++] = *color;
      }
    }
  }
}


void wis::Game::handle_event(const apeiron::engine::Key_down_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::handle_event(const apeiron::engine::Key_up_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::handle_event(const apeiron::engine::Mouse_button_down_event& event)
{
  if (!stage_ui_.handle_event(event)) {
    stage_.handle_event(event);
  }
}


void wis::Game::handle_event(const apeiron::engine::Mouse_button_up_event& event)
{
  if (!stage_ui_.handle_event(event)) {
    stage_.handle_event(event);
  }
}


void wis::Game::handle_event(const apeiron::engine::Mouse_motion_event& event)
{
  if (!stage_ui_.handle_event(event)) {
    stage_.handle_event(event);
  }
}


void wis::Game::handle_event(const apeiron::engine::Mouse_wheel_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::on_achievement_unlocked([[maybe_unused]] const event::Achievement_unlocked& event)
{
}
