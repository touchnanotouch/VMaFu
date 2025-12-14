#pragma once


#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <functional>


namespace vmafu_test {
    class TestRunner {
        private:
            size_t _passed = 0;
            size_t _failed = 0;
            std::string _current_test;

        public:
            void start_test(const std::string& test_name) {
                _current_test = test_name;
                std::cout << "TEST: " << test_name << std::endl;
            }

            void end_test() {
                std::cout << "-> " << _current_test << " completed" << std::endl;
                _current_test.clear();
            }

            template <typename T, typename U>
            void assert_equal(const T& actual, const U& expected, const std::string& message = "") {
                if (actual == expected) {
                    _passed++;
                    std::cout << "  PASS: " << message << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
                    throw std::runtime_error("Assertion failed: " + message);
                }
            }

            void assert_equal(double actual, double expected, const std::string& message = "") {
                if (std::abs(actual - expected) < 1e-10) {
                    _passed++;
                    std::cout << "  PASS: " << message << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
                    throw std::runtime_error("Assertion failed: " + message);
                }
            }

            void assert_equal(float actual, float expected, const std::string& message = "") {
                if (std::abs(actual - expected) < 1e-6f) {
                    _passed++;
                    std::cout << "  PASS: " << message << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
                    throw std::runtime_error("Assertion failed: " + message);
                }
            }

            template <typename T>
            void assert_almost_equal(T actual, T expected, T epsilon, const std::string& message = "") {
                if (std::abs(actual - expected) < epsilon) {
                    _passed++;
                    std::cout << "  PASS: " << message << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (expected: " << expected << ", actual: " << actual << ")" << std::endl;
                    throw std::runtime_error("Assertion failed: " + message);
                }
            }

            void assert_true(bool condition, const std::string& message = "") {
                if (condition) {
                    _passed++;
                    std::cout << "  PASS: " << message << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << std::endl;
                    throw std::runtime_error("Assertion failed: " + message);
                }
            }

            void assert_false(bool condition, const std::string& message = "") {
                assert_true(!condition, message);
            }

            template <typename ExceptionType>
            void assert_throws(const std::function<void()>& func, const std::string& message = "") {
                bool caught = false;
                try {
                    func();
                } catch (const ExceptionType&) {
                    caught = true;
                } catch (const std::exception& e) {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (wrong exception type: " << e.what() << ")" << std::endl;
                    throw std::runtime_error("Wrong exception type in: " + message);
                } catch (...) {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (wrong exception type: unknown)" << std::endl;
                    throw std::runtime_error("Wrong exception type in: " + message);
                }

                if (caught) {
                    _passed++;
                    std::cout << "  PASS: " << message << " (exception correctly thrown)" << std::endl;
                } else {
                    _failed++;
                    std::cerr << "  FAIL: " << message << " (expected exception not thrown)" << std::endl;
                    throw std::runtime_error("Expected exception not thrown in: " + message);
                }
            }

            void print_summary() const {
                std::cout << "\n=== TEST SUMMARY ===" << std::endl;
                std::cout << "Passed: " << _passed << std::endl;
                std::cout << "Failed: " << _failed << std::endl;
                std::cout << "Total:  " << (_passed + _failed) << std::endl;
                
                if (_failed == 0) {
                    std::cout << "=== ALL TESTS PASSED ===" << std::endl;
                } else {
                    std::cout << "=== SOME TESTS FAILED ===" << std::endl;
                }
            }

            bool all_passed() const { return _failed == 0; }
    };
}


#define TEST_START(test_name) runner.start_test(test_name)
#define TEST_END() runner.end_test()
#define ASSERT_EQ(actual, expected) runner.assert_equal(actual, expected, #actual " == " #expected)
#define ASSERT_NEAR(actual, expected, epsilon) runner.assert_almost_equal(actual, expected, epsilon, #actual " ≈ " #expected)
#define ASSERT_TRUE(condition) runner.assert_true(condition, #condition)
#define ASSERT_FALSE(condition) runner.assert_false(condition, "!(" #condition ")")

#define ASSERT_THROWS(expression, exception_type) \
    do { \
        auto lambda = [&]() { expression; }; \
        runner.assert_throws<exception_type>(lambda, #expression " throws " #exception_type); \
    } while(0)
