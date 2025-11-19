namespace vmafu {
    // Checks

    template <typename T>
    void Function<T>::_check_initialized(const char* op) const {
        if (!_func) {
            throw std::runtime_error(std::string("Function not initialized for ") + op);
        }
    }

    // Constructors/Destructor

    template <typename T>
    Function<T>::Function() : _func(nullptr) {}

    template <typename T>
    Function<T>::Function(std::function<T(T)> func) : _func(func) {}

    template <typename T>
    Function<T>::Function(const Function& other) = default;

    template <typename T>
    Function<T>::Function(Function&& other) = default;

    // Getters

    template <typename T>
    double Function<T>::eps() const noexcept { return _eps; }

    template <typename T>
    bool Function<T>::is_initialized() const noexcept { return static_cast<bool>(_func); }

    // Setters

    template <typename T>
    void Function<T>::set_eps(double new_eps) { _eps = new_eps; }

    // Copy/Move operators

    template <typename T>
    Function<T>& Function<T>::operator=(const Function& other) = default;

    template <typename T>
    Function<T>& Function<T>::operator=(Function&& other) = default;

    // Access operators

    template <typename T>
    T Function<T>::operator()(T x) const {
        _check_initialized("function evaluation");

        return _func(x);
    }

    template <typename T>
    T Function<T>::at(T x) const {
        return (*this)(x);
    }

    // Arithmetic operators with functions

    template <typename T>
    Function<T> Function<T>::operator+(const Function& rhs) const {
        _check_initialized("addition");
        rhs._check_initialized("addition");
        
        return Function<T>(std::function<T(T)>([this, &rhs](T x) { 
            return (*this)(x) + rhs(x); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator-(const Function& rhs) const {
        _check_initialized("subtraction");
        rhs._check_initialized("subtraction");
        
        return Function<T>(std::function<T(T)>([this, &rhs](T x) { 
            return (*this)(x) - rhs(x); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator*(const Function& rhs) const {
        _check_initialized("multiplication");
        rhs._check_initialized("multiplication");
        
        return Function<T>(std::function<T(T)>([this, &rhs](T x) { 
            return (*this)(x) * rhs(x); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator/(const Function& rhs) const {
        _check_initialized("division");
        rhs._check_initialized("division");
        
        return Function<T>(std::function<T(T)>([this, &rhs](T x) { 
            T denominator = rhs(x);

            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(denominator) < _eps) {
                    throw std::invalid_argument("Division by zero in function");
                }
            } else {
                if (denominator == 0) {
                    throw std::invalid_argument("Division by zero in function");
                }
            }

            return (*this)(x) / denominator; 
        }));
    }

    template <typename T>
    Function<T>& Function<T>::operator+=(const Function& rhs) {
        *this = *this + rhs;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator-=(const Function& rhs) {
        *this = *this - rhs;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator*=(const Function& rhs) {
        *this = *this * rhs;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator/=(const Function& rhs) {
        *this = *this / rhs;

        return *this;
    }

    // Arithmetic operators with scalars

    template <typename T>
    Function<T> Function<T>::operator+(T scalar) const {
        _check_initialized("scalar addition");
        
        return Function<T>(std::function<T(T)>([this, scalar](T x) { 
            return (*this)(x) + scalar; 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator-(T scalar) const {
        _check_initialized("scalar subtraction");
        
        return Function<T>(std::function<T(T)>([this, scalar](T x) { 
            return (*this)(x) - scalar; 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator*(T scalar) const {
        _check_initialized("scalar multiplication");
        
        return Function<T>(std::function<T(T)>([this, scalar](T x) { 
            return (*this)(x) * scalar; 
        }));
    }

    template <typename T>
    Function<T> Function<T>::operator/(T scalar) const {
        _check_initialized("scalar division");
        
        if constexpr (std::is_floating_point_v<T>) {
            if (std::abs(scalar) < _eps) {
                throw std::invalid_argument("Division by zero");
            }
        } else {
            if (scalar == 0) {
                throw std::invalid_argument("Division by zero");
            }
        }
        
        return Function<T>(std::function<T(T)>([this, scalar](T x) { 
            return (*this)(x) / scalar; 
        }));
    }

    template <typename T>
    Function<T>& Function<T>::operator+=(T scalar) {
        *this = *this + scalar;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator-=(T scalar) {
        *this = *this - scalar;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator*=(T scalar) {
        *this = *this * scalar;

        return *this;
    }

    template <typename T>
    Function<T>& Function<T>::operator/=(T scalar) {
        *this = *this / scalar;

        return *this;
    }

    // Function composition

    template <typename T>
    Function<T> Function<T>::compose(const Function& inner) const {
        _check_initialized("composition");
        inner._check_initialized("composition");
        
        return Function<T>(std::function<T(T)>([this, &inner](T x) { 
            return (*this)(inner(x)); 
        }));
    }

    // Calculus operators

    template <typename T>
    Function<T> Function<T>::derivative(T h) const {
        _check_initialized("derivative");
        
        return Function<T>(std::function<T(T)>([this, h](T x) { 
            return ((*this)(x + h) - (*this)(x - h)) / (2 * h); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::integral(T a, T b, size_t n) const {
        T value = definite_integral(a, b, n);

        return Function<T>::constant(value);
    }

    template <typename T>
    T Function<T>::definite_integral(T a, T b, size_t n) const {
        _check_initialized("integration");
        
        if (n == 0) {
            throw std::invalid_argument("Number of intervals must be positive");
        }
        
        T h = (b - a) / n;

        T sum = ((*this)(a) + (*this)(b)) / 2;

        for (size_t i = 1; i < n; i++) {
            sum += (*this)(a + i * h);
        }
        
        return sum * h;
    }

    // Functional properties

    template <typename T>
    T Function<T>::norm(T a, T b, size_t n) const {
        _check_initialized("norm calculation");

        T h = (b - a) / n;

        T sum = ((*this)(a) * (*this)(a) + (*this)(b) * (*this)(b)) / 2;
        
        for (size_t i = 1; i < n; i++) {
            T x = a + i * h;
            T fx = (*this)(x);

            sum += fx * fx;
        }
        
        return std::sqrt(sum * h);
    }

    template <typename T>
    T Function<T>::maximum(T a, T b, size_t n) const {
        _check_initialized("maximum finding");
        
        T h = (b - a) / n;
        T max_val = (*this)(a);
        
        for (size_t i = 1; i <= n; i++) {
            T x = a + i * h;
            T val = (*this)(x);

            if (val > max_val) {
                max_val = val;
            }
        }
        
        return max_val;
    }

    template <typename T>
    T Function<T>::minimum(T a, T b, size_t n) const {
        _check_initialized("minimum finding");
        
        T h = (b - a) / n;
        T min_val = (*this)(a);
        
        for (size_t i = 1; i <= n; i++) {
            T x = a + i * h;
            T val = (*this)(x);
            if (val < min_val) {
                min_val = val;
            }
        }
        
        return min_val;
    }

    template <typename T>
    T Function<T>::root(T a, T b, size_t n) const {
        _check_initialized("root finding");
        
        if (!has_root(a, b)) {
            throw std::invalid_argument("Function has no root in the given interval");
        }
        
        T left = a, right = b;
        for (size_t i = 0; i < n; i++) {
            T mid = (left + right) / 2;
            T f_mid = (*this)(mid);
            
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

    template <typename T>
    bool Function<T>::is_continuous(T a, T b, size_t n) const {
        _check_initialized("continuity check");

        T h = (b - a) / n;
        for (size_t i = 0; i < n; i++) {
            T x1 = a + i * h;
            T x2 = a + (i + 1) * h;
            
            T diff = std::abs((*this)(x2) - (*this)(x1));

            if (diff > 1.0 / h) {
                return false;
            }
        }

        return true;
    }

    template <typename T>
    bool Function<T>::is_monotonic(T a, T b, size_t n) const {
        _check_initialized("monotonicity check");
        
        T h = (b - a) / n;

        bool increasing = true, decreasing = true;
        
        for (size_t i = 1; i < n; i++) {
            T x1 = a + (i - 1) * h;
            T x2 = a + i * h;
            
            T diff = (*this)(x2) - (*this)(x1);

            if (diff < -_eps) { increasing = false; }
            if (diff > _eps) { decreasing = false; }
            
            if (!increasing && !decreasing) {
                return false;
            }
        }
        
        return true;
    }

    template <typename T>
    bool Function<T>::has_root(T a, T b, size_t n) const {
        _check_initialized("root existence check");
        
        T fa = (*this)(a);
        T fb = (*this)(b);

        if (fa * fb <= 0) {
            return true;
        }

        T h = (b - a) / n;

        for (size_t i = 1; i < n; i++) {
            T x = a + i * h;
            T fx = (*this)(x);

            if (std::abs(fx) < _eps || fa * fx <= 0) {
                return true;
            }
        }
        
        return false;
    }

    // Comparison operators

    template <typename T>
    bool Function<T>::operator==(const Function& rhs) const {
        if (!is_initialized() && !rhs.is_initialized()) { return true; }
        if (!is_initialized() || !rhs.is_initialized()) { return false; }
        
        // Test on a common interval [-10, 10] with 100 points

        const T test_a = -10, test_b = 10;
        const size_t test_n = 100;

        T h = (test_b - test_a) / test_n;
        
        for (size_t i = 0; i <= test_n; i++) {
            T x = test_a + i * h;
            T diff = std::abs((*this)(x) - rhs(x));

            if (diff > _eps) {
                return false;
            }
        }
        
        return true;
    }

    template <typename T>
    bool Function<T>::operator!=(const Function& rhs) const {
        return !(*this == rhs);
    }

    // Static methods

    template <typename T>
    Function<T> Function<T>::constant(T value) {
        return Function<T>(std::function<T(T)>([value](T x) { return value; }));
    }

    template <typename T>
    Function<T> Function<T>::linear(T intercept, T slope) {
        return Function<T>(std::function<T(T)>([slope, intercept](T x) { return slope * x + intercept; }));
    }

    template <typename T>
    Function<T> Function<T>::polynomial(const std::vector<T>& coefficients) {
        return Function<T>(std::function<T(T)>([coefficients](T x) {
            T result = 0;
            T power = 1;

            for (size_t i = 0; i < coefficients.size(); i++) {
                result += coefficients[i] * power;
                power *= x;
            }

            return result;
        }));
    }

    template <typename T>
    Function<T> Function<T>::exponential(T base) {
        return Function<T>(std::function<T(T)>([base](T x) { 
            return std::pow(base, x); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::logarithmic(T base) {
        return Function<T>(std::function<T(T)>([base](T x) { 
            if (x <= 0) {
                throw std::invalid_argument("Logarithm undefined for non-positive values");
            }

            return std::log(x) / std::log(base); 
        }));
    }

    template <typename T>
    Function<T> Function<T>::trigonometric_sin() {
        return Function<T>(std::function<T(T)>([](T x) { return std::sin(x); }));
    }

    template <typename T>
    Function<T> Function<T>::trigonometric_cos() {
        return Function<T>(std::function<T(T)>([](T x) { return std::cos(x); }));
    }

    template <typename T>
    Function<T> Function<T>::trigonometric_tan() {
        return Function<T>(std::function<T(T)>([](T x) { return std::tan(x); }));
    }

    template <typename T>
    Function<T> Function<T>::from_function_ptr(T (*func)(T)) {
        return Function<T>(std::function<T(T)>(func));
    }

    // Friend methods

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Function<T>& f) {
        if (f.is_initialized()) {
            os << "Function<" << typeid(T).name() << ">[initialized]";
        } else {
            os << "Function<" << typeid(T).name() << ">[uninitialized]";
        }

        return os;
    }

    // External methods

    template <typename T>
    Function<T> operator+(T scalar, const Function<T>& func) {
        return func + scalar;
    }

    template <typename T>
    Function<T> operator-(T scalar, const Function<T>& func) {
        return Function<T>::constant(scalar) - func;
    }

    template <typename T>
    Function<T> operator*(T scalar, const Function<T>& func) {
        return func * scalar;
    }

    template <typename T>
    Function<T> operator/(T scalar, const Function<T>& func) {
        return Function<T>::constant(scalar) / func;
    }
}
