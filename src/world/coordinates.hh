#pragma once

// Coordinate spaces and transforms for a 3D globe-based grand strategy game.
//
// Coordinate pipeline:
//   Geographic (lat/lon radians)
//       |  geo_to_cartesian() / cartesian_to_geo()
//       v
//   Cartesian (unit sphere x,y,z) -- the mesh lives here
//       |  world_from_globe() transform
//       v
//   World (after globe rotation + axial tilt)
//       |  view_from_world() transform
//       v
//   View / Camera space
//       |  clip_from_view() transform (perspective projection)
//       v
//   Clip (NDC after w-divide: [-1,1]^3)
//       |  screen_from_clip() / clip_from_screen()
//       v
//   Screen (pixel coordinates, origin top-left)
//
// Conventions:
//   - Latitude in [-pi/2, pi/2], longitude in [-pi, pi], both in RADIANS.
//   - Y is up on the unit sphere: y = sin(lat), xz-plane is the equator.
//   - Right-handed coordinate system.
//   - All matrices are 4x4 homogeneous (Eigen::Matrix4f).

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>
#include <utility>

namespace fenrir {

// ---------------------------------------------------------------------------
// Type aliases
// ---------------------------------------------------------------------------
using Vec3f = Eigen::Vector3f;
using Vec4f = Eigen::Vector4f;
using Mat3f = Eigen::Matrix3f;
using Mat4f = Eigen::Matrix4f;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.0f * kPi;
constexpr float kHalfPi = kPi / 2.0f;
constexpr float kDegToRad = kPi / 180.0f;
constexpr float kRadToDeg = 180.0f / kPi;
constexpr float kEarthRadiusKm = 6371.0f;

// ---------------------------------------------------------------------------
// Degree/radian helpers
// ---------------------------------------------------------------------------
constexpr float deg_to_rad(float degrees) { return degrees * kDegToRad; }
constexpr float rad_to_deg(float radians) { return radians * kRadToDeg; }

// ---------------------------------------------------------------------------
// Geographic <-> Cartesian (unit sphere)
//
// Geographic: lat in [-pi/2, pi/2], lon in [-pi, pi]
// Cartesian:  unit sphere with Y-up
//   x = cos(lat) * cos(lon)
//   y = sin(lat)
//   z = cos(lat) * sin(lon)
// ---------------------------------------------------------------------------

/// Convert geographic coordinates (radians) to a point on the unit sphere.
inline Vec3f geo_to_cartesian(float lat_rad, float lon_rad) {
    const float cos_lat = std::cos(lat_rad);
    return Vec3f(
        cos_lat * std::cos(lon_rad),
        std::sin(lat_rad),
        cos_lat * std::sin(lon_rad)
    );
}

/// Convert a Cartesian point (need not be unit-length) to geographic coords.
/// Returns {latitude, longitude} in radians.
inline std::pair<float, float> cartesian_to_geo(const Vec3f& p) {
    const Vec3f n = p.normalized();
    const float lat = std::asin(std::clamp(n.y(), -1.0f, 1.0f));
    const float lon = std::atan2(n.z(), n.x());
    return {lat, lon};
}

// ---------------------------------------------------------------------------
// Great circle distance on the unit sphere (result in radians).
// Uses the numerically stable formula: d = atan2(|a x b|, a . b)
// ---------------------------------------------------------------------------
inline float great_circle_distance(const Vec3f& a, const Vec3f& b) {
    const Vec3f an = a.normalized();
    const Vec3f bn = b.normalized();
    return std::atan2(an.cross(bn).norm(), an.dot(bn));
}

/// Great circle distance in kilometres (assuming unit sphere = Earth).
inline float great_circle_distance_km(const Vec3f& a, const Vec3f& b) {
    return great_circle_distance(a, b) * kEarthRadiusKm;
}

// ---------------------------------------------------------------------------
// Globe -> World transform  (rotation about Y + axial tilt about Z)
//
// "Globe frame" is where the mesh vertices live (unit sphere, no rotation).
// "World frame" applies the planet's daily rotation and axial tilt.
//   M = Ry(rotation) * Rz(-tilt)
//
// Both angles are in RADIANS.
// ---------------------------------------------------------------------------

/// Build the 4x4 world-from-globe matrix.
///   rotation_rad: rotation about Y axis (planet spin).
///   tilt_rad:     axial tilt about Z axis (applied first, then rotation).
inline Mat4f world_from_globe(float rotation_rad, float tilt_rad) {
    // Ry
    const float cr = std::cos(rotation_rad);
    const float sr = std::sin(rotation_rad);
    Mat3f Ry;
    Ry <<  cr, 0.0f, sr,
          0.0f, 1.0f, 0.0f,
          -sr, 0.0f, cr;

    // Rz(-tilt)  -- negative tilt so north pole tilts toward +x when tilt>0
    const float ct = std::cos(-tilt_rad);
    const float st = std::sin(-tilt_rad);
    Mat3f Rz;
    Rz << ct, -st, 0.0f,
          st,  ct, 0.0f,
         0.0f, 0.0f, 1.0f;

    const Mat3f R = Ry * Rz;

    Mat4f M = Mat4f::Identity();
    M.block<3, 3>(0, 0) = R;
    return M;
}

/// Inverse: globe-from-world (rotation matrices are orthogonal, so transpose).
inline Mat4f globe_from_world(float rotation_rad, float tilt_rad) {
    Mat4f M = world_from_globe(rotation_rad, tilt_rad);
    // For a pure rotation the inverse is the transpose of the 3x3 block.
    M.block<3, 3>(0, 0).transposeInPlace();
    return M;
}

// ---------------------------------------------------------------------------
// View (camera) transform:  view_from_world
//
// Standard look-at matrix.  Camera at `eye` looking at `target`, with `up`.
// Produces a right-handed view matrix (camera looks down -Z in view space).
// ---------------------------------------------------------------------------
inline Mat4f view_from_world(const Vec3f& eye, const Vec3f& target,
                             const Vec3f& up) {
    const Vec3f f = (target - eye).normalized();   // forward
    const Vec3f r = f.cross(up).normalized();       // right
    const Vec3f u = r.cross(f);                     // true up

    Mat4f V = Mat4f::Identity();
    V(0, 0) =  r.x();  V(0, 1) =  r.y();  V(0, 2) =  r.z();  V(0, 3) = -r.dot(eye);
    V(1, 0) =  u.x();  V(1, 1) =  u.y();  V(1, 2) =  u.z();  V(1, 3) = -u.dot(eye);
    V(2, 0) = -f.x();  V(2, 1) = -f.y();  V(2, 2) = -f.z();  V(2, 3) =  f.dot(eye);
    // row 3 stays (0,0,0,1)
    return V;
}

// ---------------------------------------------------------------------------
// Projection:  clip_from_view  (perspective)
//
// Standard OpenGL-style perspective projection.
//   fov_y_rad : vertical field of view in radians
//   aspect    : width / height
//   near, far : clipping planes (must be > 0)
//
// Maps view-space to clip-space.  After w-divide you get NDC in [-1,1]^3.
// ---------------------------------------------------------------------------
inline Mat4f clip_from_view(float fov_y_rad, float aspect,
                            float near_plane, float far_plane) {
    const float t = std::tan(fov_y_rad * 0.5f);

    Mat4f P = Mat4f::Zero();
    P(0, 0) = 1.0f / (aspect * t);
    P(1, 1) = 1.0f / t;
    P(2, 2) = -(far_plane + near_plane) / (far_plane - near_plane);
    P(2, 3) = -2.0f * far_plane * near_plane / (far_plane - near_plane);
    P(3, 2) = -1.0f;
    return P;
}

// ---------------------------------------------------------------------------
// Screen <-> Clip (NDC) conversions
//
// Screen coordinates: x in [0, width), y in [0, height), origin top-left.
// NDC: x in [-1,1], y in [-1,1] (y up after projection).
// ---------------------------------------------------------------------------

/// Convert screen pixel to NDC (z = -1 near plane, z = +1 far plane).
inline Vec3f clip_from_screen(float screen_x, float screen_y,
                              float screen_w, float screen_h, float ndc_z) {
    const float ndc_x = (2.0f * screen_x / screen_w) - 1.0f;
    const float ndc_y = 1.0f - (2.0f * screen_y / screen_h);  // flip y
    return Vec3f(ndc_x, ndc_y, ndc_z);
}

/// Convert NDC to screen pixel coordinates.
/// Returns {screen_x, screen_y}.
inline std::pair<float, float> screen_from_clip(float ndc_x, float ndc_y,
                                                float screen_w, float screen_h) {
    const float sx = (ndc_x + 1.0f) * 0.5f * screen_w;
    const float sy = (1.0f - ndc_y) * 0.5f * screen_h;  // flip y
    return {sx, sy};
}

// ---------------------------------------------------------------------------
// Full MVP (model-view-projection)
// ---------------------------------------------------------------------------
inline Mat4f mvp(float rotation_rad, float tilt_rad,
                 const Vec3f& eye, const Vec3f& target, const Vec3f& up,
                 float fov_y_rad, float aspect,
                 float near_plane, float far_plane) {
    return clip_from_view(fov_y_rad, aspect, near_plane, far_plane)
         * view_from_world(eye, target, up)
         * world_from_globe(rotation_rad, tilt_rad);
}

// ---------------------------------------------------------------------------
// Screen -> world ray (un-projection for picking)
//
// Given a screen-space pixel, produce a ray (origin, direction) in world
// space suitable for intersection tests.
// ---------------------------------------------------------------------------
inline std::pair<Vec3f, Vec3f> screen_to_world_ray(
        float screen_x, float screen_y,
        float screen_w, float screen_h,
        const Mat4f& view, const Mat4f& proj) {
    const Mat4f inv_p = proj.inverse();
    const Mat4f inv_v = view.inverse();

    // Near and far points in NDC
    const Vec3f ndc_near = clip_from_screen(screen_x, screen_y,
                                            screen_w, screen_h, -1.0f);
    const Vec3f ndc_far  = clip_from_screen(screen_x, screen_y,
                                            screen_w, screen_h,  1.0f);

    // Un-project through inverse projection -> camera space
    Vec4f cam_near = inv_p * Vec4f(ndc_near.x(), ndc_near.y(), ndc_near.z(), 1.0f);
    Vec4f cam_far  = inv_p * Vec4f(ndc_far.x(),  ndc_far.y(),  ndc_far.z(),  1.0f);
    cam_near /= cam_near.w();
    cam_far  /= cam_far.w();

    // Camera space -> world space
    const Vec4f world_near = inv_v * cam_near;
    const Vec4f world_far  = inv_v * cam_far;

    const Vec3f origin = world_near.head<3>();
    const Vec3f dir    = (world_far.head<3>() - origin).normalized();
    return {origin, dir};
}

// ---------------------------------------------------------------------------
// Ray-sphere intersection
//
// Ray: P(t) = origin + t * dir,  t >= 0
// Sphere: centered at `center` with `radius`.
//
// Returns the nearest positive t, or -1 if no intersection.
// ---------------------------------------------------------------------------
inline float ray_sphere_intersect(const Vec3f& origin, const Vec3f& dir,
                                  const Vec3f& center, float radius) {
    const Vec3f oc = origin - center;
    // a = dir . dir  (== 1 if dir is normalized, but keep it general)
    const float a = dir.dot(dir);
    const float b = 2.0f * oc.dot(dir);
    const float c = oc.dot(oc) - radius * radius;
    const float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) return -1.0f;

