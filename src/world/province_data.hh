#pragma once

#include <flecs.h>
#include <string>
#include <cstdint>

namespace fenrir {

// define UUID
typedef uint64_t ProvinceUUID; 
typedef uint64_t ResourceUUID; 

// maybe make this a veritual class for more attrs 
enum class TerrainType : uint{
    kOCEAN_TERRAIN = 0, 
    kSHORE_TERRAIN = 1, 
    kPLAINS_TERRAIN = 2, 
    kFOREST_TERRAIN = 3, 
    kHILLS_TERRAIN = 4, 
    kMOUNTAINS_TERRAIN = 5, 
    kDESERT_TERRAIN = 6, 
    kNUMBER_OF_TERRAIN_TYPES //Sent value also used for generation as null value 
}; 

enum class ResourceType : uint{
    kFOO = 0, 
    kNUMBER_OF_RESOURCE_TYPES
}; 

template <typename ResourceType>
class ResourcesTypeData{
public: 
    explicit ResourcesTypeData(
        cont ProvinceUUID provinceUUID, 
        const uint32_t init_resource_quantity):
        owning_province_id{provinceUUID}
        quantity{init_resource_quantity}
        {}

    const ProvinceUUID owning_province_id; 
    uint32_t quantity; 
}; 

// Province simulation data
struct ProvinceData {
    ProvinceUUID id;
    std::string name;
    uint32_t population;
    float wealth;
    float fertility;      // Agricultural potential
    float resources;      // Mineral resources
    TerrainType terrain;      // 0=ocean, 1=plains, 2=hills, 3=mountains, 4=desert
};

// Generate random province data
inline ProvinceData generateProvinceData(uint32_t id, uint64_t seed) {
    // Simple deterministic RNG based on id and seed
    uint64_t h = (id * 2654435761ULL) ^ seed;
    auto rnd = [&]() -> float {
        h = h * 6364136223846793005ULL + 1442695040888963407ULL;
        return (h >> 33) / (float)(1ULL << 31);
    };

    ProvinceData p;
    p.id = id;
    p.name = "Province " + std::to_string(id);
    p.terrain{TerrainType::kNUMBER_OF_TERRAIN_TYPES}; 
    p.fertility = (p.terrain == 1) ? 0.5f + rnd() * 0.5f : rnd() * 0.3f;
    p.resources = rnd();
    p.population = (p.terrain == 0) ? 0 : (uint32_t)(10000 + rnd() * 990000 * p.fertility);
    p.wealth = p.population * (0.5f + rnd() * 1.5f) * 0.001f;
    return p;
}


}  // namespace fenrir
