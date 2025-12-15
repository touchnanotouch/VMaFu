#pragma once


#include <cmath>
#include <stdexcept>
#include <random>

#include "../../core/Vector.hpp"
#include "../../core/Matrix.hpp"
#include "../../core/Function.hpp"

#include "types.hpp"


namespace vmafu {
    // Main class

    template <typename T>
    class IntEqFredholm {
        private:
            Function2D<T> _kernel;
            Function1D<T> _free_term;

            T _lambda;
            T _a, _b;

            mutable std::mt19937 _gen;
            mutable bool _random_initialized = false;

            // Helper methods

            void _initialize_random() const;

            Function2D<T> _create_basis(BasisType type) const;
            Vector<T> _create_collocation(
                CollocationType type, size_t n
            ) const;

            // Certain solve methods

            Function1D<T> solve_collocation(const IntEqConfig<T>& config);
            Function1D<T> solve_galerkin(const IntEqConfig<T>& config);

        public:

            // Constuctor

            IntEqFredholm(
                Function2D<T> kernel,
                const Function1D<T>& free_term,
                T lambda,
                T a, T b
            );

            // Solve method

            Function1D<T> solve(const IntEqConfig<T>& config);
    };
}


#include "detail/IntEqFredholm.ipp"
