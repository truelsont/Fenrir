#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "world/globe_generator.hh"
#include "world/province_data.hh"
#include "engine/simulation.hh"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

using namespace fenrir;
using Vec3f = Eigen::Vector3f;
using Mat4f = Eigen::Matrix4f;

// ---------------------------------------------------------------------------
// Fonts — loaded once, used everywhere
// ---------------------------------------------------------------------------

struct Fonts {
    Font regular;
    Font bold;
    bool loaded = false;

    void load() {
        // Try common system font paths
        const char* regularPaths[] = {
            "assets/fonts/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/System/Library/Fonts/SFNSText.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "C:\\Windows\\Fonts\\segoeui.ttf",
            nullptr
        };
        const char* boldPaths[] = {
            "assets/fonts/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
            "/System/Library/Fonts/SFNSText-Bold.otf",
            "C:\\Windows\\Fonts\\segoeuib.ttf",
            nullptr
        };

        int sizes[] = {16, 18, 20, 24, 28, 32, 0};

        for (int i = 0; regularPaths[i]; ++i) {
            if (FileExists(regularPaths[i])) {
                regular = LoadFontEx(regularPaths[i], 32, nullptr, 0);
                SetTextureFilter(regular.texture, TEXTURE_FILTER_BILINEAR);
                loaded = true;
                break;
            }
        }
        for (int i = 0; boldPaths[i]; ++i) {
            if (FileExists(boldPaths[i])) {
                bold = LoadFontEx(boldPaths[i], 32, nullptr, 0);
                SetTextureFilter(bold.texture, TEXTURE_FILTER_BILINEAR);
                break;
            }
        }
        if (!loaded) {
            regular = GetFontDefault();
            bold = GetFontDefault();
        }
        if (bold.glyphCount == 0) bold = regular;
    }

    void unload() {
        if (loaded) {
            UnloadFont(regular);
            if (bold.texture.id != regular.texture.id) UnloadFont(bold);
        }
    }
};

static Fonts gFonts;

static void drawText(const char* text, int x, int y, int size, Color col) {
    DrawTextEx(gFonts.regular, text, {(float)x, (float)y}, (float)size, 1.0f, col);
}

static void drawTextBold(const char* text, int x, int y, int size, Color col) {
    DrawTextEx(gFonts.bold, text, {(float)x, (float)y}, (float)size, 1.0f, col);
}

// ---------------------------------------------------------------------------
// Number formatting helpers
// ---------------------------------------------------------------------------

static const char* fmtInt(int v) {
    // Add thousand separators: 1234567 -> "1,234,567"
    static char buf[32];
    char raw[20];
    snprintf(raw, sizeof(raw), "%d", v < 0 ? -v : v);
    int len = (int)strlen(raw);
    int commas = (len - 1) / 3;
    int total = len + commas + (v < 0 ? 1 : 0);
    buf[total] = '\0';
    int dst = total - 1;
    for (int i = len - 1, c = 0; i >= 0; --i) {
        buf[dst--] = raw[i];
        if (++c == 3 && i > 0) { buf[dst--] = ','; c = 0; }
    }
    if (v < 0) buf[0] = '-';
    return buf;
}

static const char* fmtFloat1(float v) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%.1f", v);
    return buf;
}

static const char* fmtFloat0(float v) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%.0f", v);
    return buf;
}

static const char* fmtPct(float v) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "%.2f%%", v * 100.0f);
    return buf;
}

// ---------------------------------------------------------------------------
// Globe math
// ---------------------------------------------------------------------------

static Mat4f buildModelMatrix(float rotYDeg, float tiltDeg) {
    Eigen::Matrix3f Ry;
    Ry = Eigen::AngleAxisf(rotYDeg * DEG2RAD, Vec3f::UnitY());
    Eigen::Matrix3f Rz;
    Rz = Eigen::AngleAxisf(-tiltDeg * DEG2RAD, Vec3f::UnitZ());
    Mat4f M = Mat4f::Identity();
    M.block<3,3>(0,0) = Ry * Rz;
    return M;
}

