#pragma once


#include "../../core/Vector.hpp"


namespace vmafu {
    enum class SolutionMethod {
        Collocation,
        Galerkin
        // MonteCarlo,
        // Nystrom
    };

    enum class BasisType {
        Polynomial,      // φ_j(x) = x^j
        Fourier,
        Chebyshev,       // Chebyshev 1-kind
        Legendre
    };

    enum class CollocationType {
        Uniform,
        Chebyshev,
        // Legendre,
        Random
    };

    template <typename T>
    struct IntEqConfig {
        SolutionMethod method;
        BasisType basis;
        CollocationType collocation;

        size_t n_basis;
        size_t n_collocation;
        size_t n_integration;
        
        IntEqConfig(
            SolutionMethod solution_method,
            BasisType basis_type = BasisType::Polynomial,
            CollocationType collocation_type = CollocationType::Uniform,
            size_t basis_size = 5,
            size_t collocation_size = 10,
            size_t integration_size = 100
        ) : method(solution_method), basis(basis_type),
            collocation(collocation_type), n_basis(basis_size),
            n_collocation(collocation_size), n_integration(integration_size)
        {
            if (method == SolutionMethod::Collocation && n_basis > n_collocation) {
                throw std::invalid_argument("For collocation method, n_basis must be <= n_collocation");
            }
        }
    };
}
