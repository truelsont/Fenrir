#include "province_debug_mode.hh"

namespace fenrir {
namespace ui {

WorldManager::pixel ProvinceDebugMode::getPixelColor(
    const WorldManager::province_t& province,
    const WorldManager& world_manager
) const {
  return province.color;
}

}  // namespace ui
}  // namespace fenrir
