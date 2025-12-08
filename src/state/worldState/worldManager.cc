// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "worldManager.hh"
#include <algorithm>
#include <cmath>

namespace fenrir {

void WorldManager::generateVoronoiProvinces() {
    const uint32_t num_centers = 10;
    
    province_centers_.resize(num_centers);
    province_centers_[0] = {200.0f, 150.0f};
    province_centers_[1] = {640.0f, 100.0f};
    province_centers_[2] = {1000.0f, 200.0f};
    province_centers_[3] = {150.0f, 400.0f};
    province_centers_[4] = {500.0f, 360.0f};
    province_centers_[5] = {900.0f, 450.0f};
    province_centers_[6] = {300.0f, 600.0f};
    province_centers_[7] = {750.0f, 650.0f};
    province_centers_[8] = {1100.0f, 550.0f};
    province_centers_[9] = {640.0f, 360.0f};
    
    for (size_t i = 0; i < provinces.size(); i++) {
        uint32_t x = i % width;
        uint32_t y = i / width;
        
        float min_distance = 999999.0f;
        uint32_t closest_center = 0;
        
        for (uint32_t c = 0; c < num_centers; c++) {
            float dx = static_cast<float>(x) - province_centers_[c].x;
            float dy = static_cast<float>(y) - province_centers_[c].y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance < min_distance) {
                min_distance = distance;
                closest_center = c;
            }
        }
        
        provinces[i].id = i;
        provinces[i].owner = closest_center;
        provinces[i].color = owner_to_color_map[closest_center % owner_to_color_map.size()];
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

}  // namespace fenrir
