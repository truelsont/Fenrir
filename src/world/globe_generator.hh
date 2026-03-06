#pragma once

#include "globe.hh"
#include <random>
#include <unordered_map>

namespace fenrir {

class GlobeGenerator {
public:
    // Generate globe with given subdivision level and province count
    // subdivisions: 0=20 tris, 1=80, 2=320, 3=1280, 4=5120, 5=20480...
    static Globe generate(uint32_t subdivisions, uint32_t num_provinces, uint64_t seed = 42);

private:
    static void generateIcosphere(Globe& globe, uint32_t subdivisions);
    static void generateProvinces(Globe& globe, uint32_t num_provinces, uint64_t seed);
    static void assignVerticesToProvinces(Globe& globe);

    // Icosphere helpers
    using EdgeKey = uint64_t;
    static EdgeKey edgeKey(uint32_t a, uint32_t b) {
        return a < b ? ((uint64_t)a << 32) | b : ((uint64_t)b << 32) | a;
    }

    static uint32_t getMidpoint(
        Globe& globe,
        std::unordered_map<EdgeKey, uint32_t>& cache,
        uint32_t a, uint32_t b
    );
};

}  // namespace fenrir
