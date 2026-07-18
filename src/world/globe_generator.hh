#pragma once

#include "globe.hh"
#include "province_data.hh"
#include <random>
#include <unordered_map>
#include <vector>

namespace fenrir {

class GlobeGenerator {
public:
    /// Generate a complete globe with mesh, provinces, adjacency, terrain,
    /// and province areas.
    ///
    /// @param subdivisions  Icosphere subdivision depth.
    ///   0 -> 20 tris, 1 -> 80, 2 -> 320, 3 -> 1280, 4 -> 5120, 5 -> 20480 ...
    /// @param num_provinces Number of Voronoi provinces to generate.
    /// @param seed          RNG seed for reproducibility.
    static Globe generate(uint32_t subdivisions, uint32_t num_provinces,
                          uint64_t seed = 42);

    /// Generate ProvinceData for every province on the globe, incorporating
    /// latitude-based terrain and the computed area.
    static std::vector<ProvinceData> generate_province_data(
        const Globe& globe, uint64_t seed);

private:
    // ---- Icosphere ----
    static void generateIcosphere(Globe& globe, uint32_t subdivisions);

    using EdgeKey = uint64_t;
    static EdgeKey edgeKey(uint32_t a, uint32_t b) {
        return a < b ? (static_cast<uint64_t>(a) << 32) | b
                     : (static_cast<uint64_t>(b) << 32) | a;
    }
    static uint32_t getMidpoint(Globe& globe,
                                std::unordered_map<EdgeKey, uint32_t>& cache,
                                uint32_t a, uint32_t b);

    // ---- Provinces ----
    static void generateProvinces(Globe& globe, uint32_t num_provinces,
                                  uint64_t seed);
    static void assignVerticesToProvinces(Globe& globe);

    // ---- New: adjacency, areas, terrain ----
    static void computeProvinceAdjacency(Globe& globe);
    static void computeProvinceAreas(Globe& globe);
    static TerrainType assignTerrain(float latitude_rad, float noise,
                                     float elevation_noise);
};

}  // namespace fenrir
