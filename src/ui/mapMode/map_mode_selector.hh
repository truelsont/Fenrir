#ifndef FENRIR_SRC_UI_MAPMODE_MAPMODESELECTOR_HH_
#define FENRIR_SRC_UI_MAPMODE_MAPMODESELECTOR_HH_

#include <raylib.h>
#include <vector>
#include <raylib.h>
#include "map_mode_registry.hh"
#include "src/util/logger/logger.hh"

namespace fenrir {
namespace ui {

struct MapModeButton {
  std::string mode_id;
  std::string label;
  std::string description;
  Rectangle bounds;
};

class MapModeSelector {
 public:
  MapModeSelector(MapModeRegistry& registry, Logger& logger);
  
  void initialize(int screen_width, int screen_height);
  void update(float delta);
  void draw();
  void handleInput();
  const MapModeStrategyInterface* getCurrentStrategy() const { return current_strategy_.get(); }
  
 private:
  MapModeRegistry& registry_;
  Logger& logger_;
  
  std::vector<MapModeButton> buttons_;
  std::unique_ptr<MapModeStrategyInterface> current_strategy_;
  std::string current_mode_id_;
  
  int hovered_index_;
  float hover_time_;
  
  void buildButtons(int screen_width, int screen_height);
  void switchMode(const std::string& mode_id);
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_MAPMODE_MAPMODESELECTOR_HH_
