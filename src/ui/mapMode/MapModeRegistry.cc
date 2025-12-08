#include "MapModeRegistry.hh"

namespace fenrir {
namespace ui {

void MapModeRegistry::registerMode(const std::string& id, Factory factory) {
  modes_[id] = factory;
}

MapModeRegistry::StrategyPtr MapModeRegistry::create(const std::string& id) const {
  auto it = modes_.find(id);
  if (it != modes_.end()) {
    return it->second();
  }
  return nullptr;
}

std::vector<std::string> MapModeRegistry::getAllIds() const {
  std::vector<std::string> ids;
  ids.reserve(modes_.size());
  for (const auto& pair : modes_) {
    ids.push_back(pair.first);
  }
  return ids;
}

}  // namespace ui
}  // namespace fenrir
