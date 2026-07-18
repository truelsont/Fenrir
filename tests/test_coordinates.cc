// Tests for Globe coordinate transforms (globe.hh).
//
// Coordinate convention in fenrir:
//   geoToCart(lat, lon) where lat/lon are in radians.
//   Y is the polar axis (sin(lat)), X = cos(lat)*cos(lon), Z = cos(lat)*sin(lon).
//
// So equator/0-lon => (1, 0, 0),
//    north pole    => (0, 1, 0),
//    south pole    => (0,-1, 0),
//    90E equator   => (0, 0, 1).

#include "test_framework.hh"
#include "src/world/globe.hh"

#include <cmath>

using namespace fenrir;

static constexpr float kCoordEps = 1e-5f;
static constexpr float kCPi = static_cast<float>(M_PI);

// ---------------------------------------------------------------------------
// geoToCart
// ---------------------------------------------------------------------------

TEST(geoToCart_equator_zero_lon) {
    Vec3 v = Globe::geoToCart(0.0f, 0.0f);
    ASSERT_NEAR(v.x, 1.0f, kCoordEps);
    ASSERT_NEAR(v.y, 0.0f, kCoordEps);
    ASSERT_NEAR(v.z, 0.0f, kCoordEps);
}

TEST(geoToCart_north_pole) {
    Vec3 v = Globe::geoToCart(kCPi / 2.0f, 0.0f);
    ASSERT_NEAR(v.x, 0.0f, kCoordEps);
    ASSERT_NEAR(v.y, 1.0f, kCoordEps);
    ASSERT_NEAR(v.z, 0.0f, kCoordEps);
}

TEST(geoToCart_south_pole) {
    Vec3 v = Globe::geoToCart(-kCPi / 2.0f, 0.0f);
    ASSERT_NEAR(v.x, 0.0f, kCoordEps);
    ASSERT_NEAR(v.y, -1.0f, kCoordEps);
    ASSERT_NEAR(v.z, 0.0f, kCoordEps);
}

TEST(geoToCart_equator_90E) {
    Vec3 v = Globe::geoToCart(0.0f, kCPi / 2.0f);
    ASSERT_NEAR(v.x, 0.0f, kCoordEps);
    ASSERT_NEAR(v.y, 0.0f, kCoordEps);
    ASSERT_NEAR(v.z, 1.0f, kCoordEps);
}

TEST(geoToCart_equator_180) {
    Vec3 v = Globe::geoToCart(0.0f, kCPi);
    ASSERT_NEAR(v.x, -1.0f, kCoordEps);
    ASSERT_NEAR(v.y, 0.0f, kCoordEps);
    ASSERT_NEAR(v.z, 0.0f, kCoordEps);
}

TEST(geoToCart_equator_90W) {
    Vec3 v = Globe::geoToCart(0.0f, -kCPi / 2.0f);
    ASSERT_NEAR(v.x, 0.0f, kCoordEps);
    ASSERT_NEAR(v.y, 0.0f, kCoordEps);
    ASSERT_NEAR(v.z, -1.0f, kCoordEps);
}

TEST(geoToCart_result_on_unit_sphere) {
    // Arbitrary lat/lon should produce a unit vector.
    Vec3 v = Globe::geoToCart(0.7f, -1.2f);
    ASSERT_NEAR(v.length(), 1.0f, kCoordEps);
}

// ---------------------------------------------------------------------------
// cartToGeo
// ---------------------------------------------------------------------------

TEST(cartToGeo_roundtrip_equator) {
    Vec3 v = Globe::geoToCart(0.0f, 0.0f);
    GeoCoord g = Globe::cartToGeo(v);
    ASSERT_NEAR(g.lat, 0.0f, kCoordEps);
    ASSERT_NEAR(g.lon, 0.0f, kCoordEps);
}

TEST(cartToGeo_roundtrip_north_pole) {
    Vec3 v = Globe::geoToCart(kCPi / 2.0f, 0.0f);
    GeoCoord g = Globe::cartToGeo(v);
    ASSERT_NEAR(g.lat, kCPi / 2.0f, kCoordEps);
    // Longitude is undefined at the pole; we just check lat.
}

