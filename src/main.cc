#include <raylib.h>
#include "src/ui/renderer.hh"
#include "src/ui/input_handler.hh"
#include "src/ui/ui_overlay.hh"
#include "src/state/worldState/worldManager.hh"
#include "src/util/logger/logger.hh"

int main() {
  fenrir::Logger logger;
  logger.log("=== Fenrir Grand Strategy Engine ===");
  logger.log("Initializing Raylib window...");

  const int screen_width = 800;
  const int screen_height = 600;

  InitWindow(screen_width, screen_height, "Fenrir - Grand Strategy");
  SetTargetFPS(60);

  logger.log("Window initialized: " + std::to_string(screen_width) + "x" + std::to_string(screen_height));

  fenrir::WorldManager world_manager;
  fenrir::ui::Renderer renderer;
  fenrir::ui::InputHandler input_handler;
  fenrir::ui::UIOverlay ui_overlay;

  ui_overlay.initialize();
  logger.log("All systems initialized");

  fenrir::ui::Camera camera;
  camera.width = screen_width;
  camera.height = screen_height;

  int tick_count = 0;

  logger.log("Entering main loop...");

  while (!WindowShouldClose()) {
    float delta = GetFrameTime();

    input_handler.update(camera, delta);

    if (input_handler.shouldTriggerDebug()) {
      logger.log("=== DEBUG TRIGGER ===");
      logger.log("Tick: " + std::to_string(tick_count));
      logger.log("Camera: (" + std::to_string(camera.x) + ", " + std::to_string(camera.y) + ")");
      logger.log("Zoom: " + std::to_string(camera.zoom));
      logger.log("FPS: " + std::to_string(GetFPS()));
      input_handler.clearDebugTrigger();
    }

    tick_count++;

    BeginDrawing();
    ClearBackground(BLACK);

    Texture2D world_texture = renderer.getWorldTexture(world_manager, camera);
    DrawTexture(world_texture, 0, 0, WHITE);

    ui_overlay.drawDebugInfo(tick_count, GetFPS(), camera.x, camera.y, camera.zoom);

    EndDrawing();
  }

  logger.log("Shutting down...");
  ui_overlay.shutdown();
  CloseWindow();
  logger.log("Fenrir terminated successfully");

  return 0;
}
