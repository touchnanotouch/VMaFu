namespace vmafu {
    // Helper methods

    template <typename T>
    void IntEqFredholm<T>::_initialize_random() const {
        if (!_random_initialized) {
            std::random_device rd;
            _gen = std::mt19937(rd());
            _random_initialized = true;
        }
    }

    template <typename T>
    Function2D<T> IntEqFredholm<T>::_create_basis(BasisType type) const {
        switch(type) {
            case BasisType::Polynomial:
                return Function2D<T>([](size_t j, T x) -> T {
                    return std::pow(x, static_cast<T>(j));
                });
            case BasisType::Fourier:
                return Function2D<T>([](size_t j, T x) -> T {
                    if (j == 0) { return T{1}; }

                    size_t k = (j + 1) / 2

                    if (j % 2 == 1) {
                        return std::cos(static_cast<T>(k) * x);
                    } else {
                        return std::sin(static_cast<T>(k - 1) * x);
                    }
                });
            case BasisType::Chebyshev:
                return Function2D<T>([this](size_t j, T x) -> T {
                    T x_norm = x;

                    if (_a != -1.0 || _b != 1.0) {
                        x_norm = 2.0 * (x - _a) / (_b - _a) - 1.0;
                    }

                    if (j == 0) { return T{1}; }
                    if (j == 1) { return x_norm; }

                    T T_prev2 = T{1};
                    T T_prev1 = x_norm;
                    T T_curr;

                    for (size_t n = 2; n <= j; n++) {
                        T_curr = 2.0 * x_norm * T_prev1 - T_prev2;

                        T_prev2 = T_prev1;
                        T_prev1 = T_curr;
                    }

                    return T_prev1;
                });
            case BasisType::Legendre:
                return Function2D<T>([](size_t j, T x) -> T {
                    if (j == 0) { return T{1}; };
                    if (j == 1) { return x; };

                    T P_prev2 = T{1};
                    T P_prev1 = x;

                    T P_curr;

                    for (size_t n = 2; n <= j; n++) {
                        T n_t = static_cast<T>(n);

                        P_curr = ((2 * n_t - 1) * x * P_prev1 - (n_t - 1) * P_prev2) / n_t;

                        P_prev2 = P_prev1;
                        P_prev1 = P_curr;
                    }

                    return P_prev1;
                });
            default:
                throw std::invalid_argument("Unknown basis type");
        }
    }

    template <typename T>
    Vector<T> IntEqFredholm<T>::_create_collocation(
        CollocationType type, size_t n
    ) const {
        Vector<T> points(n);

        switch(type) {
            case CollocationType::Uniform: {
                if (n == 1) {
                    points[0] = (_a + _b) / 2;

                    break;
                }

                for (size_t i = 0; i < n; i++) {
                    points[i] = _a + (_b - _a) * static_cast<T>(i) / (n - 1);
                }

                break;
            }
            case CollocationType::Chebyshev: {
                if (n == 1) {
                    points[0] = (_a + _b) / 2.0;

                    break;
                }

                T center = (_a + _b) / 2.0;
                T radius = (_b - _a) / 2.0;

                T pi = std::acos(T{-1.0});

                for (size_t k = 0; k < n; k++) {
                    T cos_val = std::cos(pi * (2.0 * static_cast<T>(k) + 1.0) / (2.0 * static_cast<T>(n)));

                    points[k] = center + radius * cos_val;
                }

                std::sort(points.begin(), points.end());

                break;
            }
            // case CollocationType::Legendre:
            //     return _generate_legendre_points(config.n_points);
            case CollocationType::Random: {
                _initialize_random();

                std::uniform_real_distribution<T> dist(_a, _b);

                for (size_t i = 0; i < n; i++) {
                    points[i] = dist(_gen);
                }

                std::sort(points.begin(), points.end());

                break;
            }
            default:
                throw std::invalid_argument("Unknown collocation type");
        }

        return points;
    }

    // Certain solve methods

    template <typename T>
    Function1D<T> IntEqFredholm<T>::solve_collocation(
        const IntEqConfig<T>& config
    ) {
        Function2D<T> basis_func = _create_basis(config.basis);
        Vector<T> collocation = _create_collocation(config.collocation, config.n_collocation);

        Matrix<T> A(config.n_collocation, config.n_basis);
        Vector<T> b(config.n_collocation);

        for (size_t i = 0; i < config.n_collocation; i++) {
            T x_i = collocation[i];

            b[i] = _free_term(x_i);

            for (size_t j = 0; j < config.n_basis; j++) {
                // phi_j(x_i)

                T basis_val = basis_func(j, x_i);

                // lambda * \int K(x_i, t) * phi_j(t) dt

                Function1D<T> integrand([this, j, x_i, &basis_func](T t) -> T {
                    return _kernel(x_i, t) * basis_func(j, t);
                });

                T integral = integrand.definite_integral(_a, _b, config.n_integration);

                A(i, j) = basis_val - _lambda * integral;
            }
        }

        Matrix<T> ATA = A.transpose() * A;
        Vector<T> ATb = A.transpose() * b;

        Vector<T> coefficients = ATA.inverse() * ATb;

        return Function1D<T>([coefficients, basis_func, config](T x) -> T {
            T result = T{0};
            for (size_t i = 0; i < config.n_basis; i++) {
                result += coefficients[i] * basis_func(i, x);
            }

            return result;
        });
    }

    template <typename T>
    Function1D<T> IntEqFredholm<T>::solve_galerkin(
        const IntEqConfig<T>& config
    ) {
        // (phi_i, phi_j) - lambda * (K*phi_i, phi_j) = (f, phi_i)
        
        Function2D<T> basis_func = _create_basis(config.basis);
        
        Matrix<T> A(config.n_basis, config.n_basis);
        Vector<T> b(config.n_basis);

        for (size_t i = 0; i < config.n_basis; i++) {
            // (f, phi_i) = \int f(x) * phi_i(x) dx

            Function1D<T> rhs_integrand([this, i, &basis_func](T x) -> T {
                return _free_term(x) * basis_func(i, x);
            });

            b[i] = rhs_integrand.definite_integral(_a, _b, config.n_integration);
            
            for (size_t j = 0; j < config.n_basis; j++) {
                // (phi_i, phi_j) - lambda * (K*phi_i, phi_j)
                
                // 1. \int phi_i(x) * phi_j(x) dx

                Function1D<T> basis_integrand([i, j, &basis_func](T x) -> T {
                    return basis_func(i, x) * basis_func(j, x);
                });

                T basis_integral = basis_integrand.definite_integral(
                    _a, _b, config.n_integration
                );
                
                // 2. \int\int K(x, t) * phi_i(t) * phi_j(x) dt dx

                Function2D<T> kernel_integrand([this, i, j, &basis_func](T x, T t) -> T {
                    return _kernel(x, t) * basis_func(i, t) * basis_func(j, x);
                });

                T kernel_integral = kernel_integrand.definite_integral(
                    _a, _b,  // dx
                    _a, _b,  // dt
                    config.n_integration,  // n_integration_x
                    config.n_integration   // n_integration_y
                );

                A(i, j) = basis_integral - _lambda * kernel_integral;
            }
        }

        Vector<T> coefficients = A.inverse() * b;

        return Function1D<T>([coefficients, basis_func, config](T x) -> T {
            T result = T{0};
            for (size_t i = 0; i < config.n_basis; i++) {
                result += coefficients[i] * basis_func(i, x);
            }

            return result;
        });
    }

    // Constructor

    template <typename T>
    IntEqFredholm<T>::IntEqFredholm(
        Function2D<T> kernel,
        const Function1D<T>& free_term,
        T lambda,
        T a, T b
    ) : _kernel(kernel), _free_term(free_term), _lambda(lambda), _a(a), _b(b) {}

    // Solve method

    template <typename T>
    Function1D<T> IntEqFredholm<T>::solve(const IntEqConfig<T>& config) {
        switch(config.method) {
            case SolutionMethod::Collocation:
                return solve_collocation(config);
            case SolutionMethod::Galerkin:
                return solve_galerkin(config)
            default:
                throw std::invalid_argument("Unknown solution method");
        }
    }
}