static Vec3f xformPoint(const Mat4f& M, const Vec3f& p) {
    Eigen::Vector4f h(p.x(), p.y(), p.z(), 1.0f);
    return (M * h).head<3>();
}

static float rayTriHit(Vector3 o, Vector3 d, Vector3 v0, Vector3 v1, Vector3 v2) {
    constexpr float kE = 1e-6f;
    Vector3 e1 = Vector3Subtract(v1, v0), e2 = Vector3Subtract(v2, v0);
    Vector3 h = Vector3CrossProduct(d, e2);
    float a = Vector3DotProduct(e1, h);
    if (a > -kE && a < kE) return -1.0f;
    float f = 1.0f / a;
    Vector3 s = Vector3Subtract(o, v0);
    float u = f * Vector3DotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return -1.0f;
    Vector3 q = Vector3CrossProduct(s, e1);
    float v = f * Vector3DotProduct(d, q);
    if (v < 0.0f || u + v > 1.0f) return -1.0f;
    float t = f * Vector3DotProduct(e2, q);
    return (t > kE) ? t : -1.0f;
}

// ---------------------------------------------------------------------------
// Orbit camera
// ---------------------------------------------------------------------------

struct OrbitCamera {
    float yaw = 0.0f;
    float pitch = 0.3f;
    float distance = 3.5f;

    Camera3D toRaylib() const {
        float cy = cosf(yaw), sy = sinf(yaw);
        float cp = cosf(pitch), sp = sinf(pitch);
        Camera3D cam = {0};
        cam.position = {distance * cp * sy, distance * sp, distance * cp * cy};
        cam.target = {0.0f, 0.0f, 0.0f};
        cam.up = {0.0f, 1.0f, 0.0f};
        cam.fovy = 45.0f;
        cam.projection = CAMERA_PERSPECTIVE;
        return cam;
    }

    void zoom(float delta) {
        distance = Clamp(distance - delta * 0.3f, 1.5f, 10.0f);
    }

    void orbit(float dx, float dy) {
        yaw += dx * 0.005f;
        pitch = Clamp(pitch + dy * 0.005f, -1.4f, 1.4f);
    }
};

// ---------------------------------------------------------------------------
// Globe state
// ---------------------------------------------------------------------------

struct GlobeState {
    Globe globe;
    std::vector<ProvinceData> provinces;
    std::vector<Color> colors;
    uint32_t num_provinces = 256;
    uint32_t subdivisions = 5;
    uint64_t seed = 12345;
    int selected = -1;
    float axialTilt = 23.5f;

    void generate() {
        globe = GlobeGenerator::generate(subdivisions, num_provinces, seed);
        provinces.resize(num_provinces);
        colors.resize(num_provinces);
        for (uint32_t i = 0; i < num_provinces; ++i) {
            provinces[i] = generateProvinceData(i, seed);
            colors[i] = makeColor(provinces[i].terrain, i);
        }
        selected = -1;
    }

    Color makeColor(TerrainType terrain, uint32_t id) {
        float h = static_cast<float>(id) / num_provinces;
        int b = 160 + static_cast<int>(h * 60);
        auto uc = [](int v) { return static_cast<unsigned char>(Clamp(v, 0, 255)); };
        switch (terrain) {
            case TerrainType::Ocean:     return {uc(25 + (int)(id % 15)), uc(50 + (int)(id % 20)), uc(110 + (int)(id % 40)), 255};
            case TerrainType::Plains:    return {uc(80 + (int)(id % 30)), uc(b + 10), uc(60 + (int)(id % 20)), 255};
            case TerrainType::Forest:    return {uc(40 + (int)(id % 20)), uc(b - 30), uc(50 + (int)(id % 15)), 255};
            case TerrainType::Hills:     return {uc(b - 40), uc(b - 30), uc(b - 60), 255};
            case TerrainType::Mountains: return {uc(b - 20), uc(b - 30), uc(b - 40), 255};
            case TerrainType::Desert:    return {uc(b + 30), uc(b + 10), uc(100 + (int)(id % 20)), 255};
            case TerrainType::Tundra:    return {uc(b), uc(b + 10), uc(b + 20), 255};
            default:                     return {128, 128, 128, 255};
        }
    }

