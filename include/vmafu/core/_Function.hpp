// core/_Function.hpp


#pragma once


#include <algorithm>
#include <cmath>
#include <tuple>
#include <functional>
#include <ostream>
#include <stdexcept>

#include "_Vector.hpp"


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

            template <typename OuterResultT>
            Function<OuterResultT, ArgT> compose(const Function<OuterResultT, ResultT>& outer) const;

            template <typename CurveArgT>
            Function<ResultT, CurveArgT> compose_with_curve(
                const Function<ArgT, CurveArgT>& x_curve
            ) const;

            // Calculus operators

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT>>
            derivative(ArgT h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            definite_integral(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT>>
            integral(ArgT c = 0, size_t n = 1000) const;

            // Functional properties

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            norm(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            maximum(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            minimum(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            root(ArgT a, ArgT b, size_t n = 100) const;

            // Functional analysis

            std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
            is_continuous(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
            is_monotonic(ArgT a, ArgT b, size_t n = 1000) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
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

            // Checks

            void _check_initialized(const char* op) const;

        public:
            // Constructors/Destructor

            Function();
            Function(std::function<ResultT(ArgT1, ArgT2)> func);
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

            ResultT operator()(ArgT1 x, ArgT2 y) const;
            ResultT at(ArgT1 x, ArgT2 y) const;

            // Function currying

            Function<ResultT, ArgT2> bind_first(ArgT1 x_val) const;
            Function<ResultT, ArgT1> bind_second(ArgT2 y_val) const;

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

            template <typename OuterResultT>
            Function<OuterResultT, ArgT1, ArgT2> compose(
                const Function<OuterResultT, ResultT>& outer
            ) const;

            template <typename CurveArgT>
            Function<ResultT, CurveArgT> compose_with_curve(
                const Function<ArgT1, CurveArgT>& x_curve,
                const Function<ArgT2, CurveArgT>& y_curve
            ) const;

            // Calculus operators

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2>>
            partial_derivative_x(ArgT1 h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2>>
            partial_derivative_y(ArgT2 h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            definite_integral(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2,
                size_t n1 = 100, size_t n2 = 100
            ) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, 
                std::pair<Function<ResultT, ArgT1, ArgT2>, Function<ResultT, ArgT1, ArgT2>>
            >
            gradient(ArgT1 hx = 1e-6, ArgT2 hy = 1e-6) const;

            // Functional properties

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            norm(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1 = 100, size_t n2 = 100) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            maximum(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1 = 100, size_t n2 = 100) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            minimum(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1 = 100, size_t n2 = 100) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, std::pair<ArgT1, ArgT2>>
            root(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n = 100) const;

            // Functional analysis

            std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
            is_continuous(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1 = 100, size_t n2 = 100) const;

            // Comparison operators

            bool operator==(const Function& rhs) const;
            bool operator!=(const Function& rhs) const;

            // Static methods

            static Function constant(ResultT value);
            static Function bilinear(ResultT a, ResultT b, ResultT c, ResultT d);
            static Function gaussian(ResultT sigma_x = 1.0, ResultT sigma_y = 1.0);

            static Function from_function_ptr(ResultT (*func)(ArgT1, ArgT2));

            // Friend methods

            friend std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT1, ArgT2>& f);
        };

    // Function 3-d class

    template <typename ResultT, typename ArgT1, typename ArgT2, typename ArgT3>
    class Function<ResultT, ArgT1, ArgT2, ArgT3> {
        private:
            std::function<ResultT(ArgT1, ArgT2, ArgT3)> _func;
            
            double _eps = 1e-10;

            // Checks

            void _check_initialized(const char* op) const;

        public:

            // Constructors/Destructor

            Function();
            Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)> func);
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

            ResultT operator()(ArgT1 x, ArgT2 y, ArgT3 z) const;
            ResultT at(ArgT1 x, ArgT2 y, ArgT3 z) const;

            // Function currying

            Function<ResultT, ArgT2, ArgT3> bind_first(ArgT1 x_val) const;
            Function<ResultT, ArgT1, ArgT3> bind_second(ArgT2 y_val) const;
            Function<ResultT, ArgT1, ArgT2> bind_third(ArgT3 z_val) const;

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

            template <typename OuterResultT>
            Function<OuterResultT, ArgT1, ArgT2, ArgT3> compose(
                const Function<OuterResultT, ResultT>& outer
            ) const;

            template <typename CurveArgT>
            Function<ResultT, CurveArgT> compose_with_curve(
                const Function<ArgT1, CurveArgT>& x_curve,
                const Function<ArgT2, CurveArgT>& y_curve,
                const Function<ArgT3, CurveArgT>& z_curve
            ) const;

            // Calculus operators  

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
            partial_derivative_x(ArgT1 h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
            partial_derivative_y(ArgT2 h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
            partial_derivative_z(ArgT3 h = 1e-6) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            definite_integral(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2, 
                ArgT3 a3, ArgT3 b3,
                size_t n1, size_t n2, size_t n3
            ) const;

            std::enable_if_t<
                std::is_floating_point_v<ResultT>, 
                std::tuple<
                    Function<ResultT, ArgT1, ArgT2, ArgT3>, 
                    Function<ResultT, ArgT1, ArgT2, ArgT3>,
                    Function<ResultT, ArgT1, ArgT2, ArgT3>
                >
            >
            gradient(ArgT1 hx, ArgT2 hy, ArgT3 hz) const;

            // Functional properties

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            norm(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2,
                ArgT3 a3, ArgT3 b3, 
                size_t n1, size_t n2, size_t n3
            ) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            maximum(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2,
                ArgT3 a3, ArgT3 b3,
                size_t n1, size_t n2, size_t n3
            ) const;

            std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
            minimum(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2,
                ArgT3 a3, ArgT3 b3,
                size_t n1, size_t n2, size_t n3
            ) const;

            // Functional analysis

            std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
            is_continuous(
                ArgT1 a1, ArgT1 b1,
                ArgT2 a2, ArgT2 b2,
                ArgT3 a3, ArgT3 b3,
                size_t n1, size_t n2, size_t n3
            ) const;

            // Static methods

            static Function constant(ResultT value);
            static Function trilinear(
                ResultT a, ResultT b, ResultT c, ResultT d, 
                ResultT e, ResultT f, ResultT g, ResultT h
            );
            static Function gaussian(ResultT sigma_x = 1.0, ResultT sigma_y = 1.0, ResultT sigma_z = 1.0);

            static Function from_function_ptr(ResultT (*func)(ArgT1, ArgT2, ArgT3));

            // Friend methods

            friend std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT1, ArgT2, ArgT3>& f);
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

    // Aliases for simplicity

    template <typename T>
    using Function1D = Function<T, T>;
    
    template <typename T>
    using Function2D = Function<T, T, T>;
    
    template <typename T>  
    using Function3D = Function<T, T, T, T>;
}


#include "detail/_Function.ipp"
