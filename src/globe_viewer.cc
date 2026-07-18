#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "world/globe_generator.hh"
#include "world/province_data.hh"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>
#include <cstdio>

using namespace fenrir;
using Vec3f = Eigen::Vector3f;
using Mat4f = Eigen::Matrix4f;

// Build the same model matrix that rlRotatef applies
// rlRotatef applies rotations to the current matrix in order called
// So: rlRotatef(rotY, 0,1,0) then rlRotatef(rotZ, 0,0,1)
// means M = RotY * RotZ (vertices transform as M * v)
Mat4f buildModelMatrix(float rotationDeg, float tiltDeg) {
    float rotRad = rotationDeg * DEG2RAD;
    float tiltRad = tiltDeg * DEG2RAD;

    // rlRotatef(rotation, 0, 1, 0) - rotation about Y axis
    Eigen::Matrix3f Ry;
    Ry = Eigen::AngleAxisf(rotRad, Vec3f::UnitY());

    // rlRotatef(-tilt, 0, 0, 1) - rotation about Z axis
    Eigen::Matrix3f Rz;
    Rz = Eigen::AngleAxisf(-tiltRad, Vec3f::UnitZ());

    // OpenGL/rlgl multiplies: CurrentMatrix = CurrentMatrix * NewRotation
    // So after both calls: M = Ry * Rz
    Eigen::Matrix3f R = Ry * Rz;

    Mat4f M = Mat4f::Identity();
    M.block<3,3>(0,0) = R;
    return M;
}

// Transform point from globe frame to world frame
Vec3f transformPoint(const Mat4f& M, const Vec3f& p) {
    Eigen::Vector4f h(p.x(), p.y(), p.z(), 1.0f);
    Eigen::Vector4f w = M * h;
    return w.head<3>();
}

// Möller–Trumbore ray-triangle intersection
float rayTriangleIntersect(Vector3 rayOrigin, Vector3 rayDir,
                           Vector3 v0, Vector3 v1, Vector3 v2) {
    constexpr float kEps = 1e-6f;

    Vector3 e1 = Vector3Subtract(v1, v0);
    Vector3 e2 = Vector3Subtract(v2, v0);
    Vector3 h = Vector3CrossProduct(rayDir, e2);
    float a = Vector3DotProduct(e1, h);

    if (a > -kEps && a < kEps) return -1.0f;

    float f = 1.0f / a;
    Vector3 s = Vector3Subtract(rayOrigin, v0);
    float u = f * Vector3DotProduct(s, h);

    if (u < 0.0f || u > 1.0f) return -1.0f;

    Vector3 q = Vector3CrossProduct(s, e1);
    float v = f * Vector3DotProduct(rayDir, q);

    if (v < 0.0f || u + v > 1.0f) return -1.0f;

    float t = f * Vector3DotProduct(e2, q);
    return (t > kEps) ? t : -1.0f;
}

struct WorldState {
    Globe globe;
    std::vector<ProvinceData> provinces;
    std::vector<Color> colors;
    uint32_t num_provinces = 64;
    uint32_t subdivisions = 4;
    uint64_t seed = 12345;
    int selected = -1;

    float rotation = 0.0f;
    float axialTilt = 23.5f;

    void generate() {
        globe = GlobeGenerator::generate(subdivisions, num_provinces, seed);
        provinces.resize(num_provinces);
        colors.resize(num_provinces);
        for (uint32_t i = 0; i < num_provinces; ++i) {
            provinces[i] = generateProvinceData(i, seed);
            colors[i] = terrainColor(static_cast<uint8_t>(provinces[i].terrain), i);
        }
        selected = -1;
        printf("Generated: %zu verts, %zu tris, %u provinces\n",
               globe.vertices.size(), globe.triangles.size(), num_provinces);
    }

