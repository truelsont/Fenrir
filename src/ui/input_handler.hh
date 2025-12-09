#ifndef FENRIR_SRC_UI_INPUT_HANDLER_HH_
#define FENRIR_SRC_UI_INPUT_HANDLER_HH_

#include <raylib.h>
#include "src/ui/renderer.hh"
#include "src/util/logger/logger.hh"

namespace fenrir {
namespace ui {

class InputHandler {
 public:
  InputHandler();
  ~InputHandler();

  void update(Camera& camera, float delta);
  bool shouldTriggerDebug() const { return trigger_debug_; }
  void clearDebugTrigger() { trigger_debug_ = false; }

 private:
  Logger logger_;
  float camera_speed_ = 300.0f;
  float zoom_speed_ = 0.2f;
  float min_zoom_ = 0.5f;
  float max_zoom_ = 100.0f;
  int map_width_ = 320;
  int map_height_ = 180;
  bool trigger_debug_ = false;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_INPUT_HANDLER_HH_
