#include "app.h"


#include <iostream>
#include <fmt/format.h>
#include <SDL3/SDL.h>

#include "apeiron/engine/event.h"
#include "apeiron/engine/sdl_input.h"

#include "app/debug_ui.h"
#include "app/error.h"
#include "game/game.h"
#include "game/game_data.h"


namespace {


std::uint64_t handle_timing(const auto& render, auto& timing, std::uint64_t last_ticks_ns)
{
  std::uint64_t ticks_ns = SDL_GetTicksNS();
  std::uint64_t elapsed_ns = ticks_ns - last_ticks_ns;
  const std::uint64_t target_ns = 1'000'000'000ull / render.max_fps;

  if (render.limit_fps && elapsed_ns < target_ns) {
    SDL_DelayPrecise(target_ns - elapsed_ns);
    ticks_ns = SDL_GetTicksNS();
    elapsed_ns = ticks_ns - last_ticks_ns;
  }

  timing.runtime_ns += elapsed_ns;
  timing.elapsed_ns = elapsed_ns;
  timing.delta_s = static_cast<float>(static_cast<double>(elapsed_ns) / 1'000'000'000.0);
  timing.runtime_s += timing.delta_s;

  if (elapsed_ns > 50'000'000ull) {
    const auto elapsed_ms = elapsed_ns / 1'000'000ull;
    std::cout << fmt::format("Frametime spike of {} ms", elapsed_ms) << std::endl;
  }

  // Lag, e.g. user dragged window (apparently only an issue on Windows)
  if (elapsed_ns > 150'000'000ull) {
    timing.elapsed_ns = 0;
    timing.delta_s = 0.0f;
  }

  return ticks_ns;
}


void handle_event(auto* window, const SDL_Event& sdl_event, apeiron::engine::Event_queue& events,
    auto& app_data, bool process_mouse_input)
{
  namespace engine = apeiron::engine;

  switch (sdl_event.type) {
    case SDL_EVENT_KEY_DOWN: {
      if (sdl_event.key.repeat == 0) {
        events.emplace_back(engine::Key_down_event{sdl_event.key.key, sdl_event.key.mod});

        switch (sdl_event.key.key) {
          case SDLK_ESCAPE: {
            app_data.state = wis::App_state::Quit;
          }
          break;
          case SDLK_F1: {
            app_data.debug.show_ui = !app_data.debug.show_ui;
          }
          break;
          case SDLK_F2: {
            app_data.debug.noclip = !app_data.debug.noclip;

            if (app_data.debug.noclip) {
              SDL_GetRelativeMouseState(nullptr, nullptr);  // Prevent erroneous movement
              SDL_CaptureMouse(true);
              SDL_SetWindowRelativeMouseMode(window, true);
            }
            else {
              SDL_CaptureMouse(false);
              SDL_SetWindowRelativeMouseMode(window, false);
            }
          }
          break;
          default:;
        }
      }
    }
    break;
    case SDL_EVENT_KEY_UP: {
      events.emplace_back(engine::Key_up_event{sdl_event.key.key, sdl_event.key.mod});
    }
    break;
    case SDL_EVENT_MOUSE_MOTION: {
      if (process_mouse_input) {
        events.emplace_back(engine::Mouse_motion_event{sdl_event.motion.x, sdl_event.motion.y,
            sdl_event.motion.xrel, sdl_event.motion.yrel});
      }
    }
    break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      if (process_mouse_input) {
        events.emplace_back(engine::Mouse_button_down_event{
            engine::get_mouse_button(sdl_event.button.button),
            sdl_event.button.x,
            sdl_event.button.y});
      }
    }
    break;
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      if (process_mouse_input) {
        events.emplace_back(engine::Mouse_button_up_event{
            engine::get_mouse_button(sdl_event.button.button),
            sdl_event.button.x,
            sdl_event.button.y});
      }
    }
    break;
    case SDL_EVENT_MOUSE_WHEEL: {
      events.emplace_back(engine::Mouse_wheel_event{sdl_event.wheel.x, sdl_event.wheel.y});
    }
    break;
    default:;
  }
}


}  // namespace


wis::App::App() : app_data_{}, window_{} {}


wis::App::~App()
{
  std::cout << "App quit" << std::endl;
}


void wis::App::init()
{
  settings_ = {};
  app_data_ = {};

  try {
    settings_ = wis::load_settings("settings.toml");
  }
  catch (const wis::Warning& e) {
    std::cout << e.what() << std::endl;
  }

  window_.init({
      .title = "Wismute",
      .init_flags = SDL_INIT_VIDEO,
      .width = settings_.window.width,
      .height = settings_.window.height,
      .ignore_scaling = settings_.window.ignore_scaling,
      .resizable = false,
      .fullscreen = settings_.window.fullscreen,
      .vsync = settings_.render.vsync,
      .msaa_samples = settings_.render.msaa_samples,
      .gl_major = 3,
      .gl_minor = 3,
      .gl_core = true
  });

  auto wa = window_.attributes();

  app_data_.window.logical_width = wa.logical_width;
  app_data_.window.logical_height = wa.logical_height;
  app_data_.window.pixel_width = wa.pixel_width;
  app_data_.window.pixel_height = wa.pixel_height;
  app_data_.window.density = wa.density;
  app_data_.window.scale = wa.scale;
  app_data_.window.aspect_ratio = static_cast<float>(wa.pixel_width) /
      static_cast<float>(wa.pixel_height);
  app_data_.window.video_driver = SDL_GetCurrentVideoDriver();
}


int wis::App::exec()
{
  auto window = std::move(window_);  // Close window when exec returns

  Debug_ui debug_ui{window.sdl_window(), window.gl_context()};
  debug_ui.init(3, 3);
  debug_ui.setup();

  Game_data game_data;
  Game game{app_data_, game_data};

  try {
    game.init();
  }
  catch (const Error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::uint64_t last_ticks_ns = SDL_GetTicksNS();
  apeiron::engine::Event_queue events;

  while (app_data_.state != App_state::Quit) {
    last_ticks_ns = handle_timing(settings_.render, app_data_.timing, last_ticks_ns);
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {
      if (sdl_event.type == SDL_EVENT_QUIT) {
        app_data_.state = App_state::Quit;
        std::cout << "Closing app..." << std::endl;
      }

      if (app_data_.debug.show_ui) {
        debug_ui.process(&sdl_event);
      }

      handle_event(window.sdl_window(), sdl_event, events, app_data_,
          !app_data_.debug.show_ui || !debug_ui.has_mouse());
    }

    auto input_state = apeiron::engine::get_input_state();
    game.update(events, &input_state);
    game.render();

    if (app_data_.debug.show_ui) {
      debug_ui.build(settings_, app_data_, game_data);
      debug_ui.render();
    }

    events.clear();
    SDL_GL_SwapWindow(window.sdl_window());
  }

  wis::save_settings(settings_, "settings.toml");

  return 0;
}
