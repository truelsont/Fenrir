#include <raylib.h>
#include "src/ui/renderer.hh"
#include "src/ui/input_handler.hh"
#include "src/ui/ui_overlay.hh"
#include "src/ui/mapMode/map_mode_selector.hh"
#include "src/ui/mapMode/map_mode_registry.hh"
#include "src/ui/mapMode/strategies/political_mode.hh"
#include "src/ui/mapMode/strategies/province_debug_mode.hh"
#include "src/state/worldState/world_manager.hh"
#include "src/util/logger/logger.hh"


int init() {
  //parse CLI
  //load config 
  //logging level - Boost.Log auto-initializes
  return 0;
}

int main() {
  fenrir::Logger logger;

  const int screen_width = 800;
  const int screen_height = 600;

  InitWindow(screen_width, screen_height, "Fenrir");
  SetTargetFPS(60);


  fenrir::WorldManager world_manager;
  
  fenrir::ui::MapModeRegistry registry;
  registry.registerMode("political", []() {
    return std::make_unique<fenrir::ui::PoliticalMode>();
  });
  registry.registerMode("debug", []() {
    return std::make_unique<fenrir::ui::ProvinceDebugMode>();
  });
  
  fenrir::ui::Renderer renderer;
  fenrir::ui::InputHandler input_handler;
  fenrir::ui::UIOverlay ui_overlay;
  fenrir::ui::MapModeSelector map_mode_selector(registry, logger);

  ui_overlay.initialize();
  map_mode_selector.initialize(screen_width, screen_height);
  logger.Info("All systems initialized");

  fenrir::ui::Camera camera;
  camera.x = 0;
  camera.y = 0;
  camera.width = world_manager.width;
  camera.height = world_manager.height;
  camera.zoom = 1.0f;

  int tick_count = 0;

  logger.Debug("Entering main sim loop");

  while (!WindowShouldClose()) {
    float delta = GetFrameTime();

    input_handler.update(camera, delta);
    map_mode_selector.update(delta);

    if (input_handler.shouldTriggerDebug()) {
      logger.Trace("=== DEBUG TRIGGER ===");
      logger.Trace("Tick: " + std::to_string(tick_count));
      logger.Trace("Camera: (" + std::to_string(camera.x) + ", " + std::to_string(camera.y) + ")");
      logger.Trace("Zoom: " + std::to_string(camera.zoom));
      logger.Trace("FPS: " + std::to_string(GetFPS()));
      input_handler.clearDebugTrigger();
    }

    tick_count++;

    BeginDrawing();
    ClearBackground(BLACK);

    Texture2D world_texture = renderer.getWorldTexture(
        world_manager, 
        camera,
        map_mode_selector.getCurrentStrategy()
    );
    
    DrawTexture(world_texture, 0, 0, WHITE);

    Vector2 mouse_pos = GetMousePosition();
    int world_x = camera.x + (static_cast<int>(mouse_pos.x) * camera.width) / screen_width;
    int world_y = camera.y + (static_cast<int>(mouse_pos.y) * camera.height) / screen_height;
    
    auto* hovered_location = world_manager.getLocationAt(world_x, world_y);
    const fenrir::WorldManager::province_t* hovered_province = nullptr;
    if (hovered_location) {
      hovered_province = world_manager.getProvinceById(hovered_location->province_id);
    }

    map_mode_selector.draw();
    ui_overlay.drawDebugInfo(tick_count, GetFPS(), camera.x, camera.y, camera.zoom);
    
    if (hovered_location && hovered_province) {
      ui_overlay.drawLocationInfo(hovered_location, hovered_province, mouse_pos.x, mouse_pos.y);
    }

    EndDrawing();
  }

  ui_overlay.shutdown();
  CloseWindow();
  logger.Info("Fenrir terminated successfully");

  return 0;
}
