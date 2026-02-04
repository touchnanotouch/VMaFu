#include <vmafu/core/_Matrix.hpp>

#include "test-utils.hpp"


namespace vmafu_test {
    void test_matrix_constructors(TestRunner& runner) {
        TEST_START("Matrix Constructors");
        
        vmafu::Matrix<int> m1;
        ASSERT_TRUE(m1.empty());
        ASSERT_EQ(m1.rows(), 0);
        ASSERT_EQ(m1.cols(), 0);
        
        vmafu::Matrix<int> m2(2, 3);
        ASSERT_EQ(m2.rows(), 2);
        ASSERT_EQ(m2.cols(), 3);
        ASSERT_EQ(m2.size(), 6);
        
        vmafu::Matrix<int> m3(2, 2, 5);
        ASSERT_EQ(m3(0, 0), 5);
        ASSERT_EQ(m3(1, 1), 5);
        
        vmafu::Matrix<int> m4{{1, 2, 3}, {4, 5, 6}};
        ASSERT_EQ(m4.rows(), 2);
        ASSERT_EQ(m4.cols(), 3);
        ASSERT_EQ(m4(0, 0), 1);
        ASSERT_EQ(m4(1, 2), 6);
        
        TEST_END();
    }

    void test_matrix_operations(TestRunner& runner) {
        TEST_START("Matrix Operations");
        
        vmafu::Matrix<int> m1{{1, 2}, {3, 4}};
        vmafu::Matrix<int> m2{{5, 6}, {7, 8}};
        
        // Addition
        auto sum = m1 + m2;
        ASSERT_EQ(sum(0, 0), 6);
        ASSERT_EQ(sum(1, 1), 12);
        
        // Multiplication
        auto product = m1 * m2;
        ASSERT_EQ(product(0, 0), 19); // 1*5 + 2*7
        ASSERT_EQ(product(0, 1), 22); // 1*6 + 2*8
        ASSERT_EQ(product(1, 0), 43); // 3*5 + 4*7
        ASSERT_EQ(product(1, 1), 50); // 3*6 + 4*8
        
        // Matrix-vector multiplication
        vmafu::Vector<int> v{2, 3};
        auto mv_product = m1 * v;
        ASSERT_EQ(mv_product.size(), 2);
        ASSERT_EQ(mv_product[0], 8);  // 1*2 + 2*3
        ASSERT_EQ(mv_product[1], 18); // 3*2 + 4*3
        
        TEST_END();
    }

    void test_matrix_static_methods(TestRunner& runner) {
        TEST_START("Matrix Static Methods");
        
        auto identity = vmafu::Matrix<double>::identity(3);
        ASSERT_TRUE(identity.is_square());
        ASSERT_EQ(identity(0, 0), 1.0);
        ASSERT_EQ(identity(1, 1), 1.0);
        ASSERT_EQ(identity(2, 2), 1.0);
        ASSERT_EQ(identity(0, 1), 0.0);
        
        auto zeros = vmafu::Matrix<int>::zeros(2, 3);
        ASSERT_EQ(zeros.rows(), 2);
        ASSERT_EQ(zeros.cols(), 3);
        for (size_t i = 0; i < zeros.rows(); ++i) {
            for (size_t j = 0; j < zeros.cols(); ++j) {
                ASSERT_EQ(zeros(i, j), 0);
            }
        }
        
        auto ones = vmafu::Matrix<int>::ones(2, 2);
        for (size_t i = 0; i < ones.rows(); ++i) {
            for (size_t j = 0; j < ones.cols(); ++j) {
                ASSERT_EQ(ones(i, j), 1);
            }
        }
        
        vmafu::Vector<int> diag_vec{1, 2, 3};
        auto diag = vmafu::Matrix<int>::diagonal(diag_vec);
        ASSERT_EQ(diag(0, 0), 1);
        ASSERT_EQ(diag(1, 1), 2);
        ASSERT_EQ(diag(2, 2), 3);
        ASSERT_EQ(diag(0, 1), 0);
        
        TEST_END();
    }

    void run_matrix_tests() {
        TestRunner runner;
        std::cout << "=== vmafu::Matrix Test Suite ===" << std::endl;
        
        try {
            test_matrix_constructors(runner);
            test_matrix_operations(runner);
            test_matrix_static_methods(runner);
            // Добавьте другие тестовые функции здесь
            
            runner.print_summary();
            if (!runner.all_passed()) {
                throw std::runtime_error("Some matrix tests failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Matrix tests failed: " << e.what() << std::endl;
            throw;
        }
    }
}
