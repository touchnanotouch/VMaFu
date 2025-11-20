#pragma once


#include <algorithm>
#include <cmath>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <initializer_list>

#include "Vector.hpp"


namespace vmafu {
    // Main class

    template <typename ResultT, typename... ArgTs>
    class Function;
    
    // Function 1-d class

    template <typename ResultT, typename ArgT>
    class Function<ResultT, ArgT> {
        private:
            std::function<ResultT(ArgT)> _func;
            double _eps = 1e-10;

            // Checks

            void _check_initialized(const char* op) const;
        public:
            // Constructors/Destructor

            Function();
            Function(std::function<ResultT(ArgT)> func);
            Function(const Function& other);
            Function(Function&& other);

            ~Function();

            // Getters

            double eps() const noexcept;
            bool is_initialized() const noexcept;

            // Setters

            void set_eps(double new_eps);

            // Copy/Move operators

            Function& operator=(const Function& other);
            Function& operator=(Function&& other);

            // Access operators

            ResultT operator()(ArgT x) const;
            ResultT at(ArgT x) const;

            // Arithmetic operators with functions

            Function operator+(const Function& rhs) const;
            Function operator-(const Function& rhs) const;
            Function operator*(const Function& rhs) const;
            Function operator/(const Function& rhs) const;

            Function& operator+=(const Function& rhs);
            Function& operator-=(const Function& rhs);
            Function& operator*=(const Function& rhs);
            Function& operator/=(const Function& rhs);

            // Arithmetic operators with scalars

            Function operator+(ResultT scalar) const;
            Function operator-(ResultT scalar) const;
            Function operator*(ResultT scalar) const;
            Function operator/(ResultT scalar) const;

            Function& operator+=(ResultT scalar);
            Function& operator-=(ResultT scalar);
            Function& operator*=(ResultT scalar);
            Function& operator/=(ResultT scalar);

            // Function composition

            template<typename InnerResultT>
            Function<ResultT, InnerResultT> compose(const Function<InnerResultT, ArgT>& inner) const;

            // Calculus operators (только для вещественных типов)

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, Function<ResultT, ArgT>>
            derivative(ArgT h = 1e-6) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, ResultT>
            definite_integral(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, Function<ResultT, ArgT>>
            integral(ArgT a, ArgT b, size_t n = 1000) const;

            // Functional properties (только для вещественных типов)

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, ResultT>
            norm(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, ResultT>
            maximum(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, ResultT>
            minimum(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, ResultT>
            root(ArgT a, ArgT b, size_t n = 100) const;

            // Functional analysis

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, bool>
            is_continuous(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, bool>
            is_monotonic(ArgT a, ArgT b, size_t n = 1000) const;

            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, bool>
            has_root(ArgT a, ArgT b, size_t n = 100) const;

            // Comparison operators

            bool operator==(const Function& rhs) const;
            bool operator!=(const Function& rhs) const;

            // Static methods

            static Function constant(ResultT value);

            static Function linear(ResultT intercept, ResultT slope);

            static Function polynomial(const Vector<ResultT>& coefficients);

            static Function exponential(ResultT base = static_cast<ResultT>(M_E));
            static Function logarithmic(ResultT base = static_cast<ResultT>(M_E));

            static Function trigonometric_sin();
            static Function trigonometric_cos();
            static Function trigonometric_tan();

            static Function from_function_ptr(ResultT (*func)(ArgT));

            // Friend methods

            template <typename U, typename V>
            friend std::ostream& operator<<(std::ostream& os, const Function<U, V>& f);
    };

    // Function 2-d class

    template <typename ResultT, typename ArgT1, typename ArgT2>
    class Function<ResultT, ArgT1, ArgT2> {
        private:
            std::function<ResultT(ArgT1, ArgT2)> _func;
            double _eps = 1e-10;

            void _check_initialized(const char* op) const {
                if (!_func) {
                    throw std::runtime_error(std::string("Function not initialized for ") + op);
                }
            }

        public:
            // Constructors
            Function() : _func(nullptr) {}
            Function(std::function<ResultT(ArgT1, ArgT2)> func) : _func(func) {}
            Function(const Function& other) = default;
            Function(Function&& other) = default;
            ~Function() = default;

            // Getters/Setters
            double eps() const noexcept { return _eps; }
            bool is_initialized() const noexcept { return static_cast<bool>(_func); }
            void set_eps(double new_eps) { _eps = new_eps; }

            // Copy/Move operators
            Function& operator=(const Function& other) = default;
            Function& operator=(Function&& other) = default;

            // Evaluation
            ResultT operator()(ArgT1 x, ArgT2 y) const {
                _check_initialized("function evaluation");
                return _func(x, y);
            }

            ResultT at(ArgT1 x, ArgT2 y) const { return (*this)(x, y); }

            // Частичное применение (currying)
            Function<ResultT, ArgT2> bind_first(ArgT1 x_val) const {
                _check_initialized("partial application");
                return Function<ResultT, ArgT2>([*this, x_val](ArgT2 y) { 
                    return (*this)(x_val, y); 
                });
            }

            Function<ResultT, ArgT1> bind_second(ArgT2 y_val) const {
                _check_initialized("partial application");
                return Function<ResultT, ArgT1>([*this, y_val](ArgT1 x) { 
                    return (*this)(x, y_val); 
                });
            }

            // Arithmetic operations
            Function operator+(const Function& rhs) const {
                _check_initialized("addition");
                rhs._check_initialized("addition");
                
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, rhs](ArgT1 x, ArgT2 y) { 
                        return (*this)(x, y) + rhs(x, y); 
                    })
                );
            }

            Function operator-(const Function& rhs) const {
                _check_initialized("subtraction");
                rhs._check_initialized("subtraction");
                
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, rhs](ArgT1 x, ArgT2 y) { 
                        return (*this)(x, y) - rhs(x, y); 
                    })
                );
            }

