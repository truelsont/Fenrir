// Tests for GlobeGenerator (globe_generator.hh / .cc).
//
// Validates icosphere construction, province assignment, and mesh integrity.

#include "test_framework.hh"
#include "src/world/globe.hh"
#include "src/world/globe_generator.hh"

#include <cmath>
#include <set>

using namespace fenrir;

static constexpr float kGlobeEps = 1e-4f;

// ---------------------------------------------------------------------------
// Icosphere vertex counts
// ---------------------------------------------------------------------------
// An icosahedron starts with V=12, E=30, F=20.
// Each subdivision: F' = 4F, each original edge gains 1 midpoint.
// V(n) = 10 * 4^n + 2  (for n >= 0)
//   n=0: 12,  n=1: 42,  n=2: 162,  n=3: 642

TEST(globe_gen_subdivision0_vertex_count) {
    Globe g = GlobeGenerator::generate(0, 4, /*seed=*/42);
    ASSERT_EQ(g.vertices.size(), 12u);
    ASSERT_EQ(g.triangles.size(), 20u);
}

TEST(globe_gen_subdivision1_vertex_count) {
    Globe g = GlobeGenerator::generate(1, 4, 42);
    ASSERT_EQ(g.vertices.size(), 42u);
    ASSERT_EQ(g.triangles.size(), 80u);
}

TEST(globe_gen_subdivision2_vertex_count) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    ASSERT_EQ(g.vertices.size(), 162u);
    ASSERT_EQ(g.triangles.size(), 320u);
}

TEST(globe_gen_subdivision3_vertex_count) {
    Globe g = GlobeGenerator::generate(3, 32, 42);
    ASSERT_EQ(g.vertices.size(), 642u);
    ASSERT_EQ(g.triangles.size(), 1280u);
}

// ---------------------------------------------------------------------------
// All vertices on unit sphere
// ---------------------------------------------------------------------------

TEST(globe_gen_all_vertices_on_unit_sphere) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    for (size_t i = 0; i < g.vertices.size(); ++i) {
        ASSERT_NEAR(g.vertices[i].length(), 1.0f, kGlobeEps);
    }
}

// ---------------------------------------------------------------------------
// Province assignment
// ---------------------------------------------------------------------------

TEST(globe_gen_province_count_matches_request) {
    constexpr uint32_t kNumProvinces = 16;
    Globe g = GlobeGenerator::generate(2, kNumProvinces, 42);
    ASSERT_EQ(g.province_centers.size(), kNumProvinces);
    ASSERT_EQ(g.province_vertices.size(), kNumProvinces);
}

TEST(globe_gen_every_vertex_has_province) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    ASSERT_EQ(g.vertex_province.size(), g.vertices.size());
    for (uint32_t pid : g.vertex_province) {
        ASSERT_TRUE(pid < g.province_centers.size());
    }
}

TEST(globe_gen_no_province_is_empty) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    for (size_t p = 0; p < g.province_vertices.size(); ++p) {
        ASSERT_TRUE(!g.province_vertices[p].empty());
    }
}

TEST(globe_gen_province_vertex_lists_cover_all) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    // Total vertex count across all province_vertices should equal vertex count.
    size_t total = 0;
    for (const auto& pv : g.province_vertices) {
        total += pv.size();
    }
    ASSERT_EQ(total, g.vertices.size());
}

TEST(globe_gen_province_centers_on_unit_sphere) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    for (const auto& c : g.province_centers) {
        ASSERT_NEAR(c.length(), 1.0f, kGlobeEps);
    }
}

// ---------------------------------------------------------------------------
// Triangle validity
// ---------------------------------------------------------------------------

TEST(globe_gen_triangles_have_valid_indices) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    uint32_t nv = static_cast<uint32_t>(g.vertices.size());
    for (const auto& tri : g.triangles) {
        ASSERT_TRUE(tri[0] < nv);
        ASSERT_TRUE(tri[1] < nv);
        ASSERT_TRUE(tri[2] < nv);
    }
}

