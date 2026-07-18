#include "src/ui/input_handler.hh"
#include <algorithm>
#include <cmath>

namespace fenrir {
namespace ui {

InputHandler::InputHandler() {
  logger_.Log("InputHandler initialized");
}

InputHandler::~InputHandler() {
  logger_.Log("InputHandler destroyed");
}

void InputHandler::update(Camera& camera, float delta) {
  Vector2 velocity = {0.0f, 0.0f};

  if (IsKeyDown(KEY_RIGHT)) velocity.x += 1.0f;
  if (IsKeyDown(KEY_LEFT)) velocity.x -= 1.0f;
  if (IsKeyDown(KEY_DOWN)) velocity.y += 1.0f;
  if (IsKeyDown(KEY_UP)) velocity.y -= 1.0f;

  if (velocity.x != 0.0f || velocity.y != 0.0f) {
    float length = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
    velocity.x /= length;
    velocity.y /= length;

    camera.x += static_cast<int>(velocity.x * camera_speed_ * delta);
    camera.y += static_cast<int>(velocity.y * camera_speed_ * delta);

    camera.x = std::clamp(camera.x, 0, map_width_ - camera.width);
    camera.y = std::clamp(camera.y, 0, map_height_ - camera.height);
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0.0f) {
    float old_zoom = camera.zoom;
    camera.zoom += wheel * zoom_speed_;
    camera.zoom = std::clamp(camera.zoom, 1.0f, max_zoom_);
    
    int base_width = 800;
    int base_height = 600;
    
    int new_width = static_cast<int>(base_width / camera.zoom);
    int new_height = static_cast<int>(base_height / camera.zoom);
    
    new_width = std::clamp(new_width, 3, map_width_);
    new_height = std::clamp(new_height, 3, map_height_);
    
    int center_x = camera.x + camera.width / 2;
    int center_y = camera.y + camera.height / 2;
    
    camera.width = new_width;
    camera.height = new_height;
    
    camera.x = center_x - camera.width / 2;
    camera.y = center_y - camera.height / 2;
    
    camera.x = std::clamp(camera.x, 0, map_width_ - camera.width);
    camera.y = std::clamp(camera.y, 0, map_height_ - camera.height);
    
    logger_.Log("Zoom: " + std::to_string(camera.zoom) + " Viewport: " + 
                std::to_string(camera.width) + "x" + std::to_string(camera.height));
  }

  if (IsKeyPressed(KEY_SPACE)) {
    trigger_debug_ = true;
    logger_.Log("Debug trigger activated (Space key pressed)");
  }
}

}  // namespace ui
}  // namespace fenrir
