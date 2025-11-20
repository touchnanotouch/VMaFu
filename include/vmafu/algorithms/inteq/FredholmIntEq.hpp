#pragma once


#include <cmath>
#include <functional>
#include <stdexcept>

#include "../../core/Vector.hpp"
#include "../../core/Matrix.hpp"
#include "../../core/Function.hpp"

#include "types.hpp"


// Main class

namespace vmafu {
    template<typename T>
    class FredholmIntEq {
        private:
            std::function<T(T, T)> kernel_;

            Function<T> free_term_;

            T lambda_;
            T a_, b_;
            
            // Helper methods

            Vector<Function<T>> create_basis_functions(size_t n_basis) const;

            T compute_integral(const Function<T>& basis_func, T x, size_t n_segments = 100) const;
            T compute_integral_solution(const Function<T>& solution, T x, size_t n_segments = 100) const;

            T compute_galerkin_rhs(const Function<T>& basis_func, size_t n_segments) const;

            T compute_basis_integral(
                const Function<T>& basis_i,
                const Function<T>& basis_j, 
                size_t n_segments
            ) const;

            T compute_galerkin_kernel_integral(
                const Function<T>& basis_i,
                const Function<T>& basis_j,
                size_t n_segments
            ) const;
        public:
            Vector<T> create_collocation_points(size_t n_points) const;

            // Constuctor

            FredholmIntEq(std::function<T(T, T)> kernel,
                        const Function<T>& free_term,
                        T lambda,
                        T a, T b);
            
            // Solve methods
            
            Function<T> solve(const IntegralEquationConfig<T>& config);
            Function<T> solve_collocation(size_t n_basis, size_t n_points);
            Function<T> solve_galerkin(size_t n_basis, size_t n_integration_segments = 100);

            // Solution analysis methods

            T calculate_residual(const Function<T>& solution, size_t n_points = 1000) const;
    };
}


#include "detail/FredholmIntEq.ipp"
