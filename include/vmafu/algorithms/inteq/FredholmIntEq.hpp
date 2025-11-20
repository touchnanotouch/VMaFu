#pragma once


#include <cmath>
#include <stdexcept>

#include "../../core/Vector.hpp"
#include "../../core/Matrix.hpp"
#include "../../core/Function.hpp"

#include "types.hpp"


namespace vmafu {
    // Main class

    template<typename T>
    class FredholmIntEq {
        private:
            Function2D<T> kernel_;

            Function1D<T> free_term_;

            T lambda_;
            T a_, b_;
            
            // Helper methods

            Vector<Function1D<T>> create_basis_functions(size_t n_basis) const;

            T compute_integral(const Function1D<T>& basis_func, T x, size_t n_segments = 100) const;
            T compute_integral_solution(const Function1D<T>& solution, T x, size_t n_segments = 100) const;

            T compute_galerkin_rhs(const Function1D<T>& basis_func, size_t n_segments) const;

            T compute_basis_integral(
                const Function1D<T>& basis_i,
                const Function1D<T>& basis_j, 
                size_t n_segments
            ) const;

            T compute_galerkin_kernel_integral(
                const Function1D<T>& basis_i,
                const Function1D<T>& basis_j,
                size_t n_segments
            ) const;
        public:
            Vector<T> create_collocation_points(size_t n_points) const;

            // Constuctor

            FredholmIntEq(Function2D<T> kernel,
                        const Function1D<T>& free_term,
                        T lambda,
                        T a, T b);
            
            // Solve methods
            
            Function1D<T> solve(const IntegralEquationConfig<T>& config);
            Function1D<T> solve_collocation(size_t n_basis, size_t n_points);
            Function1D<T> solve_galerkin(size_t n_basis, size_t n_integration_segments = 100);

            // Solution analysis methods

            T calculate_residual(const Function1D<T>& solution, size_t n_points = 1000) const;
    };
}


#include "detail/FredholmIntEq.ipp"
