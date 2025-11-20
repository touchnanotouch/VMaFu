namespace vmafu {
    template<typename T>
    T FredholmIntEq<T>::compute_galerkin_rhs(const Function<T>& basis_func, size_t n_segments) const {
        T integral = T{0};
        T h = (b_ - a_) / n_segments;
        
        for(size_t k = 0; k <= n_segments; ++k) {
            T x = a_ + k * h;
            T weight = (k == 0 || k == n_segments) ? h/3 : 
                    (k % 2 == 1) ? 4*h/3 : 2*h/3;
            integral += weight * free_term_(x) * basis_func(x);
        }
        
        return integral;
    }

    template<typename T>
    T FredholmIntEq<T>::compute_basis_integral(const Function<T>& basis_i, const Function<T>& basis_j, 
                                            size_t n_segments) const {
        T integral = T{0};
        T h = (b_ - a_) / n_segments;
        
        for(size_t k = 0; k <= n_segments; ++k) {
            T x = a_ + k * h;
            T weight = (k == 0 || k == n_segments) ? h/3 : 
                    (k % 2 == 1) ? 4*h/3 : 2*h/3;
            integral += weight * basis_i(x) * basis_j(x);
        }
        
        return integral;
    }

    template<typename T>
    T FredholmIntEq<T>::compute_galerkin_kernel_integral(const Function<T>& basis_i, const Function<T>& basis_j,
                                                        size_t n_segments) const {
        T integral = T{0};
        T h = (b_ - a_) / n_segments;
        
        // Внешний интеграл по x
        for(size_t i = 0; i <= n_segments; ++i) {
            T x = a_ + i * h;
            T weight_x = (i == 0 || i == n_segments) ? h/3 : 
                        (i % 2 == 1) ? 4*h/3 : 2*h/3;
            
            // Внутренний интеграл по t
            T inner_integral = T{0};
            for(size_t j = 0; j <= n_segments; ++j) {
                T t = a_ + j * h;
                T weight_t = (j == 0 || j == n_segments) ? h/3 : 
                            (j % 2 == 1) ? 4*h/3 : 2*h/3;
                
                inner_integral += weight_t * kernel_(x, t) * basis_j(t);
            }
            
            integral += weight_x * inner_integral * basis_i(x);
        }
        
        return integral;
    }

    template<typename T>
    FredholmIntEq<T>::FredholmIntEq(std::function<T(T, T)> kernel,
                                     const Function<T>& free_term,
                                     T lambda,
                                     T a, T b)
        : kernel_(kernel), free_term_(free_term), lambda_(lambda), a_(a), b_(b) {}

    template<typename T>
    Function<T> FredholmIntEq<T>::solve(const IntegralEquationConfig<T>& config) {
        switch(config.method) {
            case SolutionMethod::Collocation:
                return solve_collocation(config.n_basis, config.n_points);
            case SolutionMethod::Galerkin:
                return solve_galerkin(config.n_basis, config.n_points);
            default:
                throw std::invalid_argument("Unknown solution method");
        }
    }

    template<typename T>
    Function<T> FredholmIntEq<T>::solve_collocation(size_t n_basis, size_t n_points) {
        auto basis_funcs = create_basis_functions(n_basis);
        auto points = create_collocation_points(n_points);
        
        Matrix<T> A(n_points, n_basis);
        Vector<T> b(n_points);
        
        // Построение матрицы системы
        for(size_t i = 0; i < n_points; ++i) {
            T x_i = points[i];
            b[i] = free_term_(x_i);
            
            for(size_t j = 0; j < n_basis; ++j) {
                T integral = compute_integral(basis_funcs[j], x_i);
                A(i, j) = basis_funcs[j](x_i) - lambda_ * integral;
            }
        }
        
        // Решение системы методом наименьших квадратов
        Matrix<T> ATA = A.transpose() * A;
        Vector<T> ATb = A.transpose() * b;
        Vector<T> coefficients = ATA.inverse() * ATb;
        
        // Построение решения как линейной комбинации базисных функций
        return Function<T>([coefficients, basis_funcs](T x) -> T {
            T result = T{0};
            for(size_t i = 0; i < coefficients.size(); ++i) {
                result += coefficients[i] * basis_funcs[i](x);
            }
            return result;
        });
    }

    template<typename T>
    Function<T> FredholmIntEq<T>::solve_galerkin(size_t n_basis, size_t n_integration_segments) {
        auto basis_funcs = create_basis_functions(n_basis);
        
        Matrix<T> A(n_basis, n_basis);
        Vector<T> b(n_basis);
        
        // Построение матрицы системы и правой части
        for(size_t i = 0; i < n_basis; ++i) {
            // Вычисление правой части: (f, φ_i)
            b[i] = compute_galerkin_rhs(basis_funcs[i], n_integration_segments);
            
            for(size_t j = 0; j < n_basis; ++j) {
                // Вычисление элементов матрицы: (φ_j, φ_i) - λ * (Kφ_j, φ_i)
                T basis_integral = compute_basis_integral(basis_funcs[i], basis_funcs[j], n_integration_segments);
                T kernel_integral = compute_galerkin_kernel_integral(basis_funcs[i], basis_funcs[j], n_integration_segments);
                
                A(i, j) = basis_integral - lambda_ * kernel_integral;
            }
        }
        
        // Решение системы линейных уравнений
        Vector<T> coefficients = A.inverse() * b;
        
        // Построение решения как линейной комбинации базисных функций
        return Function<T>([coefficients, basis_funcs](T x) -> T {
            T result = T{0};
            for(size_t i = 0; i < coefficients.size(); ++i) {
                result += coefficients[i] * basis_funcs[i](x);
            }
            return result;
        });
    }

    template<typename T>
    T FredholmIntEq<T>::calculate_residual(const Function<T>& solution, size_t n_points) const {
        T max_residual = T{0};
        
        for(size_t i = 0; i < n_points; ++i) {
            T x = a_ + (b_ - a_) * static_cast<T>(i) / (n_points - 1);
            T integral = compute_integral_solution(solution, x);
            T residual_val = std::abs(solution(x) - free_term_(x) - lambda_ * integral);
            
            if(residual_val > max_residual) {
                max_residual = residual_val;
            }
        }
        
        return max_residual;
    }

    template<typename T>
    Vector<Function<T>> FredholmIntEq<T>::create_basis_functions(size_t n_basis) const {
        Vector<Function<T>> basis(n_basis);
        for(size_t i = 0; i < n_basis; ++i) {
            basis[i] = Function<T>([i](T x) -> T {
                return std::pow(x, static_cast<T>(i));
            });
        }
        return basis;
    }

    template<typename T>
    Vector<T> FredholmIntEq<T>::create_collocation_points(size_t n_points) const {
        Vector<T> points(n_points);
        for(size_t i = 0; i < n_points; ++i) {
            points[i] = a_ + (b_ - a_) * static_cast<T>(i) / (n_points - 1);
        }
        return points;
    }

    template<typename T>
    T FredholmIntEq<T>::compute_integral(const Function<T>& basis_func, T x, size_t n_segments) const {
        T integral = T{0};
        T h = (b_ - a_) / n_segments;
        
        for(size_t k = 0; k <= n_segments; ++k) {
            T t = a_ + k * h;
            T weight = (k == 0 || k == n_segments) ? h/3 : 
                      (k % 2 == 1) ? 4*h/3 : 2*h/3;
            integral += weight * kernel_(x, t) * basis_func(t);
        }
        
        return integral;
    }

    template<typename T>
    T FredholmIntEq<T>::compute_integral_solution(const Function<T>& solution, T x, size_t n_segments) const {
        T integral = T{0};
        T h = (b_ - a_) / n_segments;
        
        for(size_t k = 0; k <= n_segments; ++k) {
            T t = a_ + k * h;
            T weight = (k == 0 || k == n_segments) ? h/3 : 
                      (k % 2 == 1) ? 4*h/3 : 2*h/3;
            integral += weight * kernel_(x, t) * solution(t);
        }
        
        return integral;
    }
}