    int pickProvince(Camera3D& cam) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), cam);
        Mat4f M = buildModelMatrix(0.0f, axialTilt);
        float closest = 1e9f;
        int picked = -1;
        for (const auto& tri : globe.triangles) {
            Vec3f aw = xformPoint(M, {globe.vertices[tri[0]].x, globe.vertices[tri[0]].y, globe.vertices[tri[0]].z});
            Vec3f bw = xformPoint(M, {globe.vertices[tri[1]].x, globe.vertices[tri[1]].y, globe.vertices[tri[1]].z});
            Vec3f cw = xformPoint(M, {globe.vertices[tri[2]].x, globe.vertices[tri[2]].y, globe.vertices[tri[2]].z});
            Vector3 va = {aw.x(), aw.y(), aw.z()};
            Vector3 vb = {bw.x(), bw.y(), bw.z()};
            Vector3 vc = {cw.x(), cw.y(), cw.z()};
            float t = rayTriHit(ray.position, ray.direction, va, vb, vc);
            if (t > 0 && t < closest) { closest = t; picked = globe.vertex_province[tri[0]]; }
            t = rayTriHit(ray.position, ray.direction, va, vc, vb);
            if (t > 0 && t < closest) { closest = t; picked = globe.vertex_province[tri[0]]; }
        }
        return picked;
    }
};

// ---------------------------------------------------------------------------
// UI
// ---------------------------------------------------------------------------

