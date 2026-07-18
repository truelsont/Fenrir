#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace fenrir {

// -------------------------------------------------------------------------
// Vec3 -- lightweight 3D vector for mesh storage.
//
// Kept free of Eigen so that Globe can be included without pulling in
// the entire Eigen library.  Use to_eigen() / from_eigen() to convert.
// -------------------------------------------------------------------------
struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s)       const { return {x * s,   y * s,   z * s};   }

    float dot(const Vec3& o)   const { return x * o.x + y * o.y + z * o.z; }
    float length()             const { return std::sqrt(dot(*this)); }
    Vec3  normalized()         const {
        float l = length();
        return l > 0.0f ? *this * (1.0f / l) : *this;
    }

    Vec3 cross(const Vec3& o) const {
        return {y * o.z - z * o.y,
                z * o.x - x * o.z,
                x * o.y - y * o.x};
    }

    /// Great-circle distance on the unit sphere (radians).
    /// Uses atan2 for numerical stability.
    float sphere_dist(const Vec3& o) const {
        const Vec3 an = normalized();
        const Vec3 bn = o.normalized();
        return std::atan2(an.cross(bn).length(), an.dot(bn));
    }

    // Keep the old camelCase name for backward compatibility.
    float sphereDist(const Vec3& o) const { return sphere_dist(o); }
};

// -------------------------------------------------------------------------
// Eigen interop  (only available when Eigen headers are included)
// -------------------------------------------------------------------------
#ifdef EIGEN_CORE_H
inline Eigen::Vector3f to_eigen(const Vec3& v) {
    return Eigen::Vector3f(v.x, v.y, v.z);
}
inline Vec3 from_eigen(const Eigen::Vector3f& v) {
    return Vec3{v.x(), v.y(), v.z()};
}
#endif

// -------------------------------------------------------------------------
// Flecs ECS components  (kept for compatibility with existing code)
// -------------------------------------------------------------------------
struct Position       { Vec3 pos; };
struct GeoCoord       { float lat, lon; };       // radians
struct ProvinceId     { uint32_t id; };
struct ProvinceCenter { Vec3 center; };
struct Adjacency      { std::vector<uint32_t> neighbors; };

// -------------------------------------------------------------------------
// Globe -- core mesh + province data
// -------------------------------------------------------------------------
struct Globe {
    // Mesh
    std::vector<Vec3>                      vertices;           // on unit sphere
    std::vector<std::array<uint32_t, 3>>   triangles;

    // Province mapping
    std::vector<uint32_t>                  vertex_province;    // province id per vertex
    std::vector<Vec3>                      province_centers;   // seed points
    std::vector<std::vector<uint32_t>>     province_vertices;  // vertex indices per province

    // Province adjacency (computed from shared triangle edges)
    std::vector<std::vector<uint32_t>>     province_adjacency; // neighbor ids per province

    // Province areas in km^2 (sum of constituent triangle areas)
    std::vector<float>                     province_areas_km2;

    float radius = 6371.0f;  // Earth radius in km (for display scale)

    // ---- Coordinate helpers (static, operate on unit sphere) ----

    /// Geographic (lat/lon radians, Y-up) -> Cartesian unit-sphere point.
    static Vec3 geoToCart(float lat, float lon) {
        const float cos_lat = std::cos(lat);
        return Vec3{
            cos_lat * std::cos(lon),
            std::sin(lat),
            cos_lat * std::sin(lon)
        };
    }

    /// Cartesian -> Geographic.  Returns {lat, lon} in radians.
    static GeoCoord cartToGeo(const Vec3& p) {
        Vec3 n = p.normalized();
        return {
            std::asin(std::clamp(n.y, -1.0f, 1.0f)),
            std::atan2(n.z, n.x)
        };
    }

    // ---- Province adjacency query ----

    /// Are two provinces adjacent?
    bool provinces_adjacent(uint32_t a, uint32_t b) const {
        if (a >= province_adjacency.size()) return false;
        const auto& nbrs = province_adjacency[a];
        return std::find(nbrs.begin(), nbrs.end(), b) != nbrs.end();
    }

    /// Get all neighbors of a province.
    const std::vector<uint32_t>& province_neighbors(uint32_t id) const {
        static const std::vector<uint32_t> empty;
        if (id >= province_adjacency.size()) return empty;
        return province_adjacency[id];
    }
};

}  // namespace fenrir