TEST(globe_gen_no_degenerate_triangles) {
    Globe g = GlobeGenerator::generate(2, 16, 42);
    for (const auto& tri : g.triangles) {
        // All three indices should be distinct.
        ASSERT_TRUE(tri[0] != tri[1]);
        ASSERT_TRUE(tri[1] != tri[2]);
        ASSERT_TRUE(tri[0] != tri[2]);
    }
}

TEST(globe_gen_triangle_normals_point_outward) {
    // For a convex unit-sphere mesh, the cross product of two edges
    // should point away from the origin (positive dot with centroid).
    Globe g = GlobeGenerator::generate(2, 16, 42);
    for (const auto& tri : g.triangles) {
        Vec3 a = g.vertices[tri[0]];
        Vec3 b = g.vertices[tri[1]];
        Vec3 c = g.vertices[tri[2]];

        // Edge vectors
        Vec3 e1 = b - a;
        Vec3 e2 = c - a;

        // Cross product (normal)
        Vec3 n = {
            e1.y * e2.z - e1.z * e2.y,
            e1.z * e2.x - e1.x * e2.z,
            e1.x * e2.y - e1.y * e2.x
        };

        // Centroid of the triangle
        Vec3 centroid = (a + b + c) * (1.0f / 3.0f);

        // Normal should point in the same direction as the centroid (outward).
        float d = n.dot(centroid);
        ASSERT_TRUE(d > 0.0f);
    }
}

// ---------------------------------------------------------------------------
// Determinism
// ---------------------------------------------------------------------------

TEST(globe_gen_deterministic_with_same_seed) {
    Globe g1 = GlobeGenerator::generate(2, 16, 42);
    Globe g2 = GlobeGenerator::generate(2, 16, 42);

    ASSERT_EQ(g1.vertices.size(), g2.vertices.size());
    ASSERT_EQ(g1.triangles.size(), g2.triangles.size());

    for (size_t i = 0; i < g1.vertices.size(); ++i) {
        ASSERT_NEAR(g1.vertices[i].x, g2.vertices[i].x, kGlobeEps);
        ASSERT_NEAR(g1.vertices[i].y, g2.vertices[i].y, kGlobeEps);
        ASSERT_NEAR(g1.vertices[i].z, g2.vertices[i].z, kGlobeEps);
    }

    for (size_t i = 0; i < g1.vertex_province.size(); ++i) {
        ASSERT_EQ(g1.vertex_province[i], g2.vertex_province[i]);
    }
}

TEST(globe_gen_different_seed_gives_different_provinces) {
    Globe g1 = GlobeGenerator::generate(2, 16, 42);
    Globe g2 = GlobeGenerator::generate(2, 16, 99);

    // Province centers should differ (with overwhelming probability).
    bool any_differ = false;
    for (size_t i = 0; i < g1.province_centers.size(); ++i) {
        float dist = g1.province_centers[i].sphereDist(g2.province_centers[i]);
        if (dist > 0.01f) {
            any_differ = true;
            break;
        }
    }
    ASSERT_TRUE(any_differ);
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

TEST(globe_gen_single_province) {
    Globe g = GlobeGenerator::generate(1, 1, 42);
    ASSERT_EQ(g.province_centers.size(), 1u);
    ASSERT_EQ(g.province_vertices.size(), 1u);
    // All vertices should belong to the single province.
    ASSERT_EQ(g.province_vertices[0].size(), g.vertices.size());
}

TEST(globe_gen_many_provinces) {
    // More provinces than vertices at subdivision 0 (12 verts, 20 provinces).
    // Some provinces may end up empty in this extreme case, but we still
    // test that the generator doesn't crash.
    Globe g = GlobeGenerator::generate(0, 20, 42);
    ASSERT_EQ(g.province_centers.size(), 20u);
    ASSERT_EQ(g.vertex_province.size(), g.vertices.size());
}