    const float sqrt_disc = std::sqrt(discriminant);
    const float inv_2a = 0.5f / a;
    const float t0 = (-b - sqrt_disc) * inv_2a;
    const float t1 = (-b + sqrt_disc) * inv_2a;

    // Return nearest positive t.
    if (t0 > 0.0f) return t0;
    if (t1 > 0.0f) return t1;
    return -1.0f;
}

// ---------------------------------------------------------------------------
// Ray-triangle intersection (Moller-Trumbore algorithm)
//
// Returns the distance t along the ray, or -1 if no hit.
// ---------------------------------------------------------------------------
inline float ray_triangle_intersect(const Vec3f& origin, const Vec3f& dir,
                                    const Vec3f& v0, const Vec3f& v1,
                                    const Vec3f& v2) {
    constexpr float kEpsilon = 1e-7f;
    const Vec3f e1 = v1 - v0;
    const Vec3f e2 = v2 - v0;
    const Vec3f h  = dir.cross(e2);
    const float a  = e1.dot(h);

    if (a > -kEpsilon && a < kEpsilon) return -1.0f;

    const float f = 1.0f / a;
    const Vec3f s = origin - v0;
    const float u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) return -1.0f;

    const Vec3f q = s.cross(e1);
    const float v = f * dir.dot(q);
    if (v < 0.0f || u + v > 1.0f) return -1.0f;

    const float t = f * e2.dot(q);
    return (t > kEpsilon) ? t : -1.0f;
}

