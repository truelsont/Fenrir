#include "PoliticalMode.hh"

namespace fenrir {
namespace ui {

WorldManager::pixel PoliticalMode::getPixelColor(
    const WorldManager::province_t& province,
    const WorldManager& world_manager
) const {
  return world_manager.owner_to_color_map[
      province.owner % world_manager.owner_to_color_map.size()
  ];
}

}  // namespace ui
}  // namespace fenrir
