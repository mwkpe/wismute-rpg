#include "game.h"


#include "game/events.h"


wis::Game::Game(const App_data& app_data,
    Game_data& game_data)
    :
    app_data_{app_data},
    game_data_{game_data},
    stage_{registry_, dispatcher_, app_data, game_data, atlas_}
{
}


void wis::Game::init()
{
  atlas_.init();
  stage_.init();
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

  dispatcher_.update();
}


void wis::Game::render()
{
  stage_.render();
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
  stage_.handle_event(event);
}


void wis::Game::handle_event(const apeiron::engine::Mouse_button_up_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::handle_event(const apeiron::engine::Mouse_motion_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::handle_event(const apeiron::engine::Mouse_wheel_event& event)
{
  stage_.handle_event(event);
}


void wis::Game::on_achievement_unlocked([[maybe_unused]] const event::Achievement_unlocked& event)
{
}
