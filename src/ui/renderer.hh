#ifndef FENRIR_SRC_UI_RENDERER_HH_
#define FENRIR_SRC_UI_RENDERER_HH_

#include <raylib.h>
#include "src/state/worldState/worldManager.hh"
#include "src/util/logger/logger.hh"

namespace fenrir {
namespace ui {

struct Camera {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  float zoom = 1.0f;
};

class Renderer {
 public:
  Renderer();
  ~Renderer();

  Texture2D getWorldTexture(WorldManager& world_manager, const Camera& camera);
  void updateCamera(Camera& camera, float delta);

 private:
  Logger logger_;
  Texture2D current_texture_;
  bool texture_loaded_ = false;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_RENDERER_HH_
