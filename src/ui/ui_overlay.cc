#include "src/ui/ui_overlay.hh"
#include <string>

namespace fenrir {
namespace ui {

UIOverlay::UIOverlay() {
  logger_.log("UIOverlay created");
}

UIOverlay::~UIOverlay() {
  if (initialized_) {
    shutdown();
  }
}

void UIOverlay::initialize() {
  initialized_ = true;
  logger_.log("UI Overlay initialized");
}

void UIOverlay::shutdown() {
  if (initialized_) {
    initialized_ = false;
    logger_.log("UI Overlay shutdown");
  }
}

void UIOverlay::drawDebugInfo(int tick_count, float fps, int camera_x, int camera_y, float zoom) {
  std::string info = "Fenrir - Ticks: " + std::to_string(tick_count) + 
                     " | FPS: " + std::to_string(static_cast<int>(fps)) +
                     " | Camera: (" + std::to_string(camera_x) + "," + std::to_string(camera_y) + ")" +
                     " | Zoom: " + std::to_string(zoom);
  
  DrawText(info.c_str(), 10, 10, 20, WHITE);
  DrawText("Controls: Arrows=Pan, Wheel=Zoom, Space=Debug", 10, 40, 16, LIGHTGRAY);
}

}  // namespace ui
}  // namespace fenrir
