// core/detail/_Vector.ipp


namespace vmafu {
    // Checks

    template <typename T>
    void Vector<T>::_check_index(size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Vector index out of range");
        }
    }

    template <typename T>
    void Vector<T>::_check_size_match(const Vector& rhs, const char* op) const {
        if (_size != rhs._size) {
            throw std::invalid_argument(std::string("Vector sizes must match for ") + op);
        }
    }

    template <typename T>
    void Vector<T>::_check_divisor(T scalar) const {
        if constexpr (std::is_floating_point_v<T>) {
            if (std::abs(scalar) < _eps) {
                throw std::invalid_argument("Division by zero");
            }
        } else {
            if (scalar == 0) {
                throw std::invalid_argument("Division by zero");
            }
        }
    }

    template <typename T>
    void Vector<T>::_check_non_empty(const char* op) const {
        if (_size == 0) {
            throw std::invalid_argument(std::string("Vector cannot be empty for ") + op);
        }
    }

    template <typename T>
    bool Vector<T>::_check_size_equal(const Vector& rhs) const noexcept {
        return _size == rhs._size;
    }

    template <typename T>
    bool Vector<T>::_check_size_almost_equal(const Vector& rhs) const noexcept {
        if (!_check_size_equal(rhs)) {
            return false;
        }

        for (size_t i = 0; i < _size; i++) {
            if constexpr (std::is_integral_v<T>) {
                if (_data[i] != rhs._data[i]) {
                    return false;
                }
            } else {
                if (std::abs(_data[i] - rhs._data[i]) > _eps) {
                    return false;
                }
            }
        }

        return true;
    }
    
    // Constructors/Destructor

    template <typename T>
    Vector<T>::Vector() = default;

    template <typename T>
    Vector<T>::Vector(size_t size) : _size(size) {
        _data = new T[_size]();
    }

    template <typename T>
    Vector<T>::Vector(size_t size, T init_val) : _size(size) {
        _data = new T[_size];
        std::fill(_data, _data + _size, init_val);
    }

    template <typename T>
    Vector<T>::Vector(std::initializer_list<T> init) 
        : _size(init.size()) {
        _data = new T[_size];
        std::copy(init.begin(), init.end(), _data);
    }

    template <typename T>
    Vector<T>::~Vector() {
        delete[] _data;
        _data = nullptr;
    }

    // Getters

    template <typename T>
    size_t Vector<T>::size() const noexcept {
        return _size;
    }

    template <typename T>
    double Vector<T>::eps() const noexcept {
        return _eps;
    }

    template <typename T>
    bool Vector<T>::empty() const noexcept {
        return _size == 0;
    }

    // Setters

    template <typename T>
    void Vector<T>::set_eps(double new_eps) { _eps = new_eps; }

    template <typename T>
    void Vector<T>::resize(size_t new_size, T value) {
        if (new_size == _size) {
            return;
        }

        T* new_data = new T[new_size]();
        size_t min_size = std::min(_size, new_size);
        
        for (size_t i = 0; i < min_size; i++) {
            new_data[i] = _data[i];
        }

        if (new_size > _size) {
            std::fill(new_data + _size, new_data + new_size, value);
        }

        delete[] _data;

        _data = new_data;
        _size = new_size;
    }

    template <typename T>
    void Vector<T>::swap(Vector& other) noexcept {
        std::swap(_data, other._data);
        std::swap(_size, other._size);
        std::swap(_eps, other._eps);
    }

    // Copy/Move operators

    template <typename T>
    Vector<T>::Vector(const Vector& other) 
        : _size(other._size), _eps(other._eps) {
        _data = new T[_size];
        std::copy(other._data, other._data + _size, _data);
    }

    template <typename T>
    Vector<T>::Vector(Vector&& other) noexcept 
        : _data(other._data), _size(other._size), _eps(other._eps) {
        
        other._data = nullptr;
        other._size = 0;
        other._eps = 1e-10;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other);
            swap(temp);
        }

        return *this;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] _data;

            _data = other._data;
            _size = other._size;
            _eps = other._eps;

            other._data = nullptr;
            other._size = 0;
            other._eps = 1e-10;
        }

        return *this;
    }

    // Access operators

    template <typename T>
    T& Vector<T>::operator[](size_t index) {
        _check_index(index);

        return _data[index];
    }

    template <typename T>
    const T& Vector<T>::operator[](size_t index) const {
        _check_index(index);

        return _data[index];
    }

    // Iterators

    template <typename T>
    T* Vector<T>::begin() noexcept { return _data; }

    template <typename T>
    T* Vector<T>::end() noexcept { return _data + _size; }

    template <typename T>
    const T* Vector<T>::begin() const noexcept { return _data; }

    template <typename T>
    const T* Vector<T>::end() const noexcept { return _data + _size; }

    // Arithmetic operators

    template <typename T>
    Vector<T> Vector<T>::operator+(const Vector& rhs) const {
        _check_size_match(rhs, "addition");

        return linear_combination(rhs, T{1}, T{1});
    }

    template <typename T>
    Vector<T> Vector<T>::operator-(const Vector& rhs) const {
        _check_size_match(rhs, "subtraction");

        return linear_combination(rhs, T{1}, T{-1});
    }

    template <typename T>
    Vector<T> Vector<T>::operator*(T scalar) const {
        Vector result(_size);
        for (size_t i = 0; i < _size; i++) {
            result._data[i] = _data[i] * scalar;
        }

        return result;
    }

    template <typename T>
    Vector<T> Vector<T>::operator/(T scalar) const {
        _check_divisor(scalar);

        Vector result(_size);
        for (size_t i = 0; i < _size; i++) {
            result._data[i] = _data[i] / scalar;
        }

        return result;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator+=(const Vector& rhs) {
        _check_size_match(rhs, "compound addition");

        for (size_t i = 0; i < _size; i++) {
            _data[i] += rhs._data[i];
        }

        return *this;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator-=(const Vector& rhs) {
        _check_size_match(rhs, "compound subtraction");

        for (size_t i = 0; i < _size; i++) {
            _data[i] -= rhs._data[i];
        }

        return *this;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator*=(T scalar) {
        for (size_t i = 0; i < _size; i++) {
            _data[i] *= scalar;
        }

        return *this;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator/=(T scalar) {
        _check_divisor(scalar);

        for (size_t i = 0; i < _size; i++) {
            _data[i] /= scalar;
        }

        return *this;
    }

    // Linear algebra operators

    template <typename T>
    T Vector<T>::sum() const {
        T total = T();
        for (size_t i = 0; i < _size; i++) {
            total += _data[i];
        }

        return total;
    }

    template <typename T>
    T Vector<T>::dot(const Vector& rhs) const {
        _check_size_match(rhs, "dot product");

        T result = T();
        for (size_t i = 0; i < _size; i++) {
            result += _data[i] * rhs._data[i];
        }

        return result;
    }

    template <typename T>
    double Vector<T>::norm() const {
        T sum_sq = T();
        for (size_t i = 0; i < _size; i++) {
            sum_sq += _data[i] * _data[i];
        }

        return std::sqrt(static_cast<double>(sum_sq));
    }

    template <typename T>
    T Vector<T>::norm_max() const {
        _check_non_empty("norm_max");

        T max_val = std::abs(_data[0]);
        for (size_t i = 1; i < _size; i++) {
            T abs_val = std::abs(_data[i]);
            if (abs_val > max_val) {
                max_val = abs_val;
            }
        }

        return max_val;
    }
    

    template <typename T>
    Vector<T> Vector<T>::normalized() const {
        _check_non_empty("normalization");

        double n = norm();
        if (n < _eps) {
            throw std::invalid_argument("Cannot normalize zero vector");
        }

        return *this / static_cast<T>(n);
    }

    template <typename T>
    Vector<T> Vector<T>::project_onto(const Vector& basis) const {
        _check_size_match(basis, "projection");

        T basis_sq_norm = basis.dot(basis);
        if (basis_sq_norm < static_cast<T>(_eps)) {
            throw std::invalid_argument("Zero basis vector");
        }

        T coefficient = dot(basis) / basis_sq_norm;

        return basis * coefficient;
    }

    template <typename T>
    bool Vector<T>::is_orthogonal_to(const Vector& other) const {
        if (!_check_size_equal(other)) {
            return false;
        }

        return std::abs(dot(other)) < _eps;
    }

    template <typename T>
    bool Vector<T>::is_collinear_with(const Vector& other) const {
        if (!_check_size_equal(other)) {
            return false;
        }

        if (norm() < _eps || other.norm() < _eps) {
            return true;
        }

        // |u x v|^2 = |u|^2|v|^2 - (u·v)^2

        double u_norm = norm();
        double v_norm = other.norm();
        double dot_product = dot(other);
        double cross_norm_sq = u_norm * u_norm * v_norm * v_norm - dot_product * dot_product;

        return std::abs(cross_norm_sq) < _eps;
    }

    template <typename T>
    Vector<T> Vector<T>::element_wise_multiply(const Vector& other) const {
        _check_size_match(other, "element-wise multiplication");

        Vector result(_size);
        for (size_t i = 0; i < _size; i++) {
            result._data[i] = _data[i] * other._data[i];
        }

        return result;
    }

    template <typename T>
    Vector<T> Vector<T>::linear_combination(const Vector& other, T alpha, T beta) const {
        _check_size_match(other, "linear combination");

        Vector result(_size);
        for (size_t i = 0; i < _size; i++) {
            result._data[i] = alpha * _data[i] + beta * other._data[i];
        }

        return result;
    }

    // Comparison operators

    template <typename T>
    bool Vector<T>::operator==(const Vector& rhs) const {
        return _check_size_almost_equal(rhs);
    }

    template <typename T>
    bool Vector<T>::operator!=(const Vector& rhs) const {
        return !_check_size_almost_equal(rhs);
    }

    // Static methods

    template <typename T>
    Vector<T> Vector<T>::unit_vector(size_t size, size_t index) {
        if (index >= size) {
            throw std::out_of_range("Unit vector index out of range");
        }

        Vector<T> result(size);
        result[index] = T{1};

        return result;
    }

    template <typename T>
    Vector<T> Vector<T>::from_function(size_t size, T (*func)(size_t)) {
        Vector<T> result(size);

        for (size_t i = 0; i < size; i++) {
            result[i] = func(i);
        }

        return result;
    }

    // Friend methods

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Vector<T>& v) {
        os << "Vector<" << typeid(T).name() << ">[" << v._size << "]: [";
        for (size_t i = 0; i < v._size; i++) {
            os << v._data[i];
            if (i + 1 < v._size) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    // External methods

    template <typename T>
    Vector<T> operator*(T scalar, const Vector<T>& vec) {
        return vec * scalar;
    }
}
