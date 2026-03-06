#pragma once

#include <flecs.h>
#include <vector>
#include <cmath>
#include <cstdint>

namespace fenrir {

// 3D vector for sphere coordinates
struct Vec3 {
    float x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }

    float dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    float length() const { return std::sqrt(dot(*this)); }
    Vec3 normalized() const { float l = length(); return l > 0 ? *this * (1.0f/l) : *this; }

    // Great circle distance on unit sphere
    float sphereDist(const Vec3& o) const {
        return std::acos(std::clamp(normalized().dot(o.normalized()), -1.0f, 1.0f));
    }
};

// Flecs components
struct Position { Vec3 pos; };           // Position on unit sphere
struct GeoCoord { float lat, lon; };     // Lat/lon in radians
struct ProvinceId { uint32_t id; };      // Which province this vertex belongs to
struct ProvinceCenter { Vec3 center; };  // Province seed point
struct Adjacency { std::vector<uint32_t> neighbors; };

// Globe mesh data
struct Globe {
    std::vector<Vec3> vertices;          // Icosphere vertices (on unit sphere)
    std::vector<std::array<uint32_t, 3>> triangles;
    std::vector<uint32_t> vertex_province; // Province ID per vertex
    std::vector<Vec3> province_centers;    // Province seed points
    std::vector<std::vector<uint32_t>> province_vertices; // Vertices per province
    float radius = 6371.0f;              // Earth radius in km (for scale)

    // Convert between coordinate systems
    static Vec3 geoToCart(float lat, float lon) {
        return {
            std::cos(lat) * std::cos(lon),
            std::sin(lat),
            std::cos(lat) * std::sin(lon)
        };
    }

    static GeoCoord cartToGeo(const Vec3& p) {
        Vec3 n = p.normalized();
        return {
            std::asin(std::clamp(n.y, -1.0f, 1.0f)),
            std::atan2(n.z, n.x)
        };
    }
};

}  // namespace fenrir
