#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

struct TestCase {
    std::string name;
    std::function<void()> func;
};

static std::vector<TestCase>& get_tests() {
    static std::vector<TestCase> tests;
    return tests;
}

#define TEST(name) \
    void test_##name(); \
    static bool reg_##name = (get_tests().push_back({#name, test_##name}), true); \
    void test_##name()

#define ASSERT_TRUE(expr) \
    if (!(expr)) { throw std::runtime_error(std::string("ASSERT_TRUE failed: ") + #expr + " at " + __FILE__ + ":" + std::to_string(__LINE__)); }

#define ASSERT_FALSE(expr) \
    if ((expr)) { throw std::runtime_error(std::string("ASSERT_FALSE failed: ") + #expr + " at " + __FILE__ + ":" + std::to_string(__LINE__)); }

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { throw std::runtime_error(std::string("ASSERT_EQ failed: ") + #a + " != " + #b + " at " + __FILE__ + ":" + std::to_string(__LINE__)); }

#define ASSERT_NEAR(a, b, epsilon) \
    if (std::abs((a) - (b)) > (epsilon)) { throw std::runtime_error(std::string("ASSERT_NEAR failed: |") + std::to_string(a) + " - " + std::to_string(b) + "| > " + std::to_string(epsilon) + " at " + __FILE__ + ":" + std::to_string(__LINE__)); }

inline int run_all_tests() {
    int passed = 0, failed = 0;
    for (auto& t : get_tests()) {
        try {
            t.func();
            std::cout << "  PASS: " << t.name << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  FAIL: " << t.name << " - " << e.what() << std::endl;
            failed++;
        }
    }
    std::cout << "\n" << passed << " passed, " << failed << " failed, " << (passed+failed) << " total" << std::endl;
    return failed > 0 ? 1 : 0;
}
