cmake_minimum_required(VERSION 3.20)

project(wismute C CXX)  # Must specify C for glad library

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror=return-type")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif ()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/apeiron/cmake")

include(apeiron)
include(apeiron-gui-sdl3-opengl3)

add_executable(wismute
    source/main.cpp
    source/app/app.cpp
    source/app/app.h
    source/app/app_data.h
    source/app/debug_ui.cpp
    source/app/debug_ui.h
    source/app/error.h
    source/app/settings.cpp
    source/app/settings.h
    source/core/atlas.cpp
    source/core/atlas.h
    source/core/constants.h
    source/core/lattice.h
    source/core/palette.h
    source/core/pixel_renderer.cpp
    source/core/pixel_renderer.h
    source/core/renderer.cpp
    source/core/renderer.h
    source/game/constants.h
    source/game/events.h
    source/game/game.cpp
    source/game/game.h
    source/game/game_data.h
    source/game/game_ui.cpp
    source/game/game_ui.h
    source/game/path_finder.cpp
    source/game/path_finder.h
    source/game/player.h
    source/game/scene.cpp
    source/game/scene.h
    source/game/slime.h
    source/game/sprite.h
    source/game/stage.cpp
    source/game/stage.h
    source/game/tile.h
    source/game/types.h
    source/ui/action_panel.cpp
    source/ui/action_panel.h
    source/ui/panel.cpp
    source/ui/panel.h
    source/ui/quad.h
    source/ui/widget.h
)

target_include_directories(wismute PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(wismute PRIVATE
    apeiron
    apeiron-gui-sdl3-opengl3)
