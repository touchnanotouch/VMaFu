namespace vmafu {
    // Macros

    #define FUNCTION_TEMPLATE_1D template <typename ResultT, typename ArgT>
    #define FUNCTION_TEMPLATE_2D template <typename ResultT, typename ArgT1, typename ArgT2>
    #define FUNCTION_TEMPLATE_3D template <typename ResultT, typename ArgT1, typename ArgT2, typename ArgT3>

    #define FUNCTION_TYPE_1D Function<ResultT, ArgT>
    #define FUNCTION_TYPE_2D Function<ResultT, ArgT1, ArgT2>  
    #define FUNCTION_TYPE_3D Function<ResultT, ArgT1, ArgT2, ArgT3>

    // Function 1-d class

    // Checks

    FUNCTION_TEMPLATE_1D
    void FUNCTION_TYPE_1D::_check_initialized(const char* op) const {
        if (!_func) {
            throw std::runtime_error(std::string("Function not initialized for ") + op);
        }
    }

    // Constructors/Destructor

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D::Function() : _func(nullptr) {}

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D::Function(std::function<ResultT(ArgT)> func) : _func(func) {}

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D::Function(const Function& other) = default;

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D::Function(Function&& other) = default;

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D::~Function() = default;

    // Getters

    FUNCTION_TEMPLATE_1D
    double FUNCTION_TYPE_1D::eps() const noexcept { return _eps; }

    FUNCTION_TEMPLATE_1D
    bool FUNCTION_TYPE_1D::is_initialized() const noexcept { return static_cast<bool>(_func); }

    // Setters

    FUNCTION_TEMPLATE_1D
    void FUNCTION_TYPE_1D::set_eps(double new_eps) { _eps = new_eps; }

    // Copy/Move operators

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator=(const Function& other) = default;

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator=(Function&& other) = default;

    // Access operators

    FUNCTION_TEMPLATE_1D
    ResultT FUNCTION_TYPE_1D::operator()(ArgT x) const {
        _check_initialized("function evaluation");

        return _func(x);
    }

    FUNCTION_TEMPLATE_1D
    ResultT FUNCTION_TYPE_1D::at(ArgT x) const {
        return (*this)(x);
    }

    // Arithmetic operators with functions

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator+(const Function& rhs) const {
        _check_initialized("addition");
        rhs._check_initialized("addition");
        
        return Function([this, rhs](ArgT x) -> ResultT{ 
            return (*this)(x) + rhs(x); 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator-(const Function& rhs) const {
        _check_initialized("subtraction");
        rhs._check_initialized("subtraction");
        
        return Function([this, rhs](ArgT x) -> ResultT { 
            return (*this)(x) - rhs(x); 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator*(const Function& rhs) const {
        _check_initialized("multiplication");
        rhs._check_initialized("multiplication");
        
        return Function([this, rhs](ArgT x) -> ResultT { 
            return (*this)(x) * rhs(x); 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator/(const Function& rhs) const {
        _check_initialized("division");
        rhs._check_initialized("division");
        
        return Function([this, rhs](ArgT x) -> ResultT { 
            ResultT denominator = rhs(x);

            if constexpr (std::is_floating_point_v<ResultT>) {
                if (std::abs(denominator) < _eps) {
                    throw std::invalid_argument("Division by zero in function");
                }
            } else {
                if (denominator == ResultT{0}) {
                    throw std::invalid_argument("Division by zero in function");
                }
            }

            return (*this)(x) / denominator; 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator+=(const Function& rhs) {
        *this = *this + rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator-=(const Function& rhs) {
        *this = *this - rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator*=(const Function& rhs) {
        *this = *this * rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator/=(const Function& rhs) {
        *this = *this / rhs;

        return *this;
    }

    // Arithmetic operators with scalars

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator+(ResultT scalar) const {
        _check_initialized("scalar addition");

        return Function([this, scalar](ArgT x) -> ResultT { 
            return (*this)(x) + scalar; 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator-(ResultT scalar) const {
        _check_initialized("scalar subtraction");

        return Function([this, scalar](ArgT x) -> ResultT { 
            return (*this)(x) - scalar; 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator*(ResultT scalar) const {
        _check_initialized("scalar multiplication");

        return Function([this, scalar](ArgT x) -> ResultT { 
            return (*this)(x) * scalar; 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator/(ResultT scalar) const {
        _check_initialized("scalar division");

        if constexpr (std::is_floating_point_v<ResultT>) {
            if (std::abs(scalar) < _eps) {
                throw std::invalid_argument("Division by zero");
            }
        } else {
            if (scalar == ResultT{0}) {
                throw std::invalid_argument("Division by zero");
            }
        }

        return Function([this, scalar](ArgT x) -> ResultT { 
            return (*this)(x) / scalar; 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator+=(ResultT scalar) {
        *this = *this + scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator-=(ResultT scalar) {
        *this = *this - scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator*=(ResultT scalar) {
        *this = *this * scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D& FUNCTION_TYPE_1D::operator/=(ResultT scalar) {
        *this = *this / scalar;

        return *this;
    }

    // Function composition

    FUNCTION_TEMPLATE_1D
    template <typename OuterResultT>
    Function<OuterResultT, ArgT> FUNCTION_TYPE_1D::compose(
        const Function<OuterResultT, ResultT>& outer
    ) const {
        _check_initialized("composition");
        
        return Function<OuterResultT, ArgT>([this, outer](ArgT x) -> ResultT { 
            return outer((*this)(x));
        });
    }

    FUNCTION_TEMPLATE_1D
    template <typename CurveArgT>
    Function<ResultT, CurveArgT> FUNCTION_TYPE_1D::compose_with_curve(
        const Function<ArgT, CurveArgT>& x_curve
    ) const {
        return Function<ResultT, CurveArgT>([this, x_curve](CurveArgT t) -> ResultT {
            return (*this)(x_curve(t));
        });
    }

    // Calculus operators

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, FUNCTION_TYPE_1D>
    FUNCTION_TYPE_1D::derivative(ArgT h) const {
        _check_initialized("derivative");

        return Function([this, h](ArgT x) -> ResultT { 
            return ((*this)(x + h) - (*this)(x - h)) / (2 * h); 
        });
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_1D::definite_integral(ArgT a, ArgT b, size_t n) const {
        _check_initialized("integration");

        if (n == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }

        if (n % 2 != 0) { n++; }
        
        ArgT h = (b - a) / n;
        
        ResultT sum = (*this)(a) + (*this)(b);
        
        for (size_t i = 1; i < n; i++) {
            double coefficient = (i % 2 == 0) ? 2.0 : 4.0;

            sum += coefficient * (*this)(a + i * h);
        }
        
        return sum * h / 3.0;
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, FUNCTION_TYPE_1D>
    FUNCTION_TYPE_1D::integral(ArgT c, size_t n) const {
        _check_initialized("integration");

        if (n % 2 != 0) { n++; }
        
        return Function([this, c, n](ArgT x) -> ResultT {
            if (x == 0) {
                return c;
            }

            ArgT a = 0;
            ArgT b = x;
            ArgT h = (b - a) / n;
            
            ResultT sum = (*this)(a) + (*this)(b);
            
            for (size_t i = 1; i < n; i++) {
                double coefficient = (i % 2 == 0) ? 2.0 : 4.0;

                sum += coefficient * (*this)(a + i * h);
            }
            
            return sum * h / 3.0 + c;
        });
    }

    // Functional properties

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_1D::norm(ArgT a, ArgT b, size_t n) const {
        _check_initialized("norm calculation");

        ArgT h = (b - a) / n;

        ResultT sum = ((*this)(a) * (*this)(a) + (*this)(b) * (*this)(b)) / 2;

        for (size_t i = 1; i < n; i++) {
            ArgT x = a + i * h;
            ResultT fx = (*this)(x);

            sum += fx * fx;
        }

        return std::sqrt(sum * h);
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_1D::maximum(ArgT a, ArgT b, size_t n) const {
        _check_initialized("maximum finding");

        ArgT h = (b - a) / n;

        ResultT max_val = (*this)(a);

        for (size_t i = 1; i <= n; i++) {
            ArgT x = a + i * h;

            ResultT val = (*this)(x);
            if (val > max_val) {
                max_val = val;
            }
        }

        return max_val;
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_1D::minimum(ArgT a, ArgT b, size_t n) const {
        _check_initialized("minimum finding");

        ArgT h = (b - a) / n;

        ResultT min_val = (*this)(a);

        for (size_t i = 1; i <= n; i++) {
            ArgT x = a + i * h;

            ResultT val = (*this)(x);
            if (val < min_val) {
                min_val = val;
            }
        }

        return min_val;
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_1D::root(ArgT a, ArgT b, size_t n) const {
        _check_initialized("root finding");

        if (!has_root(a, b)) {
            throw std::invalid_argument("Function has no root in the given interval");
        }

        ArgT left = a, right = b;

        for (size_t i = 0; i < n; i++) {
            ArgT mid = (left + right) / 2;
    
            ResultT f_mid = (*this)(mid);
            if (std::abs(f_mid) < _eps) {
                return mid;
            }

            if ((*this)(left) * f_mid < 0) {
                right = mid;
            } else {
                left = mid;
            }
        }

        return (left + right) / 2;
    }

    // Functional analysis

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
    FUNCTION_TYPE_1D::is_continuous(ArgT a, ArgT b, size_t n) const {
        _check_initialized("continuity check");

        ArgT h = (b - a) / n;

        for (size_t i = 0; i < n; i++) {
            ArgT x1 = a + i * h;
            ArgT x2 = a + (i + 1) * h;
    
            ResultT diff = std::abs((*this)(x2) - (*this)(x1));
            if (diff > 1.0 / h) {
                return false;
            }
        }

        return true;
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
    FUNCTION_TYPE_1D::is_monotonic(ArgT a, ArgT b, size_t n) const {
        _check_initialized("monotonicity check");

        ArgT h = (b - a) / n;

        bool increasing = true, decreasing = true;

        for (size_t i = 1; i < n; i++) {
            ArgT x1 = a + (i - 1) * h;
            ArgT x2 = a + i * h;

            ResultT diff = (*this)(x2) - (*this)(x1);

            if (diff < -_eps) { increasing = false; }
            if (diff > _eps) { decreasing = false; }

            if (!increasing && !decreasing) {
                return false;
            }
        }

        return true;
    }

    FUNCTION_TEMPLATE_1D
    std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
    FUNCTION_TYPE_1D::has_root(ArgT a, ArgT b, size_t n) const {
        _check_initialized("root existence check");

        ResultT fa = (*this)(a);
        ResultT fb = (*this)(b);

        if (fa * fb <= 0) {
            return true;
        }

        ArgT h = (b - a) / n;

        for (size_t i = 1; i < n; i++) {
            ArgT x = a + i * h;

            ResultT fx = (*this)(x);
            if (std::abs(fx) < _eps || fa * fx <= 0) {
                return true;
            }
        }

        return false;
    }

    // Comparison operators

    FUNCTION_TEMPLATE_1D
    bool FUNCTION_TYPE_1D::operator==(const Function& rhs) const {
        if (!is_initialized() && !rhs.is_initialized()) { return true; }
        if (!is_initialized() || !rhs.is_initialized()) { return false; }
        
        const ArgT test_a = -10, test_b = 10;
        const size_t test_n = 100;

        ArgT h = (test_b - test_a) / test_n;

        for (size_t i = 0; i <= test_n; i++) {
            ArgT x = test_a + i * h;

            ResultT diff = std::abs((*this)(x) - rhs(x));
            if (diff > _eps) {
                return false;
            }
        }

        return true;
    }

    FUNCTION_TEMPLATE_1D
    bool FUNCTION_TYPE_1D::operator!=(const Function& rhs) const {
        return !(*this == rhs);
    }

    // Static methods

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::constant(ResultT value) {
        return Function([value](ArgT x) -> ResultT { return value; });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::linear(ResultT intercept, ResultT slope) {
        return Function([slope, intercept](ArgT x) -> ResultT {
            return slope * x + intercept;
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::polynomial(const Vector<ResultT>& coefficients) {
        return Function([coefficients](ArgT x) -> ResultT {
            ResultT result = 0;
            ResultT power = 1;

            for (size_t i = 0; i < coefficients.size(); i++) {
                result += coefficients[i] * power;
                power *= x;
            }

            return result;
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::exponential(ResultT base) {
        return Function([base](ArgT x) -> ResultT { 
            return std::pow(base, x); 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::logarithmic(ResultT base) {
        return Function([base](ArgT x) -> ResultT { 
            if (x <= 0) {
                throw std::invalid_argument("Logarithm undefined for non-positive values");
            }

            return std::log(x) / std::log(base); 
        });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_sin() {
        return Function([](ArgT x) -> ResultT { return std::sin(x); });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_cos() {
        return Function([](ArgT x) -> ResultT { return std::cos(x); });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_tan() {
        return Function([](ArgT x) -> ResultT { return std::tan(x); });
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::from_function_ptr(ResultT (*func)(ArgT)) {
        return Function(std::function<ResultT(ArgT)>(func));
    }

    // Friend methods

    FUNCTION_TEMPLATE_1D
    std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT>& f) {
        if (f.is_initialized()) {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT).name() << ")>[initialized]";
        } else {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT).name() << ")>[uninitialized]";
        }

        return os;
    }

    // External methods

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator+(ResultT scalar, const Function<ResultT, ArgT>& func) {
        return func + scalar;
    }

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator-(ResultT scalar, const Function<ResultT, ArgT>& func) {
        return Function<ResultT, ArgT>::constant(scalar) - func;
    }

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator*(ResultT scalar, const Function<ResultT, ArgT>& func) {
        return func * scalar;
    }

    template <typename ResultT, typename ArgT>
    Function<ResultT, ArgT> operator/(ResultT scalar, const Function<ResultT, ArgT>& func) {
        return Function<ResultT, ArgT>::constant(scalar) / func;
    }

    // Function 2-d class

    // Checks

    FUNCTION_TEMPLATE_2D
    void FUNCTION_TYPE_2D::_check_initialized(const char* op) const {
        if (!_func) {
            throw std::runtime_error(std::string("Function not initialized for ") + op);
        }
    }

    // Constructors/Destructor

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D::Function() : _func(nullptr) {}

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D::Function(std::function<ResultT(ArgT1, ArgT2)> func) : _func(func) {}

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D::Function(const Function& other) = default;

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D::Function(Function&& other) = default;

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D::~Function() = default;

    // Getters

    FUNCTION_TEMPLATE_2D
    double FUNCTION_TYPE_2D::eps() const noexcept { return _eps; }

    FUNCTION_TEMPLATE_2D
    bool FUNCTION_TYPE_2D::is_initialized() const noexcept { return static_cast<bool>(_func); }

    // Setters

    FUNCTION_TEMPLATE_2D
    void FUNCTION_TYPE_2D::set_eps(double new_eps) { _eps = new_eps; }

    // Copy/Move operators

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator=(const Function& other) = default;

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator=(Function&& other) = default;

    // Access operators

    FUNCTION_TEMPLATE_2D
    ResultT FUNCTION_TYPE_2D::operator()(ArgT1 x, ArgT2 y) const {
        _check_initialized("function evaluation");

        return _func(x, y);
    }

    FUNCTION_TEMPLATE_2D
    ResultT FUNCTION_TYPE_2D::at(ArgT1 x, ArgT2 y) const {
        return (*this)(x, y);
    }

    // Function currying

    FUNCTION_TEMPLATE_2D
    Function<ResultT, ArgT2> FUNCTION_TYPE_2D::bind_first(ArgT1 x_val) const {
        _check_initialized("partial application");

        return Function<ResultT, ArgT2>([this, x_val](ArgT2 y) -> ResultT { 
            return (*this)(x_val, y); 
        });
    }

    FUNCTION_TEMPLATE_2D
    Function<ResultT, ArgT1> FUNCTION_TYPE_2D::bind_second(ArgT2 y_val) const {
        _check_initialized("partial application");

        return Function<ResultT, ArgT1>([this, y_val](ArgT1 x) -> ResultT { 
            return (*this)(x, y_val); 
        });
    }

    // Arithmetic operators with functions

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator+(const Function& rhs) const {
        _check_initialized("addition");
        rhs._check_initialized("addition");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) + rhs(x, y); 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator-(const Function& rhs) const {
        _check_initialized("subtraction");
        rhs._check_initialized("subtraction");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) - rhs(x, y); 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator*(const Function& rhs) const {
        _check_initialized("multiplication");
        rhs._check_initialized("multiplication");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) * rhs(x, y); 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator/(const Function& rhs) const {
        _check_initialized("division");
        rhs._check_initialized("division");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y) -> ResultT { 
            ResultT denominator = rhs(x, y);

            if constexpr (std::is_floating_point_v<ResultT>) {
                if (std::abs(denominator) < _eps) {
                    throw std::invalid_argument("Division by zero in function");
                }
            } else {
                if (denominator == ResultT{0}) {
                    throw std::invalid_argument("Division by zero in function");
                }
            }

            return (*this)(x, y) / denominator; 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator+=(const Function& rhs) {
        *this = *this + rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator-=(const Function& rhs) {
        *this = *this - rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator*=(const Function& rhs) {
        *this = *this * rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator/=(const Function& rhs) {
        *this = *this / rhs;

        return *this;
    }

    // Arithmetic operators with scalars

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator+(ResultT scalar) const {
        _check_initialized("scalar addition");

        return Function([this, scalar](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) + scalar; 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator-(ResultT scalar) const {
        _check_initialized("scalar subtraction");

        return Function([this, scalar](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) - scalar; 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator*(ResultT scalar) const {
        _check_initialized("scalar multiplication");

        return Function([this, scalar](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y) * scalar; 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::operator/(ResultT scalar) const {
        _check_initialized("scalar division");

        if constexpr (std::is_floating_point_v<ResultT>) {
            if (std::abs(scalar) < _eps) {
                throw std::invalid_argument("Division by zero");
            }
        } else {
            if (scalar == ResultT{0}) {
                throw std::invalid_argument("Division by zero");
            }
        }

        return Function(std::function<ResultT(ArgT1, ArgT2)>(
            [this, scalar](ArgT1 x, ArgT2 y) { 
                return (*this)(x, y) / scalar; 
            })
        );
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator+=(ResultT scalar) {
        *this = *this + scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator-=(ResultT scalar) {
        *this = *this - scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator*=(ResultT scalar) {
        *this = *this * scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D& FUNCTION_TYPE_2D::operator/=(ResultT scalar) {
        *this = *this / scalar;

        return *this;
    }

    // Function composition

    FUNCTION_TEMPLATE_2D
    template <typename OuterResultT>
    Function<OuterResultT, ArgT1, ArgT2> FUNCTION_TYPE_2D::compose(
        const Function<OuterResultT, ResultT>& outer
    ) const {
        _check_initialized("composition");
        
        return Function<OuterResultT, ArgT1, ArgT2>(
            [this, outer](ArgT1 x, ArgT2 y) -> ResultT {
                return outer((*this)(x, y)); 
            }
        );
    }

    FUNCTION_TEMPLATE_2D
    template <typename CurveArgT>
    Function<ResultT, CurveArgT> FUNCTION_TYPE_2D::compose_with_curve(
        const Function<ArgT1, CurveArgT>& x_curve,
        const Function<ArgT2, CurveArgT>& y_curve
    ) const {
        _check_initialized("curve composition");

        if (!x_curve.is_initialized() || !y_curve.is_initialized()) {
            throw std::runtime_error("Curve functions not initialized for composition");
        }
        
        return Function<ResultT, CurveArgT>(
            [this, x_curve, y_curve](CurveArgT t) -> ResultT {
                return (*this)(x_curve(t), y_curve(t));
            }
        );
    }

    // Calculus operators

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, FUNCTION_TYPE_2D>
    FUNCTION_TYPE_2D::partial_derivative_x(ArgT1 h) const {
        _check_initialized("partial derivative x");
        
        return FUNCTION_TYPE_2D(
            [this, h](ArgT1 x, ArgT2 y) -> ResultT { 
                return ((*this)(x + h, y) - (*this)(x - h, y)) / (2 * h); 
            }
        );
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, FUNCTION_TYPE_2D>
    FUNCTION_TYPE_2D::partial_derivative_y(ArgT2 h) const {
        _check_initialized("partial derivative y");
        
        return FUNCTION_TYPE_2D(
            [this, h](ArgT1 x, ArgT2 y) -> ResultT { 
                return ((*this)(x, y + h) - (*this)(x, y - h)) / (2 * h); 
            }
        );
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_2D::definite_integral(
        ArgT1 a1, ArgT1 b1, 
        ArgT2 a2, ArgT2 b2,
        size_t n1, size_t n2
    ) const {
        _check_initialized("double integration");

        if (n1 == 0 || n2 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }

        if (n1 % 2 != 0) { n1++; }
        if (n2 % 2 != 0) { n2++; }

        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        
        ResultT sum = 0;

        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;

            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;

                double coeff_x = 1.0;
                double coeff_y = 1.0;
                
                if (i > 0 && i < n1) {
                    coeff_x = (i % 2 == 0) ? 2.0 : 4.0;
                }
                
                if (j > 0 && j < n2) {
                    coeff_y = (j % 2 == 0) ? 2.0 : 4.0;
                }
                
                sum += coeff_x * coeff_y * (*this)(x, y);
            }
        }
        
        return sum * (h1 / 3.0) * (h2 / 3.0);
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, 
        std::pair<FUNCTION_TYPE_2D, FUNCTION_TYPE_2D>
    >
    FUNCTION_TYPE_2D::gradient(ArgT1 hx, ArgT2 hy) const {
        _check_initialized("gradient");
        
        auto df_dx = partial_derivative_x(hx);
        auto df_dy = partial_derivative_y(hy);
        
        return std::make_pair(df_dx, df_dy);
    }

    // Functional properties

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_2D::norm(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1, size_t n2) const {
        _check_initialized("norm calculation");
        
        if (n1 == 0 || n2 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        
        ResultT sum = 0;
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            double coeff_x = (i == 0 || i == n1) ? 0.5 : 1.0;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;
                double coeff_y = (j == 0 || j == n2) ? 0.5 : 1.0;
                
                ResultT f_val = (*this)(x, y);

                sum += coeff_x * coeff_y * f_val * f_val;
            }
        }
        
        return std::sqrt(sum * h1 * h2);
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_2D::maximum(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1, size_t n2) const {
        _check_initialized("maximum finding");
        
        if (n1 == 0 || n2 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        
        ResultT max_val = (*this)(a1, a2);
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;
    
                ResultT val = (*this)(x, y);
                if (val > max_val) {
                    max_val = val;
                }
            }
        }
        
        return max_val;
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>  
    FUNCTION_TYPE_2D::minimum(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1, size_t n2) const {
        _check_initialized("minimum finding");
        
        if (n1 == 0 || n2 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        
        ResultT min_val = (*this)(a1, a2);
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;

                ResultT val = (*this)(x, y);
                if (val < min_val) {
                    min_val = val;
                }
            }
        }
        
        return min_val;
    }

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, std::pair<ArgT1, ArgT2>>
    FUNCTION_TYPE_2D::root(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n) const {
        _check_initialized("root finding");

        ArgT1 best_x = a1;
        ArgT2 best_y = a2;

        ResultT min_abs_val = std::abs((*this)(a1, a2));

        ArgT1 h1 = (b1 - a1) / n;
        ArgT2 h2 = (b2 - a2) / n;

        for (size_t i = 0; i <= n; i++) {
            ArgT1 x = a1 + i * h1;

            for (size_t j = 0; j <= n; j++) {
                ArgT2 y = a2 + j * h2;

                ResultT abs_val = std::abs((*this)(x, y));

                if (abs_val < min_abs_val) {
                    min_abs_val = abs_val;

                    best_x = x;
                    best_y = y;
                }
            }
        }
        
        if (min_abs_val < _eps * 10) {
            return {best_x, best_y};
        }

        auto grad = gradient();
        auto [df_dx_func, df_dy_func] = grad;

        for (size_t refine_iter = 0; refine_iter < 10; refine_iter++) {
            ResultT f_val = (*this)(best_x, best_y);
            
            if (std::abs(f_val) < _eps * 10) {
                return {best_x, best_y};
            }
            
            ResultT df_dx = df_dx_func(best_x, best_y);
            ResultT df_dy = df_dy_func(best_x, best_y);
            
            ResultT jacobian = df_dx * df_dx + df_dy * df_dy;

            if (std::abs(jacobian) > _eps) {
                best_x -= (df_dx / jacobian) * f_val;
                best_y -= (df_dy / jacobian) * f_val;
            }

            best_x = std::max(a1, std::min(b1, best_x));
            best_y = std::max(a2, std::min(b2, best_y));
        }
        
        throw std::invalid_argument("Unable to find root within the given tolerance");
    }

    // Functional analysis

    FUNCTION_TEMPLATE_2D
    std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
    FUNCTION_TYPE_2D::is_continuous(ArgT1 a1, ArgT1 b1, ArgT2 a2, ArgT2 b2, size_t n1, size_t n2) const {
        _check_initialized("continuity check");

        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;

        for (size_t i = 0; i < n1; i++) {
            ArgT1 x1 = a1 + i * h1;
            ArgT1 x2 = a1 + (i + 1) * h1;
            
            for (size_t j = 0; j < n2; j++) {
                ArgT2 y1 = a2 + j * h2;
                ArgT2 y2 = a2 + (j + 1) * h2;

                ResultT diff_x = std::abs((*this)(x2, y1) - (*this)(x1, y1));
                ResultT diff_y = std::abs((*this)(x1, y2) - (*this)(x1, y1));

                ResultT threshold = std::max(ResultT(1.0), 
                    std::max(std::abs((*this)(x1, y1)), ResultT(1.0))) * 0.1;
                
                if (diff_x > threshold || diff_y > threshold) {
                    return false;
                }
            }
        }
        
        return true;
    }

    // Comparison operators

    FUNCTION_TEMPLATE_2D
    bool FUNCTION_TYPE_2D::operator==(const Function& rhs) const {
        if (!is_initialized() && !rhs.is_initialized()) { return true; }
        if (!is_initialized() || !rhs.is_initialized()) { return false; }

        const std::vector<std::tuple<ArgT1, ArgT1, ArgT2, ArgT2>> test_regions = {
            {-1.0, 1.0, -1.0, 1.0},
            {-5.0, 5.0, -5.0, 5.0},
            {0.1, 2.0, 0.1, 2.0}
        };
        
        const size_t test_n = 20;
        
        for (const auto& region : test_regions) {
            auto [x1, x2, y1, y2] = region;

            ArgT1 hx = (x2 - x1) / test_n;
            ArgT2 hy = (y2 - y1) / test_n;
            
            for (size_t i = 0; i <= test_n; i++) {
                ArgT1 x = x1 + i * hx;

                for (size_t j = 0; j <= test_n; j++) {
                    ArgT2 y = y1 + j * hy;
                    
                    try {
                        ResultT val1 = (*this)(x, y);
                        ResultT val2 = rhs(x, y);
                        
                        if constexpr (std::is_floating_point_v<ResultT>) {
                            ResultT diff = std::abs(val1 - val2);
                            ResultT relative_diff = diff / (std::max(std::abs(val1), std::abs(val2)) + 1.0);
                            
                            if (diff > _eps && relative_diff > _eps) {
                                return false;
                            }
                        } else {
                            if (val1 != val2) {
                                return false;
                            }
                        }
                    } catch (...) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }

    FUNCTION_TEMPLATE_2D
    bool FUNCTION_TYPE_2D::operator!=(const Function& rhs) const {
        return !(*this == rhs);
    }

    // Static methods

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::constant(ResultT value) {
        return Function([value](ArgT1 x, ArgT2 y) -> ResultT { return value; });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::bilinear(ResultT a, ResultT b, ResultT c, ResultT d) {
        return Function([a, b, c, d](ArgT1 x, ArgT2 y) -> ResultT { 
            return a * x * y + b * x + c * y + d; 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::gaussian(ResultT sigma_x, ResultT sigma_y) {
        return Function([sigma_x, sigma_y](ArgT1 x, ArgT2 y) -> ResultT { 
            return std::exp(-(x * x) / (2 * sigma_x * sigma_x) - (y * y) / (2 * sigma_y * sigma_y)); 
        });
    }

    FUNCTION_TEMPLATE_2D
    FUNCTION_TYPE_2D FUNCTION_TYPE_2D::from_function_ptr(ResultT (*func)(ArgT1, ArgT2)) {
        return Function(std::function<ResultT(ArgT1, ArgT2)>(func));
    }

    // Friend methods

    FUNCTION_TEMPLATE_2D
    std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT1, ArgT2>& f) {
        if (f.is_initialized()) {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT1).name() << ", " << typeid(ArgT2).name() << ")>[initialized]";
        } else {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT1).name() << ", " << typeid(ArgT2).name() << ")>[uninitialized]";
        }

        return os;
    }

    // Function 3-d class

    // Checks

    FUNCTION_TEMPLATE_3D
    void FUNCTION_TYPE_3D::_check_initialized(const char* op) const {
        if (!_func) {
            throw std::runtime_error(std::string("Function not initialized for ") + op);
        }
    }

    // Constructors/Destructor

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D::Function() : _func(nullptr) {}

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D::Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)> func) : _func(func) {}

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D::Function(const Function& other) = default;

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D::Function(Function&& other) = default;

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D::~Function() = default;

    // Getters

    FUNCTION_TEMPLATE_3D
    double FUNCTION_TYPE_3D::eps() const noexcept { return _eps; }

    FUNCTION_TEMPLATE_3D
    bool FUNCTION_TYPE_3D::is_initialized() const noexcept { return static_cast<bool>(_func); }

    // Setters

    FUNCTION_TEMPLATE_3D
    void FUNCTION_TYPE_3D::set_eps(double new_eps) { _eps = new_eps; }

    // Copy/Move operators

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator=(const Function& other) = default;

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator=(Function&& other) = default;

    // Access operators

    FUNCTION_TEMPLATE_3D
    ResultT FUNCTION_TYPE_3D::operator()(ArgT1 x, ArgT2 y, ArgT3 z) const {
        _check_initialized("function evaluation");

        return _func(x, y, z);
    }

    FUNCTION_TEMPLATE_3D
    ResultT FUNCTION_TYPE_3D::at(ArgT1 x, ArgT2 y, ArgT3 z) const {
        return (*this)(x, y, z);
    }

    // Function currying

    FUNCTION_TEMPLATE_3D
    Function<ResultT, ArgT2, ArgT3> FUNCTION_TYPE_3D::bind_first(ArgT1 x_val) const {
        _check_initialized("partial application");

        return Function<ResultT, ArgT2, ArgT3>([this, x_val](ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x_val, y, z); 
        });
    }

    FUNCTION_TEMPLATE_3D
    Function<ResultT, ArgT1, ArgT3> FUNCTION_TYPE_3D::bind_second(ArgT2 y_val) const {
        _check_initialized("partial application");

        return Function<ResultT, ArgT1, ArgT3>([this, y_val](ArgT1 x, ArgT3 z) -> ResultT { 
            return (*this)(x, y_val, z); 
        });
    }

    FUNCTION_TEMPLATE_3D
    Function<ResultT, ArgT1, ArgT2> FUNCTION_TYPE_3D::bind_third(ArgT3 z_val) const {
        _check_initialized("partial application");

        return Function<ResultT, ArgT1, ArgT2>([this, z_val](ArgT1 x, ArgT2 y) -> ResultT { 
            return (*this)(x, y, z_val); 
        });
    }

    // Arithmetic operators with functions

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator+(const Function& rhs) const {
        _check_initialized("addition");
        rhs._check_initialized("addition");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) + rhs(x, y, z); 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator-(const Function& rhs) const {
        _check_initialized("subtraction");
        rhs._check_initialized("subtraction");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) - rhs(x, y, z); 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator*(const Function& rhs) const {
        _check_initialized("multiplication");
        rhs._check_initialized("multiplication");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) * rhs(x, y, z); 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator/(const Function& rhs) const {
        _check_initialized("division");
        rhs._check_initialized("division");
        
        return Function([this, rhs](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            ResultT denominator = rhs(x, y, z);

            if constexpr (std::is_floating_point_v<ResultT>) {
                if (std::abs(denominator) < _eps) {
                    throw std::invalid_argument("Division by zero in function");
                }
            } else {
                if (denominator == ResultT{0}) {
                    throw std::invalid_argument("Division by zero in function");
                }
            }

            return (*this)(x, y, z) / denominator; 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator+=(const Function& rhs) {
        *this = *this + rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator-=(const Function& rhs) {
        *this = *this - rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator*=(const Function& rhs) {
        *this = *this * rhs;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator/=(const Function& rhs) {
        *this = *this / rhs;

        return *this;
    }

    // Arithmetic operators with scalars

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator+(ResultT scalar) const {
        _check_initialized("scalar addition");

        return Function([this, scalar](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) + scalar; 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator-(ResultT scalar) const {
        _check_initialized("scalar subtraction");

        return Function([this, scalar](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) - scalar; 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator*(ResultT scalar) const {
        _check_initialized("scalar multiplication");

        return Function([this, scalar](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) * scalar; 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::operator/(ResultT scalar) const {
        _check_initialized("scalar division");

        if constexpr (std::is_floating_point_v<ResultT>) {
            if (std::abs(scalar) < _eps) {
                throw std::invalid_argument("Division by zero");
            }
        } else {
            if (scalar == ResultT{0}) {
                throw std::invalid_argument("Division by zero");
            }
        }

        return Function([this, scalar](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return (*this)(x, y, z) / scalar; 
        });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator+=(ResultT scalar) {
        *this = *this + scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator-=(ResultT scalar) {
        *this = *this - scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator*=(ResultT scalar) {
        *this = *this * scalar;

        return *this;
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D& FUNCTION_TYPE_3D::operator/=(ResultT scalar) {
        *this = *this / scalar;

        return *this;
    }
    
    // Function composition

    FUNCTION_TEMPLATE_3D
    template <typename OuterResultT>
    Function<OuterResultT, ArgT1, ArgT2, ArgT3> FUNCTION_TYPE_3D::compose(
        const Function<OuterResultT, ResultT>& outer
    ) const {
        _check_initialized("composition");
        
        return Function<OuterResultT, ArgT1, ArgT2, ArgT3>(
            [this, outer](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
                return outer((*this)(x, y, z)); 
            }
        );
    }

    FUNCTION_TEMPLATE_3D
    template <typename CurveArgT>
    Function<ResultT, CurveArgT> FUNCTION_TYPE_3D::compose_with_curve(
        const Function<ArgT1, CurveArgT>& x_curve,
        const Function<ArgT2, CurveArgT>& y_curve,
        const Function<ArgT3, CurveArgT>& z_curve
    ) const {
        _check_initialized("curve composition");
        
        return Function<ResultT, CurveArgT>(
            [this, x_curve, y_curve, z_curve](CurveArgT t) -> ResultT {
                return (*this)(x_curve(t), y_curve(t), z_curve(t));
            }
        );
    }

    // Calculus operators

    // Calculus operators

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
    FUNCTION_TYPE_3D::partial_derivative_x(ArgT1 h) const {
        _check_initialized("partial derivative x");
        
        return FUNCTION_TYPE_3D([this, h](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return ((*this)(x + h, y, z) - (*this)(x - h, y, z)) / (2 * h); 
        });
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
    FUNCTION_TYPE_3D::partial_derivative_y(ArgT2 h) const {
        _check_initialized("partial derivative y");
        
        return FUNCTION_TYPE_3D([this, h](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return ((*this)(x, y + h, z) - (*this)(x, y - h, z)) / (2 * h); 
        });
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, Function<ResultT, ArgT1, ArgT2, ArgT3>>
    FUNCTION_TYPE_3D::partial_derivative_z(ArgT3 h) const {
        _check_initialized("partial derivative z");
        
        return FUNCTION_TYPE_3D([this, h](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
            return ((*this)(x, y, z + h) - (*this)(x, y, z - h)) / (2 * h); 
        });
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_3D::definite_integral(
        ArgT1 a1, ArgT1 b1,
        ArgT2 a2, ArgT2 b2, 
        ArgT3 a3, ArgT3 b3,
        size_t n1, size_t n2, size_t n3
    ) const {
        _check_initialized("triple integration");

        if (n1 == 0 || n2 == 0 || n3 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }

        if (n1 % 2 != 0) { n1++; }
        if (n2 % 2 != 0) { n2++; }
        if (n3 % 2 != 0) { n3++; }

        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        ArgT3 h3 = (b3 - a3) / n3;
        
        ResultT sum = 0;

        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;

            double coeff_x = 1.0;
            
            if (i > 0 && i < n1) {
                coeff_x = (i % 2 == 0) ? 2.0 : 4.0;
            }
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;

                double coeff_y = 1.0;
                
                if (j > 0 && j < n2) {
                    coeff_y = (j % 2 == 0) ? 2.0 : 4.0;
                }
                
                for (size_t k = 0; k <= n3; k++) {
                    ArgT3 z = a3 + k * h3;

                    double coeff_z = 1.0;
                    
                    if (k > 0 && k < n3) {
                        coeff_z = (k % 2 == 0) ? 2.0 : 4.0;
                    }
                    
                    sum += coeff_x * coeff_y * coeff_z * (*this)(x, y, z);
                }
            }
        }
        
        return sum * (h1 / 3.0) * (h2 / 3.0) * (h3 / 3.0);
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<
        std::is_floating_point_v<ResultT>, 
        std::tuple<
            Function<ResultT, ArgT1, ArgT2, ArgT3>, 
            Function<ResultT, ArgT1, ArgT2, ArgT3>,
            Function<ResultT, ArgT1, ArgT2, ArgT3>
        >
    >
    FUNCTION_TYPE_3D::gradient(ArgT1 hx, ArgT2 hy, ArgT3 hz) const {
        _check_initialized("gradient");
        
        auto df_dx = partial_derivative_x(hx);
        auto df_dy = partial_derivative_y(hy);
        auto df_dz = partial_derivative_z(hz);
        
        return std::make_tuple(df_dx, df_dy, df_dz);
    }

    // Functional properties

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_3D::norm(
        ArgT1 a1, ArgT1 b1,
        ArgT2 a2, ArgT2 b2,
        ArgT3 a3, ArgT3 b3, 
        size_t n1, size_t n2, size_t n3
    ) const {
        _check_initialized("norm calculation");
        
        if (n1 == 0 || n2 == 0 || n3 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        ArgT3 h3 = (b3 - a3) / n3;
        
        ResultT sum = 0;
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            double coeff_x = (i == 0 || i == n1) ? 0.5 : 1.0;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;
                double coeff_y = (j == 0 || j == n2) ? 0.5 : 1.0;
                
                for (size_t k = 0; k <= n3; k++) {
                    ArgT3 z = a3 + k * h3;
                    double coeff_z = (k == 0 || k == n3) ? 0.5 : 1.0;
                    
                    ResultT f_val = (*this)(x, y, z);

                    sum += coeff_x * coeff_y * coeff_z * f_val * f_val;
                }
            }
        }
        
        return std::sqrt(sum * h1 * h2 * h3);
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_3D::maximum(
        ArgT1 a1, ArgT1 b1,
        ArgT2 a2, ArgT2 b2,
        ArgT3 a3, ArgT3 b3,
        size_t n1, size_t n2, size_t n3
    ) const {
        _check_initialized("maximum finding");
        
        if (n1 == 0 || n2 == 0 || n3 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        ArgT3 h3 = (b3 - a3) / n3;
        
        ResultT max_val = (*this)(a1, a2, a3);
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;
                
                for (size_t k = 0; k <= n3; k++) {
                    ArgT3 z = a3 + k * h3;

                    ResultT val = (*this)(x, y, z);
                    if (val > max_val) {
                        max_val = val;
                    }
                }
            }
        }
        
        return max_val;
    }

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, ResultT>
    FUNCTION_TYPE_3D::minimum(
        ArgT1 a1, ArgT1 b1,
        ArgT2 a2, ArgT2 b2,
        ArgT3 a3, ArgT3 b3,
        size_t n1, size_t n2, size_t n3
    ) const {
        _check_initialized("minimum finding");
        
        if (n1 == 0 || n2 == 0 || n3 == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        ArgT3 h3 = (b3 - a3) / n3;
        
        ResultT min_val = (*this)(a1, a2, a3);
        
        for (size_t i = 0; i <= n1; i++) {
            ArgT1 x = a1 + i * h1;
            
            for (size_t j = 0; j <= n2; j++) {
                ArgT2 y = a2 + j * h2;
                
                for (size_t k = 0; k <= n3; k++) {
                    ArgT3 z = a3 + k * h3;

                    ResultT val = (*this)(x, y, z);
                    if (val < min_val) {
                        min_val = val;
                    }
                }
            }
        }
        
        return min_val;
    }

    // Functional analysis

    FUNCTION_TEMPLATE_3D
    std::enable_if_t<std::is_floating_point_v<ResultT>, bool>
    FUNCTION_TYPE_3D::is_continuous(
        ArgT1 a1, ArgT1 b1,
        ArgT2 a2, ArgT2 b2,
        ArgT3 a3, ArgT3 b3,
        size_t n1, size_t n2, size_t n3
    ) const {
        _check_initialized("continuity check");

        ArgT1 h1 = (b1 - a1) / n1;
        ArgT2 h2 = (b2 - a2) / n2;
        ArgT3 h3 = (b3 - a3) / n3;

        for (size_t i = 0; i < n1; i++) {
            ArgT1 x1 = a1 + i * h1;
            ArgT1 x2 = a1 + (i + 1) * h1;
            
            for (size_t j = 0; j < n2; j++) {
                ArgT2 y1 = a2 + j * h2;
                ArgT2 y2 = a2 + (j + 1) * h2;
                
                for (size_t k = 0; k < n3; k++) {
                    ArgT3 z1 = a3 + k * h3;
                    ArgT3 z2 = a3 + (k + 1) * h3;

                    ResultT diff_x = std::abs((*this)(x2, y1, z1) - (*this)(x1, y1, z1));
                    ResultT diff_y = std::abs((*this)(x1, y2, z1) - (*this)(x1, y1, z1));
                    ResultT diff_z = std::abs((*this)(x1, y1, z2) - (*this)(x1, y1, z1));

                    ResultT base_val = std::abs((*this)(x1, y1, z1));
                    ResultT threshold = std::max(ResultT(1.0), base_val) * 0.1;
                    
                    if (diff_x > threshold || diff_y > threshold || diff_z > threshold) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }

    // Static methods

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::constant(ResultT value) {
        return Function([value](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { return value; });
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::trilinear(
        ResultT a, ResultT b, ResultT c, ResultT d, 
        ResultT e, ResultT f, ResultT g, ResultT h
    ) {
        return Function(
            [a, b, c, d, e, f, g, h](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
                return a * x * y * z + b * x * y + c * x * z + d * y * z + e * x + f * y + g * z + h; 
            }
        );
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::gaussian(
        ResultT sigma_x, ResultT sigma_y, ResultT sigma_z
    ) {
        return Function(
            [sigma_x, sigma_y, sigma_z](ArgT1 x, ArgT2 y, ArgT3 z) -> ResultT { 
                return std::exp(
                    - (x * x) / (2 * sigma_x * sigma_x) - 
                    (y * y) / (2 * sigma_y * sigma_y) - 
                    (z * z) / (2 * sigma_z * sigma_z)
                ); 
            }
        );
    }

    FUNCTION_TEMPLATE_3D
    FUNCTION_TYPE_3D FUNCTION_TYPE_3D::from_function_ptr(ResultT (*func)(ArgT1, ArgT2, ArgT3)) {
        return Function(std::function<ResultT(ArgT1, ArgT2, ArgT3)>(func));
    }

    // Friend methods

    FUNCTION_TEMPLATE_3D
    std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT1, ArgT2, ArgT3>& f) {
        if (f.is_initialized()) {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT1).name() << ", " << typeid(ArgT2).name() 
            << ", " << typeid(ArgT3).name() << ")>[initialized]";
        } else {
            os << "Function<" << typeid(ResultT).name() 
            << "(" << typeid(ArgT1).name() << ", " << typeid(ArgT2).name() 
            << ", " << typeid(ArgT3).name() << ")>[uninitialized]";
        }

        return os;
    }

    // Undef for internal-use only

    #undef FUNCTION_TEMPLATE_1D
    #undef FUNCTION_TEMPLATE_2D
    #undef FUNCTION_TEMPLATE_3D
    #undef FUNCTION_TYPE_1D
    #undef FUNCTION_TYPE_2D
    #undef FUNCTION_TYPE_3D
}
