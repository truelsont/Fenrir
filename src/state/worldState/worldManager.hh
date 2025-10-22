#pragma once

#include <vector>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include "abstractService.hh"
#include "abstractConfiguration.hh"
#include "constants.hh"

namespace fenrir {

class WorldManagerConfig : public AbstractConfiguration {
public:
    WorldManagerConfig() = default;

};

class WorldManager : public AbstractService {
public:
    WorldManager(){
        width = 1280;
        height = 720;
        pixels.resize(width * height, {255,0,0,255});
    }
    ~WorldManager() = default;

    fenrir::ServiceCodes serializeService() override;
    fenrir::ServiceCodes deserializeService() override;

    struct pixel{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    
    // Create a Godot ImageTexture from the pixels buffer
    godot::Ref<godot::ImageTexture> createImageTexture();
    
    std::vector<pixel> pixels;
    uint32_t width;
    uint32_t height;
};


}; 