    Color terrainColor(uint8_t terrain, uint32_t id) {
        float hue = (float)id / num_provinces;
        int brightness = 180 + (int)(hue * 40);
        switch (terrain) {
            case 0: return {30, 60, (unsigned char)(120 + id % 40), 255};
            case 1: return {60, (unsigned char)brightness, 60, 255};
            case 2: return {(unsigned char)(brightness-40), (unsigned char)(brightness-20), 80, 255};
            case 3: return {(unsigned char)(brightness-60), (unsigned char)(brightness-60), (unsigned char)(brightness-40), 255};
            case 4: return {(unsigned char)brightness, (unsigned char)(brightness-40), 60, 255};
            default: return {128, 128, 128, 255};
        }
    }

    int pickProvince(Camera3D& cam, Vector2 mousePos) {
        // Use raylib's GetScreenToWorldRay - this correctly handles
        // the camera's view and projection matrices
        Ray ray = GetScreenToWorldRay(mousePos, cam);

        // Build the same model matrix we use for rendering
        Mat4f M = buildModelMatrix(rotation, axialTilt);

        float closest = 1e9f;
        int picked = -1;

        for (const auto& tri : globe.triangles) {
            // Transform triangle vertices from globe space to world space
            // using the same model matrix that rlRotatef builds
            Vec3f a_globe(globe.vertices[tri[0]].x, globe.vertices[tri[0]].y, globe.vertices[tri[0]].z);
            Vec3f b_globe(globe.vertices[tri[1]].x, globe.vertices[tri[1]].y, globe.vertices[tri[1]].z);
            Vec3f c_globe(globe.vertices[tri[2]].x, globe.vertices[tri[2]].y, globe.vertices[tri[2]].z);

            Vec3f a_world = transformPoint(M, a_globe);
            Vec3f b_world = transformPoint(M, b_globe);
            Vec3f c_world = transformPoint(M, c_globe);

            Vector3 va = {a_world.x(), a_world.y(), a_world.z()};
            Vector3 vb = {b_world.x(), b_world.y(), b_world.z()};
            Vector3 vc = {c_world.x(), c_world.y(), c_world.z()};

            // Test front face
            float t = rayTriangleIntersect(ray.position, ray.direction, va, vb, vc);
            if (t > 0 && t < closest) {
                closest = t;
                picked = globe.vertex_province[tri[0]];
            }

            // Test back face (reverse winding)
            t = rayTriangleIntersect(ray.position, ray.direction, va, vc, vb);
            if (t > 0 && t < closest) {
                closest = t;
                picked = globe.vertex_province[tri[0]];
            }
        }
        return picked;
    }
};

