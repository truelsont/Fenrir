#include "src/ui/renderer.hh"
#include <vector>

namespace fenrir {
namespace ui {

Renderer::Renderer() {
  logger_.Info("Renderer initialized");
}

Renderer::~Renderer() {
  if (texture_loaded_) {
    UnloadTexture(current_texture_);
    logger_.Log("Renderer texture unloaded");
  }
}

Texture2D Renderer::getWorldTexture(
    WorldManager& world_manager, 
    const Camera& camera,
    const MapModeStrategyInterface* strategy
) {
  if (texture_loaded_) {
    UnloadTexture(current_texture_);
  }

  if (!strategy) {
    logger_.Log("ERROR: No map mode strategy provided");
    texture_loaded_ = false;
    return current_texture_;
  }

  const int render_width = 800;
  const int render_height = 600;
  
  std::vector<WorldManager::pixel> pixel_data(render_width * render_height);
  
  // Simple grid borders - always show
  uint8_t border_mask = 1; // Always check province borders (which is every location)
  WorldManager::pixel border_color = {30, 30, 30, 255}; // Dark gray grid lines
  
  for (int screen_y = 0; screen_y < render_height; screen_y++) {
    for (int screen_x = 0; screen_x < render_width; screen_x++) {
      int world_x = camera.x + (screen_x * camera.width) / render_width;
      int world_y = camera.y + (screen_y * camera.height) / render_height;
      
      auto* location = world_manager.getLocationAt(world_x, world_y);
      if (!location) {
        pixel_data[screen_y * render_width + screen_x] = {0, 0, 0, 255};
        continue;
      }
      
      auto* province = world_manager.getProvinceById(location->province_id);
      if (!province) {
        pixel_data[screen_y * render_width + screen_x] = {0, 0, 0, 255};
        continue;
      }
      
      // Check if we're on the right or bottom edge of the world (draw grid line)
      bool is_right_edge = (world_x + 1 >= static_cast<int>(world_manager.width));
      bool is_bottom_edge = (world_y + 1 >= static_cast<int>(world_manager.height));
      
      // Draw grid line on right and bottom edges of each pixel
      if (is_right_edge || is_bottom_edge) {
        pixel_data[screen_y * render_width + screen_x] = border_color;
      } else {
        // Check if neighbor is different (creates grid effect)
        auto* right_loc = world_manager.getLocationAt(world_x + 1, world_y);
        auto* bottom_loc = world_manager.getLocationAt(world_x, world_y + 1);
        
        bool draw_border = false;
        if (right_loc && right_loc->province_id != location->province_id) draw_border = true;
        if (bottom_loc && bottom_loc->province_id != location->province_id) draw_border = true;
        
        if (draw_border) {
          pixel_data[screen_y * render_width + screen_x] = border_color;
        } else {
          pixel_data[screen_y * render_width + screen_x] = strategy->getPixelColor(*province, world_manager);
        }
      }
    }
  }

  Image raylib_image = {
    .data = reinterpret_cast<void*>(pixel_data.data()),
    .width = render_width,
    .height = render_height,
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
