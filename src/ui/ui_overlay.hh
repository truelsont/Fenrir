#ifndef FENRIR_SRC_UI_UI_OVERLAY_HH_
#define FENRIR_SRC_UI_UI_OVERLAY_HH_

#include <raylib.h>
#include "src/util/logger/logger.hh"
#include "src/state/worldState/world_manager.hh"

namespace fenrir {
namespace ui {

class UIOverlay {
 public:
  UIOverlay();
  ~UIOverlay();

  void initialize();
  void shutdown();
  void drawDebugInfo(int tick_count, float fps, int camera_x, int camera_y, float zoom);
  void drawLocationInfo(
      const WorldManager::location_t* location,
      const WorldManager::province_t* province,
      int mouse_x, 
      int mouse_y
  );

 private:
  Logger logger_;
  bool initialized_ = false;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_UI_OVERLAY_HH_
