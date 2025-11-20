#include "../Vector.hpp"


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
        
        return Function(std::function<ResultT(ArgT)>([this, rhs](ArgT x) { 
            return (*this)(x) + rhs(x); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator-(const Function& rhs) const {
        _check_initialized("subtraction");
        rhs._check_initialized("subtraction");
        
        return Function(std::function<ResultT(ArgT)>([this, rhs](ArgT x) { 
            return (*this)(x) - rhs(x); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator*(const Function& rhs) const {
        _check_initialized("multiplication");
        rhs._check_initialized("multiplication");
        
        return Function(std::function<ResultT(ArgT)>([this, rhs](ArgT x) { 
            return (*this)(x) * rhs(x); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator/(const Function& rhs) const {
        _check_initialized("division");
        rhs._check_initialized("division");
        
        return Function(std::function<ResultT(ArgT)>([this, rhs](ArgT x) { 
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
        }));
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

        return Function(std::function<ResultT(ArgT)>([this, scalar](ArgT x) { 
            return (*this)(x) + scalar; 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator-(ResultT scalar) const {
        _check_initialized("scalar subtraction");

        return Function(std::function<ResultT(ArgT)>([this, scalar](ArgT x) { 
            return (*this)(x) - scalar; 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::operator*(ResultT scalar) const {
        _check_initialized("scalar multiplication");

        return Function(std::function<ResultT(ArgT)>([this, scalar](ArgT x) { 
            return (*this)(x) * scalar; 
        }));
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

        return Function(std::function<ResultT(ArgT)>([this, scalar](ArgT x) { 
            return (*this)(x) / scalar; 
        }));
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
    template<typename InnerResultT>
    Function<ResultT, InnerResultT> FUNCTION_TYPE_1D::compose(const Function<InnerResultT, ArgT>& inner) const {
        _check_initialized("composition");
        inner._check_initialized("composition");
        
        return Function<ResultT, InnerResultT>(
            std::function<ResultT(InnerResultT)>([this, inner](InnerResultT x) { 
                return (*this)(inner(x)); 
            })
        );
    }

    // Calculus operators

    FUNCTION_TEMPLATE_1D
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, FUNCTION_TYPE_1D>
    FUNCTION_TYPE_1D::derivative(ArgT h) const {
        _check_initialized("derivative");

        return Function(std::function<ResultT(ArgT)>([this, h](ArgT x) { 
            return ((*this)(x + h) - (*this)(x - h)) / (2 * h); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, ResultT>
    FUNCTION_TYPE_1D::definite_integral(ArgT a, ArgT b, size_t n) const {
        _check_initialized("integration");

        if (n == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }

        ArgT h = (b - a) / n;

        ResultT sum = ((*this)(a) + (*this)(b)) / 2;
        for (size_t i = 1; i < n; i++) {
            sum += (*this)(a + i * h);
        }

        return sum * h;
    }

    FUNCTION_TEMPLATE_1D
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, FUNCTION_TYPE_1D>
    FUNCTION_TYPE_1D::integral(ArgT a, ArgT b, size_t n) const {
        ResultT value = definite_integral(a, b, n);

        return Function::constant(value);
    }

    // Functional properties

    FUNCTION_TEMPLATE_1D
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, ResultT>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, ResultT>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, ResultT>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, ResultT>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, bool>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, bool>
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
    template<typename U>
    std::enable_if_t<std::is_floating_point_v<U>, bool>
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
        return Function(std::function<ResultT(ArgT)>([value](ArgT x) { return value; }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::linear(ResultT intercept, ResultT slope) {
        return Function(std::function<ResultT(ArgT)>([slope, intercept](ArgT x) { return slope * x + intercept; }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::polynomial(const Vector<ResultT>& coefficients) {
        return Function(std::function<ResultT(ArgT)>([coefficients](ArgT x) {
            ResultT result = 0;
            ResultT power = 1;

            for (size_t i = 0; i < coefficients.size(); i++) {
                result += coefficients[i] * power;
                power *= x;
            }

            return result;
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::exponential(ResultT base) {
        return Function(std::function<ResultT(ArgT)>([base](ArgT x) { 
            return std::pow(base, x); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::logarithmic(ResultT base) {
        return Function(std::function<ResultT(ArgT)>([base](ArgT x) { 
            if (x <= 0) {
                throw std::invalid_argument("Logarithm undefined for non-positive values");
            }

            return std::log(x) / std::log(base); 
        }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_sin() {
        return Function(std::function<ResultT(ArgT)>([](ArgT x) { return std::sin(x); }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_cos() {
        return Function(std::function<ResultT(ArgT)>([](ArgT x) { return std::cos(x); }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::trigonometric_tan() {
        return Function(std::function<ResultT(ArgT)>([](ArgT x) { return std::tan(x); }));
    }

    FUNCTION_TEMPLATE_1D
    FUNCTION_TYPE_1D FUNCTION_TYPE_1D::from_function_ptr(ResultT (*func)(ArgT)) {
        return Function(std::function<ResultT(ArgT)>(func));
    }

    // Friend methods

    FUNCTION_TEMPLATE_1D
    std::ostream& operator<<(std::ostream& os, const Function<ResultT, ArgT>& f) {
        if (f.is_initialized()) {
            os << "Function<" << typeid(ResultT).name() << "(" << typeid(ArgT).name() << ")>[initialized]";
        } else {
            os << "Function<" << typeid(ResultT).name() << "(" << typeid(ArgT).name() << ")>[uninitialized]";
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

    

    // Undef for internal-use only

    #undef FUNCTION_TEMPLATE_1D
    #undef FUNCTION_TEMPLATE_2D
    #undef FUNCTION_TEMPLATE_3D
    #undef FUNCTION_TYPE_1D
    #undef FUNCTION_TYPE_2D
    #undef FUNCTION_TYPE_3D
}
