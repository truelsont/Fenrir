#pragma once

#include <cstdint>

namespace fenrir {

// ---------------------------------------------------------------------------
// Province components
// ---------------------------------------------------------------------------

struct ProvinceTag {
    uint32_t id;
};

struct Economy {
    float tax_income = 0.0f;
    float production = 0.0f;
    float trade_value = 0.0f;
    float development = 1.0f;       // 1.0 - 10.0
    float local_trade_power = 0.0f;
};

struct Population {
    uint32_t total = 0;
    float growth_rate = 0.002f;     // Annual % growth (0.2% default)
    float literacy = 0.1f;          // 0 - 1, affects tech spread
};

struct Military {
    float manpower = 0.0f;          // Available recruits
    float fortification = 0.0f;    // 0 - 10
    float supply_limit = 10000.0f; // Max troops before attrition
};

struct ProvinceOwner {
    uint32_t nation_id = 0;
    uint32_t controller_id = 0;
};

// ---------------------------------------------------------------------------
// Nation components
// ---------------------------------------------------------------------------

struct NationTag {
    uint32_t id;
};

struct Treasury {
    float gold = 0.0f;
    float monthly_income = 0.0f;
    float monthly_expenses = 0.0f;
};

struct NationManpower {
    float available = 0.0f;
    float max_manpower = 0.0f;
    float monthly_recovery = 0.0f;
};

struct Technology {
    float admin_tech = 1.0f;    // 1 - 32 (EU4 range)
    float diplo_tech = 1.0f;
    float military_tech = 1.0f;
};

struct Diplomacy {
    float prestige = 0.0f;        // -100 to 100
    float stability = 0.0f;       // -3 to 3
    float war_exhaustion = 0.0f;  // 0 - 20
};

// ---------------------------------------------------------------------------
// Army components
// ---------------------------------------------------------------------------

struct ArmyTag {
    uint32_t id;
};

struct ArmyPosition {
    uint32_t province_id;
};

struct ArmyStrength {
    uint32_t infantry = 0;
    uint32_t cavalry = 0;
    uint32_t artillery = 0;
    float morale = 1.0f;         // 0 - 1
    float organization = 1.0f;   // 0 - 1
};

struct ArmyOwner {
    uint32_t nation_id;
};

}  // namespace fenrir
