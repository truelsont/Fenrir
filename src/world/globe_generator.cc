#include "globe_generator.hh"
#include <algorithm>

namespace fenrir {

Globe GlobeGenerator::generate(uint32_t subdivisions, uint32_t num_provinces, uint64_t seed) {
    Globe globe;
    generateIcosphere(globe, subdivisions);
    generateProvinces(globe, num_provinces, seed);
    assignVerticesToProvinces(globe);
    return globe;
}

void GlobeGenerator::generateIcosphere(Globe& globe, uint32_t subdivisions) {
    // Golden ratio for icosahedron
    const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
    const float a = 1.0f, b = phi;

    // 12 vertices of icosahedron (normalized to unit sphere)
    globe.vertices = {
        Vec3{-a, b, 0}.normalized(), Vec3{a, b, 0}.normalized(),
        Vec3{-a,-b, 0}.normalized(), Vec3{a,-b, 0}.normalized(),
        Vec3{0,-a, b}.normalized(), Vec3{0, a, b}.normalized(),
        Vec3{0,-a,-b}.normalized(), Vec3{0, a,-b}.normalized(),
        Vec3{b, 0,-a}.normalized(), Vec3{b, 0, a}.normalized(),
        Vec3{-b, 0,-a}.normalized(), Vec3{-b, 0, a}.normalized()
    };

    // 20 triangles of icosahedron
    globe.triangles = {
        {0,11,5}, {0,5,1}, {0,1,7}, {0,7,10}, {0,10,11},
        {1,5,9}, {5,11,4}, {11,10,2}, {10,7,6}, {7,1,8},
        {3,9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9},
        {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1}
    };

    // Subdivide
    std::unordered_map<EdgeKey, uint32_t> midpoint_cache;
    for (uint32_t i = 0; i < subdivisions; ++i) {
        std::vector<std::array<uint32_t, 3>> new_tris;
        new_tris.reserve(globe.triangles.size() * 4);

        for (const auto& tri : globe.triangles) {
            uint32_t a = tri[0], b = tri[1], c = tri[2];
            uint32_t ab = getMidpoint(globe, midpoint_cache, a, b);
            uint32_t bc = getMidpoint(globe, midpoint_cache, b, c);
            uint32_t ca = getMidpoint(globe, midpoint_cache, c, a);

            new_tris.push_back({a, ab, ca});
            new_tris.push_back({b, bc, ab});
            new_tris.push_back({c, ca, bc});
            new_tris.push_back({ab, bc, ca});
        }
        globe.triangles = std::move(new_tris);
        midpoint_cache.clear();
    }
}

uint32_t GlobeGenerator::getMidpoint(
    Globe& globe,
    std::unordered_map<EdgeKey, uint32_t>& cache,
    uint32_t a, uint32_t b
) {
    EdgeKey key = edgeKey(a, b);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;

    Vec3 mid = (globe.vertices[a] + globe.vertices[b]).normalized();
    uint32_t idx = globe.vertices.size();
    globe.vertices.push_back(mid);
    cache[key] = idx;
    return idx;
}

void GlobeGenerator::generateProvinces(Globe& globe, uint32_t num_provinces, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> uni(0.0f, 1.0f);

    globe.province_centers.reserve(num_provinces);

    // Fibonacci sphere for even distribution of seed points
    const float golden_angle = M_PI * (3.0f - std::sqrt(5.0f));

    for (uint32_t i = 0; i < num_provinces; ++i) {
        float y = 1.0f - (i / (float)(num_provinces - 1)) * 2.0f;
        float radius = std::sqrt(1.0f - y * y);
        float theta = golden_angle * i;

        // Add small random perturbation
        float jitter = 0.1f;
        Vec3 center = {
            radius * std::cos(theta) + (uni(rng) - 0.5f) * jitter,
            y + (uni(rng) - 0.5f) * jitter,
            radius * std::sin(theta) + (uni(rng) - 0.5f) * jitter
        };
        globe.province_centers.push_back(center.normalized());
    }
}

void GlobeGenerator::assignVerticesToProvinces(Globe& globe) {
    size_t nv = globe.vertices.size();
    size_t np = globe.province_centers.size();

    globe.vertex_province.resize(nv);
    globe.province_vertices.resize(np);

    // Assign each vertex to nearest province center (spherical Voronoi)
    for (size_t i = 0; i < nv; ++i) {
        const Vec3& v = globe.vertices[i];
        float best_dist = 1e9f;
        uint32_t best_p = 0;

        for (size_t p = 0; p < np; ++p) {
            float d = v.sphereDist(globe.province_centers[p]);
            if (d < best_dist) {
                best_dist = d;
                best_p = p;
            }
        }

        globe.vertex_province[i] = best_p;
        globe.province_vertices[best_p].push_back(i);
    }
}

}  // namespace fenrir
