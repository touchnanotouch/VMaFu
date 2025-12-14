namespace vmafu {
    template <typename T>
    void IntEqFredholm<T>::_initialize_random() const {
        if (!_random_initialized) {
            std::random_device rd;
            _gen = std::mt19937(rd());
            _random_initialized = true;
        }
    }

    template <typename T>
    void IntEqFredholm<T>::_check_chebyshev_domain(T x) const {
        if (x < -1.0 || x > 1.0) {
            throw std::runtime_error(
                "Chebyshev basis functions require x in [-1, 1]. "
            );
        }
    }

    template <typename T>
    Function2D<T> IntEqFredholm<T>::_create_basis(BasisType type) const {
        switch(type) {
            case BasisType::Polynomial:
                return Function2D<T>([](size_t j, T x) {
                    return std::pow(x, static_cast<T>(j));
                });
            case BasisType::Fourier:
                return Function2D<T>([](size_t j, T x) {
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
                    
                    _check_chebyshev_domain(x_norm);
                    
                    if (j == 0) return T{1};
                    if (j == 1) return x_norm;
                    
                    T T_prev2 = T{1};
                    T T_prev1 = x_norm;
                    T T_current;
                    
                    for (size_t n = 2; n <= j; n++) {
                        T_current = 2.0 * x_norm * T_prev1 - T_prev2;

                        T_prev2 = T_prev1;
                        T_prev1 = T_current;
                    }
                    
                    return T_prev1;
                });
            case BasisType::Legendre:
                return Function2D<T>([](size_t j, T x) {
                    if (j == 0) { return T{1}; };
                    if (j == 1) { return x; };
                    
                    T P_prev2 = T{1};
                    T P_prev1 = x;

                    T P_current;
                    
                    for (size_t n = 2; n <= j; n++) {
                        T n_t = static_cast<T>(n);

                        P_current = ((2 * n_t - 1) * x * P_prev1 - (n_t - 1) * P_prev2) / n_t;

                        P_prev2 = P_prev1;
                        P_prev1 = P_current;
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
                
                for (size_t i = 0; i < n; ++i) {
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

    template <typename T>
    IntEqFredholm<T>::IntEqFredholm(
        Function2D<T> kernel,
        const Function1D<T>& free_term,
        T lambda,
        T a, T b
    ) : _kernel(kernel), _free_term(free_term), _lambda(lambda), _a(a), _b(b) {}

    template <typename T>
    Function1D<T> IntEqFredholm<T>::solve(const IntEqConfig<T>& config) {
        switch(config.method) {
            case SolutionMethod::Collocation:
                return solve_collocation(config);
            // case SolutionMethod::Galerkin:
            //     return solve_galerkin(config)
            default:
                throw std::invalid_argument("Unknown solution method");
        }
    }

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
            
            for (size_t j = 0; j < config.n_basis; ++j) {
                // φ_j(x_i)

                T basis_val = basis_func(j, x_i);
                
                // lambda * \int K(x_i, t) * φ_j(t) dt

                T integral = _compute_collocation_integral(
                    j, x_i, basis_func, config.n_integration
                );
                
                A(i, j) = basis_val - _lambda * integral;
            }
        }
        
        // Решаем методом наименьших квадратов
        Matrix<T> ATA = A.transpose() * A;
        Vector<T> ATb = A.transpose() * b;
        Vector<T> coefficients = ATA.inverse() * ATb;
        
        // Строем решение как линейную комбинацию базисных функций
        return Function1D<T>([coefficients, basis_func, config](T x) -> T {
            T result = T{0};
            for (size_t i = 0; i < config.n_basis; ++i) {
                result += coefficients[i] * basis_func(i, x);
            }
            return result;
        });
    }   
}
