#pragma once


namespace vmafu {
    enum class IntegralEquationType {
        FredholmIntEq,
        Volterra
    };

    enum class SolutionMethod {
        Collocation,
        Galerkin
    };

    template<typename T>
    struct IntegralEquationConfig {
        IntegralEquationType type;
        SolutionMethod method;
        T lambda;
        T a, b;
        size_t n_basis;
        size_t n_points;
        
        IntegralEquationConfig(IntegralEquationType eq_type, 
                             SolutionMethod sol_method,
                             T lambda_val,
                             T left, T right,
                             size_t basis_size = 10,
                             size_t points_count = 20)
            : type(eq_type), method(sol_method), lambda(lambda_val),
              a(left), b(right), n_basis(basis_size), n_points(points_count) {}
    };
}
