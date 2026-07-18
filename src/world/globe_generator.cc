#include "globe_generator.hh"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace fenrir {

// =========================================================================
// Public API
// =========================================================================

Globe GlobeGenerator::generate(uint32_t subdivisions, uint32_t num_provinces,
                               uint64_t seed) {
    Globe globe;
    generateIcosphere(globe, subdivisions);
    generateProvinces(globe, num_provinces, seed);
    assignVerticesToProvinces(globe);
    computeProvinceAdjacency(globe);
    computeProvinceAreas(globe);
    return globe;
}

std::vector<ProvinceData> GlobeGenerator::generate_province_data(
        const Globe& globe, uint64_t seed) {
    const uint32_t np = static_cast<uint32_t>(globe.province_centers.size());
    std::vector<ProvinceData> data(np);

    std::mt19937_64 rng(seed ^ 0xBEEF'CAFE'1234ULL);
    std::uniform_real_distribution<float> uni(0.0f, 1.0f);

    for (uint32_t i = 0; i < np; ++i) {
        // Start with the hash-based generator for economy values.
        data[i] = generateProvinceData(i, seed);

        // Compute average latitude from the province center.
        const Vec3& c = globe.province_centers[i];
        GeoCoord geo = Globe::cartToGeo(c);
        data[i].latitude = geo.lat;

        // Overwrite area with the computed geometric area.
        if (i < globe.province_areas_km2.size()) {
            data[i].area_km2 = globe.province_areas_km2[i];
        }

        // Assign terrain based on latitude + noise.
        float noise = uni(rng);
        float elev_noise = uni(rng);
        data[i].terrain = assignTerrain(geo.lat, noise, elev_noise);

        // Adjust economic fields for terrain.
        if (data[i].terrain == TerrainType::Ocean) {
            data[i].population = 0;
            data[i].fertility = 0.0f;
            data[i].manpower_pool = 0.0f;
            data[i].tax_income = 0.0f;
            data[i].fortification = 0.0f;
        } else if (data[i].terrain == TerrainType::Tundra) {
            data[i].fertility *= 0.2f;
            data[i].population = static_cast<uint32_t>(data[i].population * 0.1f);
        } else if (data[i].terrain == TerrainType::Desert) {
            data[i].fertility *= 0.15f;
            data[i].population = static_cast<uint32_t>(data[i].population * 0.2f);
        } else if (data[i].terrain == TerrainType::Mountains) {
            data[i].fertility *= 0.3f;
            data[i].minerals = std::min(1.0f, data[i].minerals + 0.3f);
        }

        // Recompute dependent fields.
        data[i].manpower_pool = data[i].population * 0.05f;
        data[i].supply_limit = 5000.0f + data[i].fertility * 20000.0f
                             + uni(rng) * 5000.0f;
    }

    return data;
}

// =========================================================================
// Icosphere generation
// =========================================================================

void GlobeGenerator::generateIcosphere(Globe& globe, uint32_t subdivisions) {
    // Golden ratio for icosahedron.
    const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
    const float a = 1.0f, b = phi;

    // 12 vertices of the icosahedron (normalised to unit sphere).
    globe.vertices = {
        Vec3{-a, b, 0}.normalized(), Vec3{ a, b, 0}.normalized(),
        Vec3{-a,-b, 0}.normalized(), Vec3{ a,-b, 0}.normalized(),
        Vec3{ 0,-a, b}.normalized(), Vec3{ 0, a, b}.normalized(),
        Vec3{ 0,-a,-b}.normalized(), Vec3{ 0, a,-b}.normalized(),
        Vec3{ b, 0,-a}.normalized(), Vec3{ b, 0, a}.normalized(),
        Vec3{-b, 0,-a}.normalized(), Vec3{-b, 0, a}.normalized()
    };

    // 20 triangles of the icosahedron.
    globe.triangles = {
        {0,11,5}, {0,5,1}, {0,1,7}, {0,7,10}, {0,10,11},
        {1,5,9}, {5,11,4}, {11,10,2}, {10,7,6}, {7,1,8},
        {3,9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9},
        {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1}
    };

    // Subdivide.
    std::unordered_map<EdgeKey, uint32_t> midpoint_cache;
    for (uint32_t i = 0; i < subdivisions; ++i) {
        std::vector<std::array<uint32_t, 3>> new_tris;
        new_tris.reserve(globe.triangles.size() * 4);

        for (const auto& tri : globe.triangles) {
            uint32_t va = tri[0], vb = tri[1], vc = tri[2];
            uint32_t ab = getMidpoint(globe, midpoint_cache, va, vb);
            uint32_t bc = getMidpoint(globe, midpoint_cache, vb, vc);
            uint32_t ca = getMidpoint(globe, midpoint_cache, vc, va);

            new_tris.push_back({va, ab, ca});
            new_tris.push_back({vb, bc, ab});
            new_tris.push_back({vc, ca, bc});
            new_tris.push_back({ab, bc, ca});
        }
        globe.triangles = std::move(new_tris);
        midpoint_cache.clear();
    }
}

