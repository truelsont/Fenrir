#include "src/ui/ui_overlay.hh"
#include <string>

namespace fenrir {
namespace ui {

UIOverlay::UIOverlay() {
  logger_.Log("UIOverlay created");
}

UIOverlay::~UIOverlay() {
  if (initialized_) {
    shutdown();
  }
}

void UIOverlay::initialize() {
  initialized_ = true;
  logger_.Log("UI Overlay initialized");
}

void UIOverlay::shutdown() {
  if (initialized_) {
    initialized_ = false;
    logger_.Log("UI Overlay shutdown");
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

void UIOverlay::drawLocationInfo(
    const WorldManager::location_t* location,
    const WorldManager::province_t* province,
    int mouse_x, 
    int mouse_y
) {
  if (!location || !province) return;
  
  Rectangle info_box = {
    static_cast<float>(mouse_x + 15),
    static_cast<float>(mouse_y + 15),
    220,
    160
  };
  
  DrawRectangleRec(info_box, Fade(BLACK, 0.90f));
  DrawRectangleLinesEx(info_box, 2, YELLOW);
  
  int text_y = info_box.y + 5;
  int x_pos = info_box.x + 5;
  
  DrawText("=== GEOGRAPHIC INFO ===", x_pos, text_y, 12, YELLOW);
  text_y += 18;
  
  DrawText("LOCATION", x_pos, text_y, 11, SKYBLUE);
  text_y += 15;
  std::string loc_id = "  ID: " + std::to_string(location->id);
  DrawText(loc_id.c_str(), x_pos, text_y, 10, WHITE);
  text_y += 15;
  
  DrawText("PROVINCE", x_pos, text_y, 11, GREEN);
  text_y += 15;
  std::string prov_id = "  ID: " + std::to_string(province->id);
  DrawText(prov_id.c_str(), x_pos, text_y, 10, WHITE);
  text_y += 12;
  std::string prov_locs = "  Locations: " + std::to_string(province->location_ids.size());
  DrawText(prov_locs.c_str(), x_pos, text_y, 10, LIGHTGRAY);
  text_y += 15;
  
  DrawText("POLITICAL", x_pos, text_y, 11, ORANGE);
  text_y += 15;
  std::string owner = "  Owner: " + std::to_string(province->owner);
  DrawText(owner.c_str(), x_pos, text_y, 10, WHITE);
  text_y += 12;
  std::string area = "  Area: " + std::to_string(province->area_id);
  DrawText(area.c_str(), x_pos, text_y, 10, LIGHTGRAY);
}

}  // namespace ui
}  // namespace fenrir
