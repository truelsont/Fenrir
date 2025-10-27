// Copyright 2025 Fenrir Project
// Google C++ Style Guide

#include "worldManager.hh"
#include <algorithm>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace fenrir {

godot::Ref<godot::ImageTexture> WorldManager::createImageTexture(rendering_options_t rendering_options) {
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

godot::Ref<godot::ImageTexture> WorldManager::extractViewport(int x, int y, int vp_width, int vp_height) {
    int clamped_x = std::max(0, std::min(x, static_cast<int>(width)));
    int clamped_y = std::max(0, std::min(y, static_cast<int>(height)));
    int clamped_width = std::min(vp_width, static_cast<int>(width) - clamped_x);
    int clamped_height = std::min(vp_height, static_cast<int>(height) - clamped_y);
    
    godot::PackedByteArray byte_array;
    byte_array.resize(clamped_width * clamped_height * 4);
    
    for (int row = 0; row < clamped_height; row++) {
        int src_offset = ((clamped_y + row) * width + clamped_x);
        int dst_offset = row * clamped_width;
        
        for (int col = 0; col < clamped_width; col++) {
            const pixel& p = pixels_[src_offset + col];
            int byte_idx = (dst_offset + col) * 4;
            byte_array[byte_idx + 0] = p.r;
            byte_array[byte_idx + 1] = p.g;
            byte_array[byte_idx + 2] = p.b;
            byte_array[byte_idx + 3] = p.a;
        }
    }
    
    godot::Ref<godot::Image> image = godot::Image::create_from_data(
        clamped_width,
        clamped_height,
        false,
        godot::Image::FORMAT_RGBA8,
        byte_array
    );
    
    return godot::ImageTexture::create_from_image(image);
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
