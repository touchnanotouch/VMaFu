#include <vmafu/core/_Vector.hpp>

#include "test-utils.hpp"


namespace vmafu_test {
    void test_vector_constructors(TestRunner& runner) {
        TEST_START("Vector Constructors");
        
        vmafu::Vector<int> v1;
        ASSERT_TRUE(v1.empty());
        ASSERT_EQ(v1.size(), 0);
        
        vmafu::Vector<int> v2(5);
        ASSERT_EQ(v2.size(), 5);
        ASSERT_FALSE(v2.empty());
        for (size_t i = 0; i < v2.size(); ++i) {
            ASSERT_EQ(v2[i], 0);
        }
        
        vmafu::Vector<int> v3(3, 42);
        ASSERT_EQ(v3.size(), 3);
        for (size_t i = 0; i < v3.size(); ++i) {
            ASSERT_EQ(v3[i], 42);
        }
        
        vmafu::Vector<int> v4{1, 2, 3, 4, 5};
        ASSERT_EQ(v4.size(), 5);
        ASSERT_EQ(v4[0], 1);
        ASSERT_EQ(v4[4], 5);
        
        // Copy constructor
        vmafu::Vector<int> v5 = v4;
        ASSERT_EQ(v5.size(), 5);
        ASSERT_EQ(v5[0], 1);
        
        // Move constructor
        vmafu::Vector<int> v6 = std::move(v5);
        ASSERT_EQ(v6.size(), 5);
        ASSERT_EQ(v6[0], 1);
        ASSERT_TRUE(v5.empty());
        
        TEST_END();
    };

    void test_vector_operations(TestRunner& runner) {
        TEST_START("Vector Operations");
        
        vmafu::Vector<int> v1{1, 2, 3};
        vmafu::Vector<int> v2{4, 5, 6};
        
        // Arithmetic
        auto sum = v1 + v2;
        ASSERT_EQ(sum[0], 5);
        ASSERT_EQ(sum[1], 7);
        ASSERT_EQ(sum[2], 9);
        
        auto diff = v1 - v2;
        ASSERT_EQ(diff[0], -3);
        ASSERT_EQ(diff[1], -3);
        ASSERT_EQ(diff[2], -3);
        
        // Linear algebra
        ASSERT_EQ(v1.dot(v2), 32); // 1*4 + 2*5 + 3*6
        
        vmafu::Vector<double> v3{3.0, 4.0};
        ASSERT_NEAR(v3.norm(), 5.0, 1e-10);
        
        // Error cases
        vmafu::Vector<int> wrong_size{1, 2};
        ASSERT_THROWS(v1 + wrong_size, std::invalid_argument);
        ASSERT_THROWS(v1 / 0, std::invalid_argument);
        
        TEST_END();
    };

    void run_vector_tests() {
        TestRunner runner;
        std::cout << "=== vmafu::Vector Test Suite ===" << std::endl;
        
        try {
            test_vector_constructors(runner);
            test_vector_operations(runner);
            
            runner.print_summary();
            if (!runner.all_passed()) {
                throw std::runtime_error("Some vector tests failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Vector tests failed: " << e.what() << std::endl;
            throw;
        }
    };
}
