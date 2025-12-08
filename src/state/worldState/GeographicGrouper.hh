#ifndef FENRIR_SRC_STATE_WORLDSTATE_GEOGRAPHICGROUPER_HH_
#define FENRIR_SRC_STATE_WORLDSTATE_GEOGRAPHICGROUPER_HH_

#include <vector>
#include <unordered_map>
#include <cstdint>

namespace fenrir {

template<typename ChildType, typename ParentType>
class GeographicGrouper {
 public:
  struct GroupingConfig {
    int min_children_per_parent = 5;
    int max_children_per_parent = 10;
    bool use_spatial_proximity = true;
  };

  GeographicGrouper(GroupingConfig config = GroupingConfig()) : config_(config) {}

  std::vector<ParentType> groupChildren(
      const std::vector<ChildType>& children,
      std::function<void(const ChildType&, float&, float&)> getCentroid,
      std::function<void(ParentType&, uint32_t, const std::vector<uint32_t>&)> initParent
  );

 private:
  GroupingConfig config_;
  
  float computeDistance(float x1, float y1, float x2, float y2);
  std::vector<uint32_t> findNearestChildren(
      const std::vector<ChildType>& children,
      const std::vector<bool>& claimed,
      float center_x,
      float center_y,
      int target_count,
      std::function<void(const ChildType&, float&, float&)> getCentroid
  );
};

template<typename ChildType, typename ParentType>
std::vector<ParentType> GeographicGrouper<ChildType, ParentType>::groupChildren(
    const std::vector<ChildType>& children,
    std::function<void(const ChildType&, float&, float&)> getCentroid,
    std::function<void(ParentType&, uint32_t, const std::vector<uint32_t>&)> initParent
) {
  std::vector<ParentType> parents;
  std::vector<bool> claimed(children.size(), false);
  
  uint32_t parent_id = 0;
  
  for (size_t seed_idx = 0; seed_idx < children.size(); seed_idx++) {
    if (claimed[seed_idx]) continue;
    
    float seed_x, seed_y;
    getCentroid(children[seed_idx], seed_x, seed_y);
    
    int target_size = config_.min_children_per_parent + 
                     (rand() % (config_.max_children_per_parent - config_.min_children_per_parent + 1));
    
    std::vector<uint32_t> child_ids;
    
    if (config_.use_spatial_proximity) {
      child_ids = findNearestChildren(children, claimed, seed_x, seed_y, target_size, getCentroid);
    } else {
      for (size_t i = seed_idx; i < children.size() && static_cast<int>(child_ids.size()) < target_size; i++) {
        if (!claimed[i]) {
          child_ids.push_back(i);
          claimed[i] = true;
        }
      }
    }
    
    if (child_ids.size() >= static_cast<size_t>(config_.min_children_per_parent)) {
      ParentType parent;
      initParent(parent, parent_id, child_ids);
      parents.push_back(parent);
      parent_id++;
    }
  }
  
  return parents;
}

template<typename ChildType, typename ParentType>
float GeographicGrouper<ChildType, ParentType>::computeDistance(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  return std::sqrt(dx * dx + dy * dy);
}

template<typename ChildType, typename ParentType>
std::vector<uint32_t> GeographicGrouper<ChildType, ParentType>::findNearestChildren(
    const std::vector<ChildType>& children,
    const std::vector<bool>& claimed,
    float center_x,
    float center_y,
    int target_count,
    std::function<void(const ChildType&, float&, float&)> getCentroid
) {
  std::vector<std::pair<float, uint32_t>> distances;
  
  for (size_t i = 0; i < children.size(); i++) {
    if (claimed[i]) continue;
    
    float child_x, child_y;
    getCentroid(children[i], child_x, child_y);
    
    float dist = computeDistance(center_x, center_y, child_x, child_y);
    distances.push_back({dist, i});
  }
  
  std::sort(distances.begin(), distances.end());
  
  std::vector<uint32_t> result;
  for (int i = 0; i < target_count && i < static_cast<int>(distances.size()); i++) {
    result.push_back(distances[i].second);
  }
  
  return result;
}

}  // namespace fenrir

#endif  // FENRIR_SRC_STATE_WORLDSTATE_GEOGRAPHICGROUPER_HH_
