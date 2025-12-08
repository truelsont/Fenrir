#pragma once

#include <vector>
#include <mutex>
#include <cstring>
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

    struct pixel{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    
    static inline const std::vector<pixel> owner_to_color_map = {
        {0, 0, 255, 255},
        {0, 255, 0, 255},
        {255, 0, 0, 255},
        {0, 255, 255, 255},
        {255, 255, 0, 255},
        {255, 0, 255, 255},
        {255, 255, 255, 255},
        {128, 128, 128, 255},
        {128, 0, 0, 255},
        {0, 128, 0, 255},
        {0, 0, 128, 255},
        {128, 128, 0, 255},
        {128, 0, 128, 255},
        {0, 128, 128, 255},
        {192, 192, 192, 255},
        {64, 64, 64, 255},
        {255, 128, 0, 255},
        {128, 255, 0, 255},
        {0, 255, 128, 255},
        {128, 0, 255, 255},
    };


    WorldManager(){
        width = 1280;
        height = 720;
        provinces.resize(width * height);
        for (size_t i = 0; i < provinces.size(); i++) {
            provinces[i].id = i;
            uint32_t owner = ((i / 20) * 5667) % 20;
            provinces[i].owner = owner;
            provinces[i].color = owner_to_color_map[owner];
        }
    }
    ~WorldManager() = default;

    fenrir::ServiceCodes serializeService() override;
    fenrir::ServiceCodes deserializeService() override;



    struct province {
        uint32_t id;
        uint32_t owner;
        pixel color;
    };
    using province_t = province;

    enum map_mode {
        kPROVINCE_DEBUG, 
        kOWNER
    };
    using map_mode_t = map_mode; 

    typedef struct rendering_options {
        map_mode_t map_mode;
        int view_port_x; 
        int view_port_y;
        int view_port_width;
        int view_port_height;
    } rendering_options_t;
    
    
    std::vector<pixel> createImageData(rendering_options_t rendering_options);

    void markCacheDirty();

    std::vector<province_t> provinces;
    uint32_t width;
    uint32_t height;

private:
    void regeneratePixelsBuffer(const std::vector<province_t>& province_snapshot, map_mode_t mode);
    std::vector<pixel> extractViewport(int x, int y, int vp_width, int vp_height);
    pixel getColorForProvince(const province_t& prov, map_mode_t mode);

    std::vector<pixel> pixels_;
    mutable std::mutex provinces_mutex_;
    map_mode_t cached_map_mode_ = kPROVINCE_DEBUG;
    bool pixels_cache_dirty_ = true;
};


}; 