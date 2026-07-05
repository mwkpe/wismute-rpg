#include "debug_ui.h"


#include "imgui/imgui.h"


namespace {


void set_style(ImGuiStyle& style)
{
  style.WindowPadding = ImVec2{8, 8};
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 0.0f;
  style.FramePadding = ImVec2{3, 3};
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2{12, 8};
  style.ItemInnerSpacing = ImVec2{8, 6};
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 5.0f;
  style.GrabRounding = 5.0f;

  ImVec4 almostblack{0.03f, 0.03f, 0.03f, 1.0f};
  ImVec4 blackglass{0.05f, 0.05f, 0.05f, 0.9f};
  ImVec4 darkgrey{0.15f, 0.15f, 0.15f, 0.9f};
  ImVec4 grey{0.2f, 0.2f, 0.2f, 0.9f};
  ImVec4 lightgrey{0.4f, 0.4f, 0.4f, 0.9f};

  style.Colors[ImGuiCol_WindowBg] = blackglass;
  style.Colors[ImGuiCol_FrameBg] = darkgrey;
  style.Colors[ImGuiCol_FrameBgHovered] = grey;
  style.Colors[ImGuiCol_FrameBgActive] = grey;
  style.Colors[ImGuiCol_TextDisabled] = grey;
  style.Colors[ImGuiCol_TitleBg] = darkgrey;
  style.Colors[ImGuiCol_TitleBgCollapsed] = darkgrey;
  style.Colors[ImGuiCol_TitleBgActive] = grey;
  style.Colors[ImGuiCol_ResizeGrip] = darkgrey;
  style.Colors[ImGuiCol_ResizeGripHovered] = lightgrey;
  style.Colors[ImGuiCol_SliderGrab] = almostblack;
  style.Colors[ImGuiCol_SliderGrabActive] = lightgrey;
  style.Colors[ImGuiCol_Button] = darkgrey;
  style.Colors[ImGuiCol_ButtonHovered] = grey;
  style.Colors[ImGuiCol_ButtonActive] = lightgrey;
}


void set_main_color(ImGuiStyle& style, ImVec4 color)
{
  style.Colors[ImGuiCol_Border] = color;
  style.Colors[ImGuiCol_Text] = color;
  style.Colors[ImGuiCol_CheckMark] = color;
  style.Colors[ImGuiCol_ResizeGripActive] = color;
}


}  // namespace


void wis::Debug_ui::setup()
{
  ImGui::StyleColorsDark();
  set_style(ImGui::GetStyle());
  set_main_color(ImGui::GetStyle(), ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}


void wis::Debug_ui::build(Settings& settings, App_data& app_data, Game_data& game_data)
{
  auto& io = ImGui::GetIO();

  create_frame();
  {
    // Settings
    ImGui::Begin("Settings");

    ImGui::Text("Window");
    ImGui::Checkbox("Fullscreen", &settings.window.fullscreen);
    ImGui::Checkbox("Limit", &settings.render.limit_fps);
    ImGui::SameLine();
    ImGui::SliderInt("FPS", &settings.render.max_fps, 30, 300);
    ImGui::Dummy({0.0f, 8.0f});

    ImGui::Text("Debug");
    ImGui::Checkbox("Wireframe", &app_data.debug.wireframe);
    ImGui::Checkbox("Axes", &app_data.debug.show_axes);
    ImGui::Checkbox("Grid", &app_data.debug.show_stage_grid);
    ImGui::SameLine();
    ImGui::Checkbox("Grid UI", &app_data.debug.show_ui_grid);
    ImGui::Checkbox("Tile info", &app_data.debug.show_tile_info);
    ImGui::Dummy({0.0f, 8.0f});

    if (ImGui::Button("Quit (ESC)")) {
      app_data.state = App_state::Quit;
    }

    ImGui::End();
    ImGui::Begin("Status");

    ImGui::Text("App");
    ImGui::Text("Video driver: %s", app_data.window.video_driver.c_str());
    ImGui::Text("Window logical size: %i x %i", app_data.window.logical_width,
        app_data.window.logical_height);
    ImGui::Text("Window pixel size: %i x %i", app_data.window.pixel_width,
        app_data.window.pixel_height);
    ImGui::Text("Runtime: %llu s", app_data.timing.runtime_ns / 1'000'000'000ull);
    ImGui::Text("Framerate: %.0f fps", io.Framerate);
    ImGui::Text("Frametime: %.1f ms", 1000.0f / (io.Framerate > 0 ? io.Framerate : 1));
    ImGui::Dummy({0.0f, 8.0f});

    ImGui::Text("Stage cursor");
    ImGui::Text("Ground X: %.1f", game_data.cursor.stage.ground_position.x);
    ImGui::Text("Ground Z: %.1f", game_data.cursor.stage.ground_position.z);
    ImGui::Text("Scene position X: %.1f", game_data.cursor.stage.scene_position.x);
    ImGui::Text("Scene position Z: %.1f", game_data.cursor.stage.scene_position.z);
    ImGui::Text("Scene index: %u", game_data.cursor.stage.scene_index);
    ImGui::Text("Scene coords X: %u", game_data.cursor.stage.scene_coords.x);
    ImGui::Text("Scene coords Z: %u", game_data.cursor.stage.scene_coords.y);
    ImGui::Dummy({0.0f, 8.0f});

    ImGui::Text("UI cursor");
    ImGui::Text("Screen X: %.2f", game_data.cursor.ui.screen_position.x);
    ImGui::Text("Screen Z: %.2f", game_data.cursor.ui.screen_position.z);
    ImGui::Text("On panel: %s", game_data.cursor.ui.on_panel ? "yes" : "no");
    ImGui::Text("Panel X: %.2f", game_data.cursor.ui.panel_position.x);
    ImGui::Text("Panel Y: %.2f", game_data.cursor.ui.panel_position.y);

    ImGui::End();
  }

  has_mouse_ = io.WantCaptureMouse;
}