uint32_t GlobeGenerator::getMidpoint(
        Globe& globe,
        std::unordered_map<EdgeKey, uint32_t>& cache,
        uint32_t a, uint32_t b) {
    EdgeKey key = edgeKey(a, b);
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;

    Vec3 mid = (globe.vertices[a] + globe.vertices[b]).normalized();
    uint32_t idx = static_cast<uint32_t>(globe.vertices.size());
    globe.vertices.push_back(mid);
    cache[key] = idx;
    return idx;
}

// =========================================================================
// Province generation (Fibonacci sphere + nearest-center Voronoi)
// =========================================================================

void GlobeGenerator::generateProvinces(Globe& globe, uint32_t num_provinces,
                                       uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> uni(0.0f, 1.0f);

    globe.province_centers.reserve(num_provinces);

    // Fibonacci sphere for even distribution of seed points.
    const float golden_angle =
        static_cast<float>(M_PI) * (3.0f - std::sqrt(5.0f));

    for (uint32_t i = 0; i < num_provinces; ++i) {
        float y = 1.0f - (i / static_cast<float>(num_provinces - 1)) * 2.0f;
        float r = std::sqrt(1.0f - y * y);
        float theta = golden_angle * static_cast<float>(i);

        // Small random perturbation.
        constexpr float kJitter = 0.1f;
        Vec3 center = {
            r * std::cos(theta) + (uni(rng) - 0.5f) * kJitter,
            y                   + (uni(rng) - 0.5f) * kJitter,
            r * std::sin(theta) + (uni(rng) - 0.5f) * kJitter
        };
        globe.province_centers.push_back(center.normalized());
    }
}

void GlobeGenerator::assignVerticesToProvinces(Globe& globe) {
    const size_t nv = globe.vertices.size();
    const size_t np = globe.province_centers.size();

    globe.vertex_province.resize(nv);
    globe.province_vertices.resize(np);

    // Assign each vertex to its nearest province center (spherical Voronoi).
    for (size_t i = 0; i < nv; ++i) {
        const Vec3& v = globe.vertices[i];
        float best_dist = 1e9f;
        uint32_t best_p = 0;

        for (size_t p = 0; p < np; ++p) {
            float d = v.sphereDist(globe.province_centers[p]);
            if (d < best_dist) {
                best_dist = d;
                best_p = static_cast<uint32_t>(p);
            }
        }

        globe.vertex_province[i] = best_p;
        globe.province_vertices[best_p].push_back(static_cast<uint32_t>(i));
    }
}

// =========================================================================
// Province adjacency from shared triangle edges
// =========================================================================

