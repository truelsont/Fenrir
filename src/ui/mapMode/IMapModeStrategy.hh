#ifndef FENRIR_SRC_UI_MAPMODE_IMAPMODESTRATEGY_HH_
#define FENRIR_SRC_UI_MAPMODE_IMAPMODESTRATEGY_HH_

#include <string>
#include "src/state/worldState/worldManager.hh"

namespace fenrir {
namespace ui {

class IMapModeStrategy {
 public:
  virtual ~IMapModeStrategy() = default;
  
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

#endif  // FENRIR_SRC_UI_MAPMODE_IMAPMODESTRATEGY_HH_
