#pragma once

#include <vector>
#include <mutex>
#include <cstring>
#include <unordered_map>
#include "src/util/interfaces/service_interface.hh"
#include "src/util/interfaces/configuration_interface.hh"
#include "src/util/constants.hh"

namespace fenrir {

class WorldManagerConfig : public ConfigurationInterface {
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


    struct Vector2 {
        float x;
        float y;
    };

    WorldManager(){
        width = 320;
        height = 180;
        locations.resize(width * height);
        generateVoronoiProvinces();
    }
    ~WorldManager() = default;

    fenrir::ServiceCodes serializeService() override;
    fenrir::ServiceCodes deserializeService() override;



    enum GeographicLevel {
        LOCATION = 0,
        PROVINCE = 1,
        AREA = 2,
        REGION = 3,
        CONTINENT = 4
    };
    
    struct location {
        uint32_t id;
        uint32_t province_id;
        pixel color;
    };
    using location_t = location;
    
    struct province {
        uint32_t id;
        uint32_t area_id;
        uint32_t owner;
        pixel color;
        std::vector<uint32_t> location_ids;
    };
    using province_t = province;
    
    struct area {
        uint32_t id;
        uint32_t region_id;
        pixel color;
        std::vector<uint32_t> province_ids;
    };
    using area_t = area;
    
    struct region {
        uint32_t id;
        uint32_t continent_id;
        pixel color;
        std::vector<uint32_t> area_ids;
    };
    using region_t = region;
    
    struct continent {
        uint32_t id;
        pixel color;
        std::vector<uint32_t> region_ids;
    };
    using continent_t = continent;

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
    
    location_t* getLocationAt(uint32_t x, uint32_t y);
    province_t* getProvinceById(uint32_t province_id);

    std::vector<location_t> locations;
    std::vector<province_t> provinces;
    std::vector<area_t> areas;
    std::vector<region_t> regions;
    std::vector<continent_t> continents;
    
    std::unordered_map<uint32_t, uint32_t> province_id_to_index;
    std::unordered_map<uint32_t, uint32_t> area_id_to_index;
    std::unordered_map<uint32_t, uint32_t> region_id_to_index;
    std::unordered_map<uint32_t, uint32_t> continent_id_to_index;
    
    uint32_t width;
    uint32_t height;

private:
    void generateVoronoiProvinces();
    void mergeLocationsIntoProvinces();
    void groupProvincesIntoAreas();
    void groupAreasIntoRegions();
    void groupRegionsIntoContinents();
    void computeProvinceCentroids();
    void assignOwnersViaVoronoi();
    void regeneratePixelsBuffer(const std::vector<province_t>& province_snapshot, map_mode_t mode);
    std::vector<pixel> extractViewport(int x, int y, int vp_width, int vp_height);
    pixel getColorForProvince(const province_t& prov, map_mode_t mode);
    
    int findRoot(int x, std::vector<int>& parent);
    void unionSets(int x, int y, std::vector<int>& parent, std::vector<int>& rank);

    std::vector<Vector2> province_centers_;
    std::vector<pixel> pixels_;
    mutable std::mutex provinces_mutex_;
    map_mode_t cached_map_mode_ = kPROVINCE_DEBUG;
    bool pixels_cache_dirty_ = true;
};


}; 