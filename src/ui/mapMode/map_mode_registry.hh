#ifndef FENRIR_SRC_UI_MAPMODE_MAPMODEREGISTRY_HH_
#define FENRIR_SRC_UI_MAPMODE_MAPMODEREGISTRY_HH_

#include <memory>
#include <unordered_map>
#include <functional>
#include <memory>
#include "map_mode_strategy_interface.hh"

namespace fenrir {
namespace ui {

class MapModeRegistry {
 public:
  using StrategyPtr = std::unique_ptr<MapModeStrategyInterface>;
  using Factory = std::function<StrategyPtr()>;
  
  void registerMode(const std::string& id, Factory factory);
  StrategyPtr create(const std::string& id) const;
  std::vector<std::string> getAllIds() const;
  
 private:
  std::unordered_map<std::string, Factory> modes_;
};

}  // namespace ui
}  // namespace fenrir

#endif  // FENRIR_SRC_UI_MAPMODE_MAPMODEREGISTRY_HH_
