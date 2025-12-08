#ifndef FENRIR_SRC_UI_UI_OVERLAY_HH_
#define FENRIR_SRC_UI_UI_OVERLAY_HH_

#include <raylib.h>
#include "src/util/logger/logger.hh"

namespace fenrir {
namespace ui {

class UIOverlay {
 public:
  UIOverlay();
  ~UIOverlay();

  void initialize();
  void shutdown();
  void drawDebugInfo(int tick_count, float fps, int camera_x, int camera_y, float zoom);

 private:
  Logger logger_;
  bool initialized_ = false;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_UI_OVERLAY_HH_
