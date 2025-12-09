// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "world_manager.hh"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <cstdlib>
#include <queue>

namespace fenrir {

void WorldManager::generateVoronoiProvinces() {
    for (size_t i = 0; i < locations.size(); i++) {
        locations[i].id = i;
        locations[i].province_id = 0;
        locations[i].color = {128, 128, 128, 255};
    }
    
    mergeLocationsIntoProvinces();
    computeProvinceCentroids();
    groupProvincesIntoAreas();
    groupAreasIntoRegions();
    groupRegionsIntoContinents();
    assignOwnersViaVoronoi();
}

int WorldManager::findRoot(int x, std::vector<int>& parent) {
    if (parent[x] != x) {
        parent[x] = findRoot(parent[x], parent);
    }
    return parent[x];
}

void WorldManager::unionSets(int x, int y, std::vector<int>& parent, std::vector<int>& rank) {
    int rootX = findRoot(x, parent);
    int rootY = findRoot(y, parent);
    
    if (rootX != rootY) {
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

void WorldManager::mergeLocationsIntoProvinces() {
    std::vector<bool> claimed(locations.size(), false);
    std::vector<int> province_assignment(locations.size(), -1);
    
    uint32_t province_id = 0;
    const int min_size = 10;
    const int max_size = 30;
    
    for (uint32_t seed_y = 0; seed_y < height; seed_y++) {
        for (uint32_t seed_x = 0; seed_x < width; seed_x++) {
            uint32_t seed_idx = seed_y * width + seed_x;
            
            if (claimed[seed_idx]) continue;
            
            int target_size = min_size + (rand() % (max_size - min_size + 1));
            
            std::vector<uint32_t> province_locations;
            std::queue<uint32_t> frontier;
            
            frontier.push(seed_idx);
            claimed[seed_idx] = true;
            
            while (!frontier.empty() && static_cast<int>(province_locations.size()) < target_size) {
                uint32_t current = frontier.front();
                frontier.pop();
                
                province_locations.push_back(current);
                province_assignment[current] = province_id;
                
                uint32_t cx = current % width;
                uint32_t cy = current / width;
                
                int dx[] = {1, 0, -1, 0, 1, 1, -1, -1};
                int dy[] = {0, 1, 0, -1, 1, -1, 1, -1};
                
                for (int dir = 0; dir < 8; dir++) {
                    int nx = cx + dx[dir];
                    int ny = cy + dy[dir];
                    
                    if (nx >= 0 && nx < static_cast<int>(width) && 
                        ny >= 0 && ny < static_cast<int>(height)) {
                        uint32_t neighbor_idx = ny * width + nx;
                        
                        if (!claimed[neighbor_idx]) {
                            claimed[neighbor_idx] = true;
                            frontier.push(neighbor_idx);
                        }
                    }
                }
            }
            
            if (province_locations.size() >= static_cast<size_t>(min_size)) {
                province_t prov;
                prov.id = province_id;
                prov.area_id = province_id / 10;
                prov.owner = 0;
                prov.color = {
                    static_cast<uint8_t>((province_id * 73) % 256),
                    static_cast<uint8_t>((province_id * 151) % 256),
                    static_cast<uint8_t>((province_id * 223) % 256),
                    255
                };
                prov.location_ids = province_locations;
                
                for (uint32_t loc_id : province_locations) {
                    locations[loc_id].province_id = province_id;
                    locations[loc_id].color = prov.color;
                }
                
                province_id_to_index[province_id] = provinces.size();
                provinces.push_back(prov);
                province_id++;
            }
        }
    }
}

void WorldManager::computeProvinceCentroids() {
    province_centers_.clear();
    province_centers_.resize(provinces.size());
    
    for (size_t i = 0; i < provinces.size(); i++) {
        float sum_x = 0.0f;
        float sum_y = 0.0f;
        
        for (uint32_t loc_id : provinces[i].location_ids) {
            uint32_t x = loc_id % width;
            uint32_t y = loc_id / width;
            sum_x += x;
            sum_y += y;
        }
        
        province_centers_[i].x = sum_x / provinces[i].location_ids.size();
        province_centers_[i].y = sum_y / provinces[i].location_ids.size();
    }
}

void WorldManager::assignOwnersViaVoronoi() {
    const uint32_t num_owners = 10;
    
    std::vector<Vector2> owner_centers(num_owners);
    owner_centers[0] = {50.0f, 40.0f};
    owner_centers[1] = {160.0f, 25.0f};
    owner_centers[2] = {250.0f, 50.0f};
    owner_centers[3] = {40.0f, 100.0f};
    owner_centers[4] = {125.0f, 90.0f};
    owner_centers[5] = {225.0f, 110.0f};
    owner_centers[6] = {75.0f, 150.0f};
    owner_centers[7] = {190.0f, 160.0f};
    owner_centers[8] = {275.0f, 140.0f};
    owner_centers[9] = {160.0f, 90.0f};
    
    for (size_t i = 0; i < provinces.size(); i++) {
        float min_distance = 999999.0f;
        uint32_t closest_owner = 0;
        
        for (uint32_t o = 0; o < num_owners; o++) {
            float dx = province_centers_[i].x - owner_centers[o].x;
            float dy = province_centers_[i].y - owner_centers[o].y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < min_distance) {
                min_distance = distance;
                closest_owner = o;
            }
        }
        
        provinces[i].owner = closest_owner;
    }
}

void WorldManager::groupProvincesIntoAreas() {
    std::vector<bool> claimed(provinces.size(), false);
    uint32_t area_id = 0;
    const int min_provinces = 5;
    const int max_provinces = 10;
    
    for (size_t seed_idx = 0; seed_idx < provinces.size(); seed_idx++) {
        if (claimed[seed_idx]) continue;
        
        int target_size = min_provinces + (rand() % (max_provinces - min_provinces + 1));
        std::vector<uint32_t> province_ids;
        
        for (size_t i = seed_idx; i < provinces.size() && static_cast<int>(province_ids.size()) < target_size; i++) {
            if (!claimed[i]) {
                province_ids.push_back(provinces[i].id);
                claimed[i] = true;
                provinces[i].area_id = area_id;
            }
        }
        
        if (province_ids.size() >= static_cast<size_t>(min_provinces)) {
            area_t area;
            area.id = area_id;
            area.region_id = 0;
            area.color = {
                static_cast<uint8_t>((area_id * 97) % 256),
                static_cast<uint8_t>((area_id * 179) % 256),
                static_cast<uint8_t>((area_id * 211) % 256),
                255
            };
            area.province_ids = province_ids;
            
            area_id_to_index[area_id] = areas.size();
            areas.push_back(area);
            area_id++;
        }
    }
}

void WorldManager::groupAreasIntoRegions() {
    std::vector<bool> claimed(areas.size(), false);
    uint32_t region_id = 0;
    const int min_areas = 3;
    const int max_areas = 8;
    
    for (size_t seed_idx = 0; seed_idx < areas.size(); seed_idx++) {
        if (claimed[seed_idx]) continue;
        
        int target_size = min_areas + (rand() % (max_areas - min_areas + 1));
        std::vector<uint32_t> area_ids;
        
        for (size_t i = seed_idx; i < areas.size() && static_cast<int>(area_ids.size()) < target_size; i++) {
            if (!claimed[i]) {
                area_ids.push_back(areas[i].id);
                claimed[i] = true;
                areas[i].region_id = region_id;
            }
        }
        
        if (area_ids.size() >= static_cast<size_t>(min_areas)) {
            region_t region;
            region.id = region_id;
            region.continent_id = 0;
            region.color = {
                static_cast<uint8_t>((region_id * 113) % 256),
                static_cast<uint8_t>((region_id * 193) % 256),
                static_cast<uint8_t>((region_id * 227) % 256),
                255
            };
            region.area_ids = area_ids;
            
            region_id_to_index[region_id] = regions.size();
            regions.push_back(region);
            region_id++;
        }
    }
}

void WorldManager::groupRegionsIntoContinents() {
    std::vector<bool> claimed(regions.size(), false);
    uint32_t continent_id = 0;
    const int min_regions = 2;
    const int max_regions = 5;
    
    for (size_t seed_idx = 0; seed_idx < regions.size(); seed_idx++) {
        if (claimed[seed_idx]) continue;
        
        int target_size = min_regions + (rand() % (max_regions - min_regions + 1));
        std::vector<uint32_t> region_ids;
        
        for (size_t i = seed_idx; i < regions.size() && static_cast<int>(region_ids.size()) < target_size; i++) {
            if (!claimed[i]) {
                region_ids.push_back(regions[i].id);
                claimed[i] = true;
                regions[i].continent_id = continent_id;
            }
        }
        
        if (region_ids.size() >= static_cast<size_t>(min_regions)) {
            continent_t continent;
            continent.id = continent_id;
            continent.color = {
                static_cast<uint8_t>((continent_id * 127) % 256),
                static_cast<uint8_t>((continent_id * 199) % 256),
                static_cast<uint8_t>((continent_id * 233) % 256),
                255
            };
            continent.region_ids = region_ids;
            
            continent_id_to_index[continent_id] = continents.size();
            continents.push_back(continent);
            continent_id++;
        }
    }
}

std::vector<WorldManager::pixel> WorldManager::createImageData(rendering_options_t rendering_options) {
    if (rendering_options.map_mode != cached_map_mode_ || pixels_cache_dirty_) {
        std::vector<province_t> province_snapshot;
        {
            std::lock_guard<std::mutex> lock(provinces_mutex_);
            province_snapshot = provinces;
        }
        
        regeneratePixelsBuffer(province_snapshot, rendering_options.map_mode);
        cached_map_mode_ = rendering_options.map_mode;
        pixels_cache_dirty_ = false;
    }
    
    return extractViewport(
        rendering_options.view_port_x,
        rendering_options.view_port_y,
        rendering_options.view_port_width,
        rendering_options.view_port_height
    );
}

void WorldManager::markCacheDirty() {
    pixels_cache_dirty_ = true;
}

void WorldManager::regeneratePixelsBuffer(const std::vector<province_t>& province_snapshot, map_mode_t mode) {
    pixels_.resize(width * height);
    
    for (size_t i = 0; i < province_snapshot.size() && i < pixels_.size(); i++) {
        pixels_[i] = getColorForProvince(province_snapshot[i], mode);
    }
}

std::vector<WorldManager::pixel> WorldManager::extractViewport(int x, int y, int vp_width, int vp_height) {
    int clamped_x = std::max(0, std::min(x, static_cast<int>(width)));
    int clamped_y = std::max(0, std::min(y, static_cast<int>(height)));
    int clamped_width = std::min(vp_width, static_cast<int>(width) - clamped_x);
    int clamped_height = std::min(vp_height, static_cast<int>(height) - clamped_y);
    
    std::vector<pixel> viewport_pixels;
    viewport_pixels.resize(clamped_width * clamped_height);
    
    for (int row = 0; row < clamped_height; row++) {
        int src_offset = ((clamped_y + row) * width + clamped_x);
        int dst_offset = row * clamped_width;
        
        for (int col = 0; col < clamped_width; col++) {
            viewport_pixels[dst_offset + col] = pixels_[src_offset + col];
        }
    }
    
    return viewport_pixels;
}

WorldManager::pixel WorldManager::getColorForProvince(const province_t& prov, map_mode_t mode) {
    switch (mode) {
        case kPROVINCE_DEBUG:
            return prov.color;
        case kOWNER: {
            uint8_t owner_hash = static_cast<uint8_t>((prov.owner * 73) % 256);
            return pixel{
                static_cast<uint8_t>((owner_hash * 17) % 256),
                static_cast<uint8_t>((owner_hash * 31) % 256),
                static_cast<uint8_t>((owner_hash * 53) % 256),
                255
            };
        }
    }
    return pixel{0, 0, 0, 255};
}

fenrir::ServiceCodes WorldManager::serializeService() {
    std::lock_guard<std::mutex> lock(provinces_mutex_);
    return fenrir::ServiceCodes::kSERVICE_SERIALIZE_SUCCESS;
}

fenrir::ServiceCodes WorldManager::deserializeService() {
    std::lock_guard<std::mutex> lock(provinces_mutex_);
    markCacheDirty();
    return fenrir::ServiceCodes::kSERVICE_SERIALIZE_SUCCESS;
}

WorldManager::location_t* WorldManager::getLocationAt(uint32_t x, uint32_t y) {
    if (x >= width || y >= height) return nullptr;
    uint32_t idx = y * width + x;
    if (idx >= locations.size()) return nullptr;
    return &locations[idx];
}

WorldManager::province_t* WorldManager::getProvinceById(uint32_t province_id) {
    auto it = province_id_to_index.find(province_id);
    if (it != province_id_to_index.end()) {
        return &provinces[it->second];
    }
    return nullptr;
}

}  // namespace fenrir
