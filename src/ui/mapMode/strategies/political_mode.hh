#ifndef FENRIR_SRC_UI_MAPMODE_STRATEGIES_POLITICALMODE_HH_
#define FENRIR_SRC_UI_MAPMODE_STRATEGIES_POLITICALMODE_HH_

#include "../map_mode_strategy_interface.hh"

namespace fenrir {
namespace ui {

class PoliticalMode : public MapModeStrategyInterface {
 public:
  WorldManager::pixel getPixelColor(
      const WorldManager::province_t& province,
      const WorldManager& world_manager
  ) const override;
  
  std::string getName() const override { return "Political Map"; }
  std::string getShortName() const override { return "Political"; }
  std::string getDescription() const override {
    return "Province ownership by nation";
  }
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_MAPMODE_STRATEGIES_POLITICALMODE_HH_
