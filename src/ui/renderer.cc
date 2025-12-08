#include "src/ui/renderer.hh"
#include <vector>

namespace fenrir {
namespace ui {

Renderer::Renderer() {
  logger_.log("Renderer initialized");
}

Renderer::~Renderer() {
  if (texture_loaded_) {
    UnloadTexture(current_texture_);
    logger_.log("Renderer texture unloaded");
  }
}

Texture2D Renderer::getWorldTexture(WorldManager& world_manager, const Camera& camera) {
  if (texture_loaded_) {
    UnloadTexture(current_texture_);
  }

  WorldManager::rendering_options_t options;
  options.map_mode = WorldManager::kPROVINCE_DEBUG;
  options.view_port_x = camera.x;
  options.view_port_y = camera.y;
  options.view_port_width = camera.width;
  options.view_port_height = camera.height;

  auto pixel_data = world_manager.createImageData(options);
  
  if (pixel_data.empty()) {
    logger_.log("ERROR: Failed to create world texture from WorldManager");
    texture_loaded_ = false;
    return current_texture_;
  }

  Image raylib_image = {
    .data = reinterpret_cast<void*>(pixel_data.data()),
    .width = camera.width,
    .height = camera.height,
    .mipmaps = 1,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
  };

  current_texture_ = LoadTextureFromImage(raylib_image);
  texture_loaded_ = true;

  return current_texture_;
}

void Renderer::updateCamera(Camera& camera, float delta) {
}

}  // namespace ui
}  // namespace fenrir
