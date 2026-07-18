#include "test_framework.hh"
// Include all test files
#include "test_coordinates.cc"
#include "test_globe_generation.cc"
#include "test_simulation.cc"

int main() {
    std::cout << "=== Fenrir Test Suite ===" << std::endl;
    return run_all_tests();
}
