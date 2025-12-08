#include "ProvinceDebugMode.hh"

namespace fenrir {
namespace ui {

WorldManager::pixel ProvinceDebugMode::getPixelColor(
    const WorldManager::province_t& province,
    const WorldManager& world_manager
) const {
  uint32_t owner = province.owner;
  uint8_t r = static_cast<uint8_t>((owner * 73) % 256);
  uint8_t g = static_cast<uint8_t>((owner * 151) % 256);
  uint8_t b = static_cast<uint8_t>((owner * 223) % 256);
  return {r, g, b, 255};
}

}  // namespace ui
}  // namespace fenrir
