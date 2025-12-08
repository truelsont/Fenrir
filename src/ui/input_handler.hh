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
  float zoom_speed_ = 0.1f;
  float min_zoom_ = 0.5f;
  float max_zoom_ = 3.0f;
  int map_width_ = 1280;
  int map_height_ = 720;
  bool trigger_debug_ = false;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_INPUT_HANDLER_HH_