// ---------------------------------------------------------------------------
// Triangle area on the unit sphere (spherical excess via cross product)
//
// For a flat triangle with vertices on the unit sphere this gives the area
// of the corresponding spherical triangle's projection -- which for small
// triangles (icosphere faces) is an excellent approximation.
// Area of flat triangle = 0.5 * |e1 x e2|.
// ---------------------------------------------------------------------------
inline float triangle_area(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) {
    return 0.5f * (v1 - v0).cross(v2 - v0).norm();
}

/// Triangle area in km^2 on the surface of a sphere with the given radius.
inline float triangle_area_km2(const Vec3f& v0, const Vec3f& v1,
                               const Vec3f& v2, float radius_km) {
    return triangle_area(v0, v1, v2) * radius_km * radius_km;
}

// ---------------------------------------------------------------------------
// Transform a point through a 4x4 homogeneous matrix (assumes w=1).
// ---------------------------------------------------------------------------
inline Vec3f transform_point(const Mat4f& M, const Vec3f& p) {
    const Vec4f h = M * Vec4f(p.x(), p.y(), p.z(), 1.0f);
    return h.head<3>();
}

// ---------------------------------------------------------------------------
// Static assertions for type sizes (compile-time sanity checks)
// ---------------------------------------------------------------------------
static_assert(sizeof(float) == 4, "float must be 32-bit IEEE 754");
static_assert(sizeof(Vec3f) == 3 * sizeof(float), "Vec3f must be tightly packed");

}  // namespace fenrir
