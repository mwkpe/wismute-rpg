#include "game.h"


#include "apeiron/engine/color_converter.h"
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
}


void wis::Game::init()
{
  Renderer::gl_init();
  Renderer::set_gl_frame_buffer(0);
  Renderer::set_gl_viewport(0, 0, app_data_.window.pixel_width, app_data_.window.pixel_height);

  atlas_.init();
  stage_.init();
  stage_ui_.init();

  scene_.load_scene("assets/test_scene.json");
  stage_.init_scene();
  stage_ui_.set_spells(scene_.spells());

  dispatcher_.sink<event::Achievement_unlocked>().connect<&Game::on_achievement_unlocked>(*this);
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
}


void wis::Game::render()
{
  // Palette 48 but darker (monochromatic - 1)
  Renderer::gl_clear(apeiron::engine::as_rgb_norm("#1d171c"));

  stage_.render();
  stage_ui_.render();
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