static const char* monthName(uint32_t m) {
    static const char* names[] = {
        "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    return (m >= 1 && m <= 12) ? names[m] : "???";
}

static void drawPanel(int x, int y, int w, int h, Color bg = {16, 18, 26, 230}) {
    DrawRectangle(x, y, w, h, bg);
    DrawRectangleLines(x, y, w, h, {50, 55, 70, 220});
}

static bool pointInRect(Vector2 p, int x, int y, int w, int h) {
    return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
}

static void drawTopBar(int W, const Simulation& sim, bool running, int speed, uint32_t playerNation) {
    drawPanel(0, 0, W, 48);

    // Date
    drawTextBold(TextFormat("%d %s, %d",
        sim.current_day(), monthName(sim.current_month()), sim.current_year()),
        16, 12, 22, WHITE);

    // Speed controls
    int sx = 260;
    int speedValues[] = {1, 3, 7, 15, 30};
    const char* speedLabels[] = {"I", "II", "III", "IV", "V"};

    // Pause button
    Color pauseCol = running ? Color{55, 55, 65, 255} : Color{180, 50, 50, 255};
    DrawRectangleRounded({(float)sx, 8, 32, 32}, 0.3f, 4, pauseCol);
    drawTextBold("||", sx + 8, 13, 18, WHITE);
    sx += 38;

    for (int i = 0; i < 5; ++i) {
        bool active = running && speed == speedValues[i];
        Color c = active ? Color{50, 140, 50, 255} : Color{55, 55, 65, 255};
        DrawRectangleRounded({(float)sx, 8, 36, 32}, 0.3f, 4, c);
        drawText(speedLabels[i], sx + 8, 14, 16, WHITE);
        sx += 40;
    }

    // Divider
    DrawRectangle(sx + 10, 8, 1, 32, {50, 55, 70, 200});

    // Player nation resources
    const Treasury* t = sim.getNationTreasury(playerNation);
    int rx = sx + 22;
    if (t) {
        drawText("Gold", rx, 6, 13, {180, 160, 100, 200});
        drawTextBold(fmtInt((int)t->gold), rx, 24, 16, GOLD);
        rx += 100;

        const NationManpower* mp = sim.getNationManpower(playerNation);
        if (mp) {
            drawText("Manpower", rx, 6, 13, {140, 180, 140, 200});
            drawTextBold(TextFormat("%s / %s", fmtInt((int)mp->available), fmtInt((int)mp->max_manpower)),
                rx, 24, 16, {160, 220, 160, 255});
            rx += 180;
        }

        const Technology* tech = sim.getNationTechnology(playerNation);
        if (tech) {
            drawText("Tech", rx, 6, 13, {140, 160, 200, 200});
            drawTextBold(TextFormat("%s / %s / %s",
                fmtFloat1(tech->admin_tech), fmtFloat1(tech->diplo_tech), fmtFloat1(tech->military_tech)),
                rx, 24, 16, {160, 180, 230, 255});
        }
    }

    // FPS
    drawText(TextFormat("FPS: %d", GetFPS()), W - 80, 16, 14, {100, 100, 110, 200});
}

static int handleSpeedClick(Vector2 mouse, bool& running, int currentSpeed) {
    int sx = 260;
    int speedValues[] = {1, 3, 7, 15, 30};
    if (pointInRect(mouse, sx, 8, 32, 32)) { running = !running; return currentSpeed; }
    sx += 38;
    for (int i = 0; i < 5; ++i) {
        if (pointInRect(mouse, sx, 8, 36, 32)) { running = true; return speedValues[i]; }
        sx += 40;
    }
    return currentSpeed;
}

static void drawLabelValue(int x, int& y, const char* label, const char* value,
                            Color labelCol, Color valueCol, int labelSize = 14, int valueSize = 16) {
    drawText(label, x, y, labelSize, labelCol);
    int labelWidth = MeasureTextEx(gFonts.regular, label, (float)labelSize, 1.0f).x;
    drawTextBold(value, x + labelWidth + 6, y, valueSize, valueCol);
    y += valueSize + 4;
}

static void drawProvincePanel(int W, int H, int selected, const GlobeState& gs,
                               const Simulation& sim) {
    if (selected < 0 || selected >= static_cast<int>(gs.provinces.size())) return;

    const auto& p = gs.provinces[selected];
    int pw = 300, ph = 360;
    int px = W - pw - 12, py = 58;
    drawPanel(px, py, pw, ph);

    int tx = px + 14, ty = py + 12;

    // Province name
    drawTextBold(p.name.c_str(), tx, ty, 22, WHITE);
    ty += 30;
    DrawRectangle(tx, ty, pw - 28, 1, {50, 55, 70, 200});
    ty += 8;

    // Terrain
    drawText(TextFormat("Terrain: %s", terrainName(p.terrain)), tx, ty, 15, {170, 170, 180, 255});
    ty += 24;

    // Economy section
    const Economy* econ = sim.getProvinceEconomy(selected);
    drawTextBold("Economy", tx, ty, 14, GOLD);
    ty += 20;
    if (econ) {
        Color vCol = {220, 220, 230, 255};
        Color lCol = {140, 140, 150, 255};
        drawLabelValue(tx + 8, ty, "Development", fmtFloat1(econ->development), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Tax Income", fmtFloat1(econ->tax_income), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Production", fmtFloat1(econ->production), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Trade Value", fmtFloat1(econ->trade_value), lCol, vCol);
    }

    ty += 6;
    DrawRectangle(tx, ty, pw - 28, 1, {40, 45, 55, 150});
    ty += 8;

    // Population section
    const Population* pop = sim.getProvincePopulation(selected);
    drawTextBold("Population", tx, ty, 14, {100, 200, 100, 255});
    ty += 20;
    if (pop) {
        Color vCol = {220, 220, 230, 255};
        Color lCol = {140, 140, 150, 255};
        drawLabelValue(tx + 8, ty, "Total", fmtInt(pop->total), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Growth", fmtPct(pop->growth_rate), lCol, vCol);
    }

    ty += 6;
    DrawRectangle(tx, ty, pw - 28, 1, {40, 45, 55, 150});
    ty += 8;

    // Military section
    const Military* mil = sim.getProvinceMilitary(selected);
    drawTextBold("Military", tx, ty, 14, {200, 110, 110, 255});
    ty += 20;
    if (mil) {
        Color vCol = {220, 220, 230, 255};
        Color lCol = {140, 140, 150, 255};
        drawLabelValue(tx + 8, ty, "Manpower", fmtInt((int)mil->manpower), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Fort Level", fmtFloat1(mil->fortification), lCol, vCol);
        drawLabelValue(tx + 8, ty, "Supply", fmtInt((int)mil->supply_limit), lCol, vCol);
    }

    // Owner
    const ProvinceOwner* owner = sim.getProvinceOwner(selected);
    if (owner) {
        ty += 8;
        DrawRectangle(tx, ty, pw - 28, 1, {40, 45, 55, 150});
        ty += 8;
        const Treasury* nt = sim.getNationTreasury(owner->nation_id);
        drawTextBold(TextFormat("Nation %d", owner->nation_id), tx, ty, 15, {200, 180, 100, 255});
        ty += 22;
        if (nt) {
            drawText(TextFormat("Treasury: %s gold", fmtInt((int)nt->gold)),
                tx + 8, ty, 14, {180, 170, 120, 255});
        }
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main() {
    const int W = 1400, H = 900;
    InitWindow(W, H, "Fenrir");
    SetTargetFPS(60);

    gFonts.load();

    GlobeState gs;
    gs.generate();

    Simulation sim;
    uint32_t numNations = 16;
    sim.initialize(gs.num_provinces, numNations);
    uint32_t playerNation = 0;

    OrbitCamera orbit;
    bool simRunning = false;
    int simSpeed = 1;
    bool wireframe = false;
    bool dragging = false;

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        // Mouse drag to rotate globe
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouse.y > 48) {
            dragging = true;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            dragging = false;
        }
        if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            orbit.orbit(-delta.x, -delta.y);
        }

        orbit.zoom(GetMouseWheelMove());

        // Right-click to select province
        Camera3D cam3d = orbit.toRaylib();
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && mouse.y > 48) {
            gs.selected = gs.pickProvince(cam3d);
        }

        // Top bar clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouse.y <= 48) {
            simSpeed = handleSpeedClick(mouse, simRunning, simSpeed);
        }

        // Keyboard
        if (IsKeyPressed(KEY_SPACE)) simRunning = !simRunning;
        if (IsKeyPressed(KEY_W)) wireframe = !wireframe;
        if (IsKeyPressed(KEY_ONE))   { simSpeed = 1;  simRunning = true; }
        if (IsKeyPressed(KEY_TWO))   { simSpeed = 3;  simRunning = true; }
        if (IsKeyPressed(KEY_THREE)) { simSpeed = 7;  simRunning = true; }
        if (IsKeyPressed(KEY_FOUR))  { simSpeed = 15; simRunning = true; }
        if (IsKeyPressed(KEY_FIVE))  { simSpeed = 30; simRunning = true; }
        if (IsKeyPressed(KEY_ESCAPE)) gs.selected = -1;
        if (IsKeyPressed(KEY_R)) {
            gs.seed = GetRandomValue(0, 999999);
            gs.generate();
            sim.initialize(gs.num_provinces, numNations);
        }

        // Sim tick
        if (simRunning) {
            sim.tick(static_cast<float>(simSpeed));
        }

        // --- Render ---
        BeginDrawing();
        ClearBackground({8, 8, 14, 255});

        BeginMode3D(cam3d);
        rlPushMatrix();
        rlRotatef(-gs.axialTilt, 0, 0, 1);

        for (const auto& tri : gs.globe.triangles) {
            Vec3 a = gs.globe.vertices[tri[0]];
            Vec3 b = gs.globe.vertices[tri[1]];
            Vec3 c = gs.globe.vertices[tri[2]];
            uint32_t prov = gs.globe.vertex_province[tri[0]];

            Color col = gs.colors[prov];
            if (static_cast<int>(prov) == gs.selected) {
                col = {255, 255, 100, 255};
            }

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

        // --- UI overlays ---
        drawTopBar(W, sim, simRunning, simSpeed, playerNation);
        drawProvincePanel(W, H, gs.selected, gs, sim);

        // Controls hint
        drawPanel(10, H - 36, 580, 28);
        drawText("LMB: rotate    Scroll: zoom    RMB: select    Space: pause    1-5: speed    W: wireframe    R: regen",
            18, H - 32, 13, {100, 100, 110, 200});

        EndDrawing();
    }

    gFonts.unload();
    CloseWindow();
    return 0;
}
