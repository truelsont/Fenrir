#pragma once

#include <cstdint>
#include <string>

namespace fenrir {

// -------------------------------------------------------------------------
// Terrain types
// -------------------------------------------------------------------------
enum class TerrainType : uint8_t {
    Ocean = 0,
    Plains,
    Forest,
    Hills,
    Mountains,
    Desert,
    Tundra,
    COUNT            // sentinel -- also used as "unassigned" during generation
};

inline const char* terrainName(TerrainType t) {
    switch (t) {
        case TerrainType::Ocean:     return "Ocean";
        case TerrainType::Plains:    return "Plains";
        case TerrainType::Forest:    return "Forest";
        case TerrainType::Hills:     return "Hills";
        case TerrainType::Mountains: return "Mountains";
        case TerrainType::Desert:    return "Desert";
        case TerrainType::Tundra:    return "Tundra";
        default:                     return "Unknown";
    }
}

// Keep old names available so existing code that uses the k-prefixed
// enumerators (e.g. globe_viewer.cc terrain-color switch) still compiles.
// New code should use the short names above.
constexpr TerrainType kOCEAN_TERRAIN     = TerrainType::Ocean;
constexpr TerrainType kPLAINS_TERRAIN    = TerrainType::Plains;
constexpr TerrainType kFOREST_TERRAIN    = TerrainType::Forest;
constexpr TerrainType kHILLS_TERRAIN     = TerrainType::Hills;
constexpr TerrainType kMOUNTAINS_TERRAIN = TerrainType::Mountains;
constexpr TerrainType kDESERT_TERRAIN    = TerrainType::Desert;

// -------------------------------------------------------------------------
// Province simulation data
// -------------------------------------------------------------------------
struct ProvinceData {
    uint32_t id = 0;
    std::string name;
    TerrainType terrain = TerrainType::COUNT;
    float area_km2 = 0.0f;        // Province surface area
    float latitude = 0.0f;        // Average latitude in radians (for climate)

    // Economy
    uint32_t population = 0;
    float tax_income = 0.0f;
    float production_output = 0.0f;
    float trade_value = 0.0f;
    float development = 1.0f;     // 1.0 -- 10.0, like EU4 dev

    // Resources
    float manpower_pool = 0.0f;
    float fertility = 0.0f;       // Agricultural potential [0,1]
    float minerals = 0.0f;        // Industrial potential   [0,1]

    // Military
    float fortification = 0.0f;   // 0 -- 10
    float supply_limit = 0.0f;    // Max troops supported

    // Owner
    uint32_t owner_nation_id = 0;
    uint32_t controller_nation_id = 0;  // Can differ during war
};

// -------------------------------------------------------------------------
// Generate province data from id + seed (deterministic).
//
// `latitude` and `area_km2` may be overwritten by the caller once the globe
// geometry is available; this function produces plausible defaults.
// -------------------------------------------------------------------------
inline ProvinceData generateProvinceData(uint32_t id, uint64_t seed) {
    // Simple deterministic hash-based RNG.
    uint64_t h = (static_cast<uint64_t>(id) * 2654435761ULL) ^ seed;
    auto rnd = [&]() -> float {
        h = h * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<float>(h >> 33) / static_cast<float>(1ULL << 31);
    };

    ProvinceData p;
    p.id = id;
    p.name = "Province " + std::to_string(id);

    // Terrain -- will typically be overwritten by the generator, but provide
    // a fallback that exercises all types.
    p.terrain = static_cast<TerrainType>(
        static_cast<uint8_t>(rnd() * static_cast<float>(TerrainType::COUNT)));

    // Fertility depends on terrain.
    p.fertility = (p.terrain == TerrainType::Plains)
                      ? 0.5f + rnd() * 0.5f
                      : rnd() * 0.3f;
    p.minerals = rnd();

    // Population -- ocean has none.
    p.population = (p.terrain == TerrainType::Ocean)
                       ? 0
                       : static_cast<uint32_t>(10000 + rnd() * 990000 * p.fertility);

    // Economy
    p.tax_income = p.population * (0.5f + rnd()) * 0.001f;
    p.production_output = p.minerals * (0.3f + rnd() * 0.7f) * 100.0f;
    p.trade_value = p.production_output * (0.2f + rnd() * 0.3f);
    p.development = 1.0f + rnd() * 9.0f;

    // Military
    p.manpower_pool = p.population * 0.05f;
    p.supply_limit = 5000.0f + p.fertility * 20000.0f + rnd() * 5000.0f;
    p.fortification = (rnd() < 0.2f) ? rnd() * 5.0f : 0.0f;

    return p;
}

}  // namespace fenrir
