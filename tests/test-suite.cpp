#include <vmafu/core/Vector.hpp>
#include <vmafu/core/Matrix.hpp>

#include "vmafu/core/test-vector.cpp"
#include "vmafu/core/test-matrix.cpp"

#include "test-utils.hpp"


int main() {
    vmafu_test::TestRunner global_runner;
    std::cout << "=== vmafu Library Test Suite ===" << std::endl;
    
    bool all_passed = true;
    
    try {
        vmafu_test::run_vector_tests();
        std::cout << "\n";
    } catch (...) {
        all_passed = false;
    }
    
    try {
        vmafu_test::run_matrix_tests();
        std::cout << "\n";
    } catch (...) {
        all_passed = false;
    }
    
    if (all_passed) {
        std::cout << "=== ALL LIBRARY TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME LIBRARY TESTS FAILED ===" << std::endl;
        return 1;
    }
}
