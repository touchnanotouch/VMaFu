#pragma once


#include <algorithm>
#include <cmath>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <initializer_list>


// Main class

namespace vmafu {
    template <typename T>
    class Function {
        private:
            std::function<T(T)> _func;

            double _eps = 1e-10;

            // Checks

            void _check_initialized(const char* op) const;

        public:
            // Constructors/Destructor

            Function();
            Function(std::function<T(T)> func);
            Function(const Function& other);
            Function(Function&& other);

            ~Function() = default;

            // Getters

            double eps() const noexcept;
            bool is_initialized() const noexcept;

            // Setters

            void set_eps(double new_eps);

            // Copy/Move operators

            Function& operator=(const Function& other);
            Function& operator=(Function&& other);

            // Access operators

            T operator()(T x) const;
            T at(T x) const;

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

            Function operator+(T scalar) const;
            Function operator-(T scalar) const;
            Function operator*(T scalar) const;
            Function operator/(T scalar) const;

            Function& operator+=(T scalar);
            Function& operator-=(T scalar);
            Function& operator*=(T scalar);
            Function& operator/=(T scalar);

            // Function composition

            Function compose(const Function& inner) const;

            // Calculus operators

            Function derivative(T h = 1e-6) const;
            Function integral(T a, T b, size_t n = 1000) const;
            T definite_integral(T a, T b, size_t n = 1000) const;

            // Functional properties

            T norm(T a, T b, size_t n = 1000) const;  // L2 norm on [a,b]
            T maximum(T a, T b, size_t n = 1000) const;
            T minimum(T a, T b, size_t n = 1000) const;
            T root(T a, T b, size_t n = 100) const;

            // Functional analysis

            bool is_continuous(T a, T b, size_t n = 1000) const;
            bool is_monotonic(T a, T b, size_t n = 1000) const;
            bool has_root(T a, T b, size_t n = 100) const;

            // Comparison operators

            bool operator==(const Function& rhs) const;
            bool operator!=(const Function& rhs) const;

            // Static methods

            static Function constant(T value);
            static Function linear(T intercept, T slope);
            static Function polynomial(const Vector<T>& coefficients);
            static Function exponential(T base = static_cast<T>(M_E));
            static Function logarithmic(T base = static_cast<T>(M_E));
            static Function trigonometric_sin();
            static Function trigonometric_cos();
            static Function trigonometric_tan();

            static Function from_function_ptr(T (*func)(T));

            // Friend methods

            template <typename U>
            friend std::ostream& operator<<(std::ostream& os, const Function<U>& f);
    };

    // External methods

    template <typename T>
    Function<T> operator+(T scalar, const Function<T>& func);

    template <typename T>
    Function<T> operator-(T scalar, const Function<T>& func);

    template <typename T>
    Function<T> operator*(T scalar, const Function<T>& func);

    template <typename T>
    Function<T> operator/(T scalar, const Function<T>& func);
}


#include "detail/Function.ipp"