            Function operator*(const Function& rhs) const {
                _check_initialized("multiplication");
                rhs._check_initialized("multiplication");
                
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, rhs](ArgT1 x, ArgT2 y) { 
                        return (*this)(x, y) * rhs(x, y); 
                    })
                );
            }

            Function operator/(const Function& rhs) const {
                _check_initialized("division");
                rhs._check_initialized("division");
                
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, rhs](ArgT1 x, ArgT2 y) { 
                        ResultT denominator = rhs(x, y);
                        if constexpr (std::is_floating_point_v<ResultT>) {
                            if (std::abs(denominator) < _eps) {
                                throw std::invalid_argument("Division by zero in function");
                            }
                        } else {
                            if (denominator == ResultT{0}) {
                                throw std::invalid_argument("Division by zero in function");
                            }
                        }
                        return (*this)(x, y) / denominator; 
                    })
                );
            }

            // Arithmetic with scalars
            Function operator+(ResultT scalar) const {
                _check_initialized("scalar addition");
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, scalar](ArgT1 x, ArgT2 y) { 
                        return (*this)(x, y) + scalar; 
                    })
                );
            }

            Function operator*(ResultT scalar) const {
                _check_initialized("scalar multiplication");
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [this, scalar](ArgT1 x, ArgT2 y) { 
                        return (*this)(x, y) * scalar; 
                    })
                );
            }

            // Static factory methods
            static Function constant(ResultT value) {
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [value](ArgT1 x, ArgT2 y) { return value; })
                );
            }

            static Function from_separable(const Function<ResultT, ArgT1>& f, 
                                        const Function<ResultT, ArgT2>& g) {
                return Function(std::function<ResultT(ArgT1, ArgT2)>(
                    [f, g](ArgT1 x, ArgT2 y) { return f(x) * g(y); })
                );
            }

            // Для интегральных уравнений - вычисление интеграла по одной переменной
            template<typename U = ResultT>
            std::enable_if_t<std::is_floating_point_v<U>, Function<ResultT, ArgT1>>
            integrate_second(ArgT2 a, ArgT2 b, size_t n_segments = 100) const {
                _check_initialized("integration");
                
                return Function<ResultT, ArgT1>([*this, a, b, n_segments](ArgT1 x) {
                    ArgT2 h = (b - a) / n_segments;
                    ResultT sum = ((*this)(x, a) + (*this)(x, b)) / 2;
                    for (size_t i = 1; i < n_segments; i++) {
                        ArgT2 t = a + i * h;
                        sum += (*this)(x, t);
                    }
                    return sum * h;
                });
            }

            // Comparison operators
            bool operator==(const Function& rhs) const {
                if (!is_initialized() && !rhs.is_initialized()) return true;
                if (!is_initialized() || !rhs.is_initialized()) return false;
                
                // Simple test - not comprehensive for 2D functions
                const ArgT1 test_x1 = -1, test_x2 = 1;
                const ArgT2 test_y1 = -1, test_y2 = 1;
                const size_t test_n = 10;
                
                ArgT1 hx = (test_x2 - test_x1) / test_n;
                ArgT2 hy = (test_y2 - test_y1) / test_n;
                
                for (size_t i = 0; i <= test_n; i++) {
                    for (size_t j = 0; j <= test_n; j++) {
                        ArgT1 x = test_x1 + i * hx;
                        ArgT2 y = test_y1 + j * hy;
                        if constexpr (std::is_floating_point_v<ResultT>) {
                            if (std::abs((*this)(x, y) - rhs(x, y)) > _eps) return false;
                        } else {
                            if ((*this)(x, y) != rhs(x, y)) return false;
                        }
                    }
                }
                return true;
            }

            bool operator!=(const Function& rhs) const {
                return !(*this == rhs);
            }
        };

    // Function 3-d class

    template <typename ResultT, typename ArgT1, typename ArgT2, typename ArgT3>
    class Function<ResultT, ArgT1, ArgT2, ArgT3> {
    private:
        std::function<ResultT(ArgT1, ArgT2, ArgT3)> _func;
        double _eps = 1e-10;

        void _check_initialized(const char* op) const {
            if (!_func) {
                throw std::runtime_error(std::string("Function not initialized for ") + op);
            }
        }

    public:
        // Constructors
        Function() : _func(nullptr) {}
        Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)> func) : _func(func) {}
        Function(const Function& other) = default;
        Function(Function&& other) = default;
        ~Function() = default;

        // Getters/Setters
        double eps() const noexcept { return _eps; }
        bool is_initialized() const noexcept { return static_cast<bool>(_func); }
        void set_eps(double new_eps) { _eps = new_eps; }

        // Copy/Move operators
        Function& operator=(const Function& other) = default;
        Function& operator=(Function&& other) = default;

        // Evaluation
        ResultT operator()(ArgT1 x, ArgT2 y, ArgT3 z) const {
            _check_initialized("function evaluation");
            return _func(x, y, z);
        }

        ResultT at(ArgT1 x, ArgT2 y, ArgT3 z) const { return (*this)(x, y, z); }

        // Частичное применение
        Function<ResultT, ArgT2, ArgT3> bind_first(ArgT1 x_val) const {
            _check_initialized("partial application");
            return Function<ResultT, ArgT2, ArgT3>(
                [*this, x_val](ArgT2 y, ArgT3 z) { return (*this)(x_val, y, z); }
            );
        }

        Function<ResultT, ArgT1, ArgT3> bind_second(ArgT2 y_val) const {
            _check_initialized("partial application");
            return Function<ResultT, ArgT1, ArgT3>(
                [*this, y_val](ArgT1 x, ArgT3 z) { return (*this)(x, y_val, z); }
            );
        }

        Function<ResultT, ArgT1, ArgT2> bind_third(ArgT3 z_val) const {
            _check_initialized("partial application");
            return Function<ResultT, ArgT1, ArgT2>(
                [*this, z_val](ArgT1 x, ArgT2 y) { return (*this)(x, y, z_val); }
            );
        }

        // Static factory methods
        static Function constant(ResultT value) {
            return Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)>(
                [value](ArgT1 x, ArgT2 y, ArgT3 z) { return value; })
            );
        }

        // Comparison operators
        bool operator==(const Function& rhs) const {
            if (!is_initialized() && !rhs.is_initialized()) return true;
            if (!is_initialized() || !rhs.is_initialized()) return false;
            
            // Simple test for 3D functions
            const ArgT1 test_x1 = -1, test_x2 = 1;
            const ArgT2 test_y1 = -1, test_y2 = 1;
            const ArgT3 test_z1 = -1, test_z2 = 1;
            const size_t test_n = 5;
            
            for (size_t i = 0; i <= test_n; i++) {
                for (size_t j = 0; j <= test_n; j++) {
                    for (size_t k = 0; k <= test_n; k++) {
                        ArgT1 x = test_x1 + i * (test_x2 - test_x1) / test_n;
                        ArgT2 y = test_y1 + j * (test_y2 - test_y1) / test_n;
                        ArgT3 z = test_z1 + k * (test_z2 - test_z1) / test_n;
                        if constexpr (std::is_floating_point_v<ResultT>) {
                            if (std::abs((*this)(x, y, z) - rhs(x, y, z)) > _eps) return false;
                        } else {
                            if ((*this)(x, y, z) != rhs(x, y, z)) return false;
                        }
                    }
                }
            }
            return true;
        }

        bool operator!=(const Function& rhs) const {
            return !(*this == rhs);
        }
    };

    // External methods

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator+(ResultT scalar, const Function<ResultT, ArgT>& func);

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator-(ResultT scalar, const Function<ResultT, ArgT>& func);

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator*(ResultT scalar, const Function<ResultT, ArgT>& func);

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator/(ResultT scalar, const Function<ResultT, ArgT>& func);

    // Allias for simplicity

    template<typename T>
    using Function1D = Function<T, T>;
    
    template<typename T>
    using Function2D = Function<T, T, T>;
    
    template<typename T>  
    using Function3D = Function<T, T, T, T>;
}


#include "detail/Function.ipp"
