#ifndef FENRIR_SRC_UI_MAPMODE_STRATEGIES_PROVINCEDEBUGMODE_HH_
#define FENRIR_SRC_UI_MAPMODE_STRATEGIES_PROVINCEDEBUGMODE_HH_

#include "../map_mode_strategy_interface.hh"

namespace fenrir {
namespace ui {

class ProvinceDebugMode : public MapModeStrategyInterface {
 public:
  WorldManager::pixel getPixelColor(
      const WorldManager::province_t& province,
      const WorldManager& world_manager
  ) const override;
  
  std::string getName() const override { return "Province Debug"; }
  std::string getShortName() const override { return "Debug"; }
  std::string getDescription() const override {
    return "Show all province boundaries";
  }
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_MAPMODE_STRATEGIES_PROVINCEDEBUGMODE_HH_