int main() {
    const int W = 1280, H = 800;
    InitWindow(W, H, "Fenrir Globe");
    SetTargetFPS(60);

    WorldState ws;
    ws.generate();

    Camera3D camera = {0};
    camera.position = {0.0f, 0.0f, 3.5f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    bool autoRotate = false;
    bool wireframe = false;

    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_SPACE)) autoRotate = !autoRotate;
        if (IsKeyPressed(KEY_W)) wireframe = !wireframe;
        if (IsKeyPressed(KEY_R)) { ws.seed = GetRandomValue(0, 999999); ws.generate(); }
        if (IsKeyPressed(KEY_UP)) { ws.num_provinces = Clamp(ws.num_provinces + 16, 8, 512); ws.generate(); }
        if (IsKeyPressed(KEY_DOWN)) { ws.num_provinces = Clamp(ws.num_provinces - 16, 8, 512); ws.generate(); }
        if (IsKeyPressed(KEY_PAGE_UP)) { ws.subdivisions = Clamp(ws.subdivisions + 1, 2, 6); ws.generate(); }
        if (IsKeyPressed(KEY_PAGE_DOWN)) { ws.subdivisions = Clamp(ws.subdivisions - 1, 2, 6); ws.generate(); }

        float dt = GetFrameTime();
        if (autoRotate) ws.rotation += dt * 15.0f;
        if (IsKeyDown(KEY_LEFT)) ws.rotation -= 60 * dt;
        if (IsKeyDown(KEY_RIGHT)) ws.rotation += 60 * dt;

        float wheel = GetMouseWheelMove();
        camera.position.z = Clamp(camera.position.z - wheel * 0.3f, 1.5f, 10.0f);

        // Province selection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ws.selected = ws.pickProvince(camera, GetMousePosition());
        }

        BeginDrawing();
        ClearBackground({15, 15, 25, 255});

        BeginMode3D(camera);

        // Apply globe rotation - this builds the model matrix
        rlPushMatrix();
        rlRotatef(ws.rotation, 0, 1, 0);
        rlRotatef(-ws.axialTilt, 0, 0, 1);

        for (const auto& tri : ws.globe.triangles) {
            Vec3 a = ws.globe.vertices[tri[0]];
            Vec3 b = ws.globe.vertices[tri[1]];
            Vec3 c = ws.globe.vertices[tri[2]];
            uint32_t prov = ws.globe.vertex_province[tri[0]];

            Color col = ws.colors[prov];
            if ((int)prov == ws.selected) col = {255, 255, 100, 255};

            if (wireframe) {
                DrawLine3D({a.x, a.y, a.z}, {b.x, b.y, b.z}, col);
                DrawLine3D({b.x, b.y, b.z}, {c.x, c.y, c.z}, col);
                DrawLine3D({c.x, c.y, c.z}, {a.x, a.y, a.z}, col);
            } else {
                DrawTriangle3D({a.x, a.y, a.z}, {b.x, b.y, b.z}, {c.x, c.y, c.z}, col);
                DrawTriangle3D({a.x, a.y, a.z}, {c.x, c.y, c.z}, {b.x, b.y, b.z}, col);
            }
        }

        rlPopMatrix();
        EndMode3D();

        // UI
        DrawRectangle(10, 10, 280, 100, {0, 0, 0, 180});
        DrawText("Fenrir Globe", 20, 15, 20, WHITE);
        DrawText(TextFormat("Verts: %d  Tris: %d", (int)ws.globe.vertices.size(), (int)ws.globe.triangles.size()), 20, 40, 16, LIGHTGRAY);
        DrawText(TextFormat("Provinces: %d  Subdivs: %d", ws.num_provinces, ws.subdivisions), 20, 58, 16, LIGHTGRAY);
        DrawText(TextFormat("Seed: %llu", ws.seed), 20, 76, 16, GRAY);

        if (ws.selected >= 0 && ws.selected < (int)ws.provinces.size()) {
            const auto& p = ws.provinces[ws.selected];
            int panelX = W - 260, panelY = 10;
            DrawRectangle(panelX, panelY, 250, 140, {0, 0, 0, 200});
            DrawText(p.name.c_str(), panelX + 10, panelY + 10, 18, WHITE);
            DrawText(TextFormat("Terrain: %s", terrainName(p.terrain)), panelX + 10, panelY + 35, 16, LIGHTGRAY);
            DrawText(TextFormat("Population: %d", p.population), panelX + 10, panelY + 55, 16, LIGHTGRAY);
            DrawText(TextFormat("Tax: %.1f", p.tax_income), panelX + 10, panelY + 75, 16, LIGHTGRAY);
            DrawText(TextFormat("Fertility: %.0f%%", p.fertility * 100), panelX + 10, panelY + 95, 16, LIGHTGRAY);
            DrawText(TextFormat("Minerals: %.0f%%", p.minerals * 100), panelX + 10, panelY + 115, 16, LIGHTGRAY);
        }

        DrawRectangle(10, H - 75, 500, 65, {0, 0, 0, 150});
        DrawText("[SPACE] Auto-rotate  [W] Wireframe  [R] New seed", 20, H - 65, 14, GRAY);
        DrawText("[UP/DOWN] Province count  [PGUP/PGDN] Mesh detail", 20, H - 48, 14, GRAY);
        DrawText("[LEFT/RIGHT] Rotate  [SCROLL] Zoom  [CLICK] Select", 20, H - 31, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