TEST(cartToGeo_roundtrip_random_points) {
    // A few deterministic "random" points.
    float lats[] = {0.3f, -0.8f, 1.1f, -1.5f, 0.0f};
    float lons[] = {1.5f, -2.3f, 0.5f, 3.0f, -1.0f};

    for (int i = 0; i < 5; ++i) {
        Vec3 v = Globe::geoToCart(lats[i], lons[i]);
        GeoCoord g = Globe::cartToGeo(v);
        ASSERT_NEAR(g.lat, lats[i], kCoordEps);
        ASSERT_NEAR(g.lon, lons[i], kCoordEps);
    }
}

// ---------------------------------------------------------------------------
// Vec3::sphereDist (great-circle distance on unit sphere)
// ---------------------------------------------------------------------------

TEST(sphereDist_same_point_is_zero) {
    Vec3 v = Globe::geoToCart(0.3f, 1.2f);
    ASSERT_NEAR(v.sphereDist(v), 0.0f, kCoordEps);
}

TEST(sphereDist_antipodal_points_is_pi) {
    Vec3 a = Globe::geoToCart(0.0f, 0.0f);
    Vec3 b = Globe::geoToCart(0.0f, kCPi);
    ASSERT_NEAR(a.sphereDist(b), kCPi, kCoordEps);
}

TEST(sphereDist_equator_90_degrees_apart) {
    Vec3 a = Globe::geoToCart(0.0f, 0.0f);
    Vec3 b = Globe::geoToCart(0.0f, kCPi / 2.0f);
    ASSERT_NEAR(a.sphereDist(b), kCPi / 2.0f, kCoordEps);
}

TEST(sphereDist_north_to_south_pole) {
    Vec3 n = Globe::geoToCart(kCPi / 2.0f, 0.0f);
    Vec3 s = Globe::geoToCart(-kCPi / 2.0f, 0.0f);
    ASSERT_NEAR(n.sphereDist(s), kCPi, kCoordEps);
}

TEST(sphereDist_symmetric) {
    Vec3 a = Globe::geoToCart(0.2f, 0.5f);
    Vec3 b = Globe::geoToCart(-0.7f, 2.1f);
    ASSERT_NEAR(a.sphereDist(b), b.sphereDist(a), kCoordEps);
}

// ---------------------------------------------------------------------------
// Vec3 basic operations
// ---------------------------------------------------------------------------

TEST(vec3_normalized_length_is_one) {
    Vec3 v = {3.0f, 4.0f, 0.0f};
    ASSERT_NEAR(v.normalized().length(), 1.0f, kCoordEps);
}

TEST(vec3_dot_orthogonal_is_zero) {
    Vec3 a = {1.0f, 0.0f, 0.0f};
    Vec3 b = {0.0f, 1.0f, 0.0f};
    ASSERT_NEAR(a.dot(b), 0.0f, kCoordEps);
}

TEST(vec3_add) {
    Vec3 a = {1.0f, 2.0f, 3.0f};
    Vec3 b = {4.0f, 5.0f, 6.0f};
    Vec3 c = a + b;
    ASSERT_NEAR(c.x, 5.0f, kCoordEps);
    ASSERT_NEAR(c.y, 7.0f, kCoordEps);
    ASSERT_NEAR(c.z, 9.0f, kCoordEps);
}

TEST(vec3_subtract) {
    Vec3 a = {4.0f, 5.0f, 6.0f};
    Vec3 b = {1.0f, 2.0f, 3.0f};
    Vec3 c = a - b;
    ASSERT_NEAR(c.x, 3.0f, kCoordEps);
    ASSERT_NEAR(c.y, 3.0f, kCoordEps);
    ASSERT_NEAR(c.z, 3.0f, kCoordEps);
}

TEST(vec3_scale) {
    Vec3 a = {1.0f, 2.0f, 3.0f};
    Vec3 c = a * 2.0f;
    ASSERT_NEAR(c.x, 2.0f, kCoordEps);
    ASSERT_NEAR(c.y, 4.0f, kCoordEps);
    ASSERT_NEAR(c.z, 6.0f, kCoordEps);
}

// ---------------------------------------------------------------------------
// Degree / radian helpers (inline, no dependency -- we just verify math)
// ---------------------------------------------------------------------------

TEST(deg_rad_roundtrip) {
    auto deg_to_rad = [](float d) { return d * kCPi / 180.0f; };
    auto rad_to_deg = [](float r) { return r * 180.0f / kCPi; };

    float degrees[] = {0.0f, 45.0f, 90.0f, 180.0f, 360.0f, -30.0f};
    for (float d : degrees) {
        ASSERT_NEAR(rad_to_deg(deg_to_rad(d)), d, kCoordEps);
    }
}