void GlobeGenerator::computeProvinceAdjacency(Globe& globe) {
    const uint32_t np = static_cast<uint32_t>(globe.province_centers.size());
    globe.province_adjacency.resize(np);

    // For each province, collect a set of neighbors.
    std::vector<std::unordered_set<uint32_t>> adj_sets(np);

    for (const auto& tri : globe.triangles) {
        // Province ids of the three vertices of this triangle.
        uint32_t pa = globe.vertex_province[tri[0]];
        uint32_t pb = globe.vertex_province[tri[1]];
        uint32_t pc = globe.vertex_province[tri[2]];

        // If any two vertices belong to different provinces, those provinces
        // are adjacent (they share at least this triangle edge).
        if (pa != pb) { adj_sets[pa].insert(pb); adj_sets[pb].insert(pa); }
        if (pb != pc) { adj_sets[pb].insert(pc); adj_sets[pc].insert(pb); }
        if (pa != pc) { adj_sets[pa].insert(pc); adj_sets[pc].insert(pa); }
    }

    for (uint32_t i = 0; i < np; ++i) {
        globe.province_adjacency[i].assign(adj_sets[i].begin(),
                                           adj_sets[i].end());
        std::sort(globe.province_adjacency[i].begin(),
                  globe.province_adjacency[i].end());
    }
}

// =========================================================================
// Province area computation (sum of triangle areas on the unit sphere)
// =========================================================================

void GlobeGenerator::computeProvinceAreas(Globe& globe) {
    const uint32_t np = static_cast<uint32_t>(globe.province_centers.size());
    globe.province_areas_km2.assign(np, 0.0f);

    const float r2 = globe.radius * globe.radius;

    for (const auto& tri : globe.triangles) {
        const Vec3& v0 = globe.vertices[tri[0]];
        const Vec3& v1 = globe.vertices[tri[1]];
        const Vec3& v2 = globe.vertices[tri[2]];

        // Flat triangle area on the unit sphere, scaled to km^2.
        // area = 0.5 * |e1 x e2| * R^2
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        float area = 0.5f * e1.cross(e2).length() * r2;

        // Attribute entire triangle area to the majority province.
        // (For a Voronoi tessellation most triangles are single-province.)
        uint32_t pa = globe.vertex_province[tri[0]];
        uint32_t pb = globe.vertex_province[tri[1]];
        uint32_t pc = globe.vertex_province[tri[2]];

        // Simple majority vote.
        uint32_t owner;
        if (pa == pb || pa == pc) {
            owner = pa;
        } else {
            owner = pb;  // pb == pc or all three differ -- pick pb
        }

        globe.province_areas_km2[owner] += area;
    }
}

// =========================================================================
// Terrain assignment based on latitude + noise
// =========================================================================

TerrainType GlobeGenerator::assignTerrain(float latitude_rad, float noise,
                                          float elevation_noise) {
    const float abs_lat = std::fabs(latitude_rad);

    // Polar regions: tundra / ocean.
    // abs_lat ~1.22 rad = ~70 deg
    if (abs_lat > 1.22f) {
        return (noise < 0.3f) ? TerrainType::Ocean : TerrainType::Tundra;
    }

    // Sub-polar (55-70 deg  ~ 0.96-1.22 rad): forest / tundra.
    if (abs_lat > 0.96f) {
        if (noise < 0.15f) return TerrainType::Tundra;
        if (noise < 0.5f)  return TerrainType::Forest;
        if (elevation_noise > 0.7f) return TerrainType::Mountains;
        return TerrainType::Hills;
    }

    // Temperate (30-55 deg  ~ 0.52-0.96 rad): varied.
    if (abs_lat > 0.52f) {
        if (noise < 0.1f)  return TerrainType::Ocean;   // inland sea
        if (noise < 0.35f) return TerrainType::Plains;
        if (noise < 0.6f)  return TerrainType::Forest;
        if (elevation_noise > 0.6f) return TerrainType::Mountains;
        return TerrainType::Hills;
    }

    // Subtropical / tropical (10-30 deg  ~ 0.17-0.52 rad): desert / plains.
    if (abs_lat > 0.17f) {
        if (noise < 0.1f)  return TerrainType::Ocean;
        if (noise < 0.4f)  return TerrainType::Desert;
        if (noise < 0.65f) return TerrainType::Plains;
        if (elevation_noise > 0.7f) return TerrainType::Mountains;
        return TerrainType::Forest;
    }

    // Equatorial (0-10 deg): forest / plains.
    if (noise < 0.08f) return TerrainType::Ocean;
    if (noise < 0.5f)  return TerrainType::Forest;
    if (elevation_noise > 0.75f) return TerrainType::Mountains;
    return TerrainType::Plains;
}

}  // namespace fenrir
