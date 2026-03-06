#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>

namespace fenrir {

// Coordinate frames:
//   World    - Globe-centered, Y-up, Z-forward (matches raylib)
//   Globe    - Rotated world frame (axial tilt + rotation)
//   Camera   - Camera-centered, Z into screen
//   Screen   - 2D pixel coordinates

using Vec3f = Eigen::Vector3f;
using Mat3f = Eigen::Matrix3f;
using Mat4f = Eigen::Matrix4f;

struct Transforms {
    // Globe state
    float rotation = 0.0f;              // Y-axis rotation (degrees)
    float axialTilt = 23.5f;            // Earth's axial tilt (degrees)

    // Camera state
    Vec3f camPos = {0, 0, 3.5f};
    Vec3f camTarget = {0, 0, 0};
    Vec3f camUp = {0, 1, 0};
    float fovY = 45.0f;                 // Vertical FOV in degrees
    int screenW = 1280, screenH = 800;

    // T_world_globe: Transform from globe frame to world frame
    // (applies rotation and tilt)
    Mat4f worldFromGlobe() const {
        float rotRad = rotation * M_PI / 180.0f;
        float tiltRad = axialTilt * M_PI / 180.0f;

        // Rotation about Y axis
        Mat3f Ry;
        Ry << cos(rotRad), 0, sin(rotRad),
              0, 1, 0,
              -sin(rotRad), 0, cos(rotRad);

        // Tilt about Z axis
        Mat3f Rz;
        Rz << cos(tiltRad), -sin(tiltRad), 0,
              sin(tiltRad), cos(tiltRad), 0,
              0, 0, 1;

        Mat3f R = Ry * Rz;  // First tilt, then rotate

        Mat4f T = Mat4f::Identity();
        T.block<3,3>(0,0) = R;
        return T;
    }

    // Inverse: globe from world
    Mat4f globeFromWorld() const {
        return worldFromGlobe().inverse();
    }

    // T_camera_world: View matrix (world to camera)
    Mat4f cameraFromWorld() const {
        Vec3f f = (camTarget - camPos).normalized();  // Forward
        Vec3f r = f.cross(camUp).normalized();        // Right
        Vec3f u = r.cross(f);                         // Up

        Mat4f V = Mat4f::Identity();
        V.block<1,3>(0,0) = r.transpose();
        V.block<1,3>(1,0) = u.transpose();
        V.block<1,3>(2,0) = -f.transpose();
        V(0,3) = -r.dot(camPos);
        V(1,3) = -u.dot(camPos);
        V(2,3) = f.dot(camPos);
        return V;
    }

    // Perspective projection matrix
    Mat4f projection() const {
        float aspect = (float)screenW / screenH;
        float fovRad = fovY * M_PI / 180.0f;
        float tanHalf = tan(fovRad / 2.0f);
        float near = 0.1f, far = 100.0f;

        Mat4f P = Mat4f::Zero();
        P(0,0) = 1.0f / (aspect * tanHalf);
        P(1,1) = 1.0f / tanHalf;
        P(2,2) = -(far + near) / (far - near);
        P(2,3) = -2.0f * far * near / (far - near);
        P(3,2) = -1.0f;
        return P;
    }

    // Full MVP: globe -> world -> camera -> clip
    Mat4f mvp() const {
        return projection() * cameraFromWorld() * worldFromGlobe();
    }

    // Unproject screen point to ray in world space
    // Returns (origin, direction)
    std::pair<Vec3f, Vec3f> screenToWorldRay(float screenX, float screenY) const {
        // Normalized device coords (-1 to 1)
        float ndcX = (2.0f * screenX / screenW) - 1.0f;
        float ndcY = 1.0f - (2.0f * screenY / screenH);

        // Inverse projection
        Mat4f invP = projection().inverse();
        Mat4f invV = cameraFromWorld().inverse();

        // Near and far points in NDC
        Eigen::Vector4f nearNDC(ndcX, ndcY, -1.0f, 1.0f);
        Eigen::Vector4f farNDC(ndcX, ndcY, 1.0f, 1.0f);

        // To camera space
        Eigen::Vector4f nearCam = invP * nearNDC;
        Eigen::Vector4f farCam = invP * farNDC;
        nearCam /= nearCam.w();
        farCam /= farCam.w();

        // To world space
        Eigen::Vector4f nearWorld = invV * nearCam;
        Eigen::Vector4f farWorld = invV * farCam;

        Vec3f origin = nearWorld.head<3>();
        Vec3f dir = (farWorld.head<3>() - origin).normalized();

        return {origin, dir};
    }

    // Transform point from globe frame to world frame
    Vec3f globeToWorld(const Vec3f& p) const {
        Eigen::Vector4f h(p.x(), p.y(), p.z(), 1.0f);
        Eigen::Vector4f w = worldFromGlobe() * h;
        return w.head<3>();
    }

    // Ray-triangle intersection (Möller–Trumbore)
    // Returns distance or -1 if no hit
    static float rayTriangle(const Vec3f& orig, const Vec3f& dir,
                             const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) {
        constexpr float kEpsilon = 1e-7f;
        Vec3f e1 = v1 - v0;
        Vec3f e2 = v2 - v0;
        Vec3f h = dir.cross(e2);
        float a = e1.dot(h);

        if (a > -kEpsilon && a < kEpsilon) return -1.0f;

        float f = 1.0f / a;
        Vec3f s = orig - v0;
        float u = f * s.dot(h);
        if (u < 0.0f || u > 1.0f) return -1.0f;

        Vec3f q = s.cross(e1);
        float v = f * dir.dot(q);
        if (v < 0.0f || u + v > 1.0f) return -1.0f;

        float t = f * e2.dot(q);
        return (t > kEpsilon) ? t : -1.0f;
    }
};

}  // namespace fenrir
