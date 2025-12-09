#pragma once

#include <string>
#include "src/state/worldState/world_manager.hh"

namespace fenrir {
namespace ui {

class MapModeStrategyInterface {
 public:
  virtual ~MapModeStrategyInterface() = default;
  
  virtual WorldManager::pixel getPixelColor(
      const WorldManager::province_t& province,
      const WorldManager& world_manager
  ) const = 0;
  
  virtual std::string getName() const = 0;
  virtual std::string getShortName() const = 0;
  virtual std::string getDescription() const = 0;
};

}  // namespace ui
}  // namespace fenrir
