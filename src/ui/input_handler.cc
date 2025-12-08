#include "src/ui/input_handler.hh"
#include <algorithm>

namespace fenrir {
namespace ui {

InputHandler::InputHandler() {
  logger_.log("InputHandler initialized");
}

InputHandler::~InputHandler() {
  logger_.log("InputHandler destroyed");
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
    camera.zoom += wheel * zoom_speed_;
    camera.zoom = std::clamp(camera.zoom, min_zoom_, max_zoom_);
    logger_.log("Zoom level: " + std::to_string(camera.zoom));
  }

  if (IsKeyPressed(KEY_SPACE)) {
    trigger_debug_ = true;
    logger_.log("Debug trigger activated (Space key pressed)");
  }
}

}  // namespace ui
}  // namespace fenrir
