#include "vmafu/core/Vector.hpp"


namespace vmafu {
    // Checks
    
    template <typename T>
    void Matrix<T>::_check_index(size_t row, size_t col) const {
        if (row >= _rows || col >= _cols) {
            throw std::out_of_range("Matrix index out of range");
        }
    }
    
    template <typename T>
    std::pair<size_t, size_t> Matrix<T>::_check_linear_index(size_t index) const {
        if (index >= _rows * _cols) {
            throw std::out_of_range("Matrix linear index out of range");
        }

        return {index / _cols, index % _cols};
    }

    template <typename T>
    void Matrix<T>::_check_size_match(const Matrix& rhs, const char* op) const {
        if (_rows != rhs._rows || _cols != rhs._cols) {
            throw std::invalid_argument(std::string("Matrix sizes must match for ") + op);
        }
    }
    
    template <typename T>
    void Matrix<T>::_check_square(const char* op) const {
        if (!is_square()) {
            throw std::invalid_argument(std::string("Matrix must be square for ") + op);
        }
    }
    
    template <typename T>
    void Matrix<T>::_check_mult_size(const Matrix& rhs, const char* op) const {
        if (_cols != rhs._rows) {
            throw std::invalid_argument(std::string("Matrix dimensions incompatible for ") + op);
        }
    }
    
    template <typename T>
    void Matrix<T>::_check_divisor(T scalar) const {
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
    
    // Constructors/Destructor
    
    template <typename T>
    Matrix<T>::Matrix() = default;
    
    template <typename T>
    Matrix<T>::Matrix(size_t rows, size_t cols) : _rows(rows), _cols(cols) {
        _data = new T[_rows * _cols]();
    }
    
    template <typename T>
    Matrix<T>::Matrix(size_t rows, size_t cols, T init_val) : _rows(rows), _cols(cols) {
        _data = new T[_rows * _cols];
        std::fill(_data, _data + _rows * _cols, init_val);
    }
    
    template <typename T>
    Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> init) {
        _rows = init.size();

        if (_rows > 0) {
            _cols = init.begin()->size();
            _data = new T[_rows * _cols];
            
            size_t i = 0;
            for (const auto& row : init) {
                if (row.size() != _cols) {
                    delete[] _data;
                    throw std::invalid_argument("All rows must have same size in initializer list");
                }

                std::copy(row.begin(), row.end(), _data + i * _cols);

                i++;
            }
        }
    }
    
    template <typename T>
    Matrix<T>::~Matrix() {
        delete[] _data;
        _data = nullptr;
    }
    
    // Getters
    
    template <typename T>
    size_t Matrix<T>::_flat_index(size_t row, size_t col) const noexcept {
        return row * _cols + col;
    }

    template <typename T>
    size_t Matrix<T>::rows() const noexcept { return _rows; }
    
    template <typename T>
    size_t Matrix<T>::cols() const noexcept { return _cols; }
    
    template <typename T>
    size_t Matrix<T>::size() const noexcept { return _rows * _cols; }
    
    template <typename T>
    double Matrix<T>::eps() const noexcept { return _eps; }
    
    template <typename T>
    bool Matrix<T>::empty() const noexcept { return _rows == 0 || _cols == 0; }
    
    template <typename T>
    bool Matrix<T>::is_square() const noexcept { return _rows == _cols; }
    
    // Setters

    template <typename T>
    void Matrix<T>::set_eps(double new_eps) { 
        _eps = new_eps; 
    }

    template <typename T>
    void Matrix<T>::resize(size_t new_rows, size_t new_cols, T value) {
        if (new_rows == _rows && new_cols == _cols) {
            return;
        }
        
        T* new_data = new T[new_rows * new_cols]();
        size_t min_rows = std::min(_rows, new_rows);
        size_t min_cols = std::min(_cols, new_cols);
        
        for (size_t i = 0; i < min_rows; i++) {
            for (size_t j = 0; j < min_cols; j++) {
                new_data[i * new_cols + j] = _data[_flat_index(i, j)];
            }
        }

        if (new_rows > _rows || new_cols > _cols) {
            for (size_t i = 0; i < new_rows; i++) {
                for (size_t j = 0; j < new_cols; j++) {
                    if (i >= _rows || j >= _cols) {
                        new_data[i * new_cols + j] = value;
                    }
                }
            }
        }
        
        delete[] _data;

        _data = new_data;
        _rows = new_rows;
        _cols = new_cols;
    }

    template <typename T>
    void Matrix<T>::swap(Matrix& other) noexcept {
        std::swap(_data, other._data);
        std::swap(_rows, other._rows);
        std::swap(_cols, other._cols);
        std::swap(_eps, other._eps);
    }

    // Copy/Move operators

    template <typename T>
    Matrix<T>::Matrix(const Matrix& other) 
        : _rows(other._rows), _cols(other._cols), _eps(other._eps) {
        _data = new T[_rows * _cols];
        std::copy(other._data, other._data + _rows * _cols, _data);
    }

    template <typename T>
    Matrix<T>::Matrix(Matrix&& other) noexcept 
        : _data(other._data), _rows(other._rows), _cols(other._cols), _eps(other._eps) {
        
        other._data = nullptr;
        other._rows = 0;
        other._cols = 0;
        other._eps = 1e-10;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
        if (this != &other) {
            Matrix temp(other);
            swap(temp);
        }
        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator=(Matrix&& other) noexcept {
        if (this != &other) {
            delete[] _data;
            
            _data = other._data;
            _rows = other._rows;
            _cols = other._cols;
            _eps = other._eps;
            
            other._data = nullptr;
            other._rows = 0;
            other._cols = 0;
            other._eps = 1e-10;
        }
        return *this;
    }

    // Access operators
    
    template <typename T>
    T& Matrix<T>::operator()(size_t row, size_t col) {
        _check_index(row, col);

        return _data[_flat_index(row, col)];
    }

    template <typename T>
    const T& Matrix<T>::operator()(size_t row, size_t col) const {
        _check_index(row, col);

        return _data[_flat_index(row, col)];
    }

    template <typename T>
    T& Matrix<T>::operator[](size_t index) {
        auto [row, col] = _check_linear_index(index);

        return _data[_flat_index(row, col)];
    }

    template <typename T>
    const T& Matrix<T>::operator[](size_t index) const {
        auto [row, col] = _check_linear_index(index);

        return _data[_flat_index(row, col)];
    }

    template <typename T>
    T& Matrix<T>::at(size_t row, size_t col) {
        _check_index(row, col);

        return _data[_flat_index(row, col)];
    }

    template <typename T>
    const T& Matrix<T>::at(size_t row, size_t col) const {
        _check_index(row, col);

        return _data[_flat_index(row, col)];
    }
    
    // Row/Col access
    
    template <typename T>
    Vector<T> Matrix<T>::row(size_t index) const {
        _check_index(index, 0);
        
        Vector<T> result(_cols);
        for (size_t j = 0; j < _cols; j++) {
            result[j] = _data[_flat_index(index, j)];
        }
    
        return result;
    }
    
    template <typename T>
    Vector<T> Matrix<T>::col(size_t index) const {
        _check_index(0, index);
        
        Vector<T> result(_rows);
        for (size_t i = 0; i < _rows; i++) {
            result[i] = _data[_flat_index(i, index)];
        }

        return result;
    }
    
    template <typename T>
    void Matrix<T>::set_row(size_t index, const Vector<T>& vec) {
        _check_index(index, 0);

        if (vec.size() != _cols) {
            throw std::invalid_argument("Vector size must match matrix columns");
        }
        
        for (size_t j = 0; j < _cols; j++) {
            _data[_flat_index(index, j)] = vec[j];
        }
    }

    template <typename T>
    void Matrix<T>::set_col(size_t index, const Vector<T>& vec) {
        _check_index(0, index);

        if (vec.size() != _rows) {
            throw std::invalid_argument("Vector size must match matrix rows");
        }
        
        for (size_t i = 0; i < _rows; i++) {
            _data[_flat_index(i, index)] = vec[i];
        }
    }

    // Iterators

    template <typename T>
    T* Matrix<T>::begin() noexcept { 
        return _data; 
    }

    template <typename T>
    T* Matrix<T>::end() noexcept { 
        return _data + _rows * _cols; 
    }

    template <typename T>
    const T* Matrix<T>::begin() const noexcept { 
        return _data; 
    }

    template <typename T>
    const T* Matrix<T>::end() const noexcept { 
        return _data + _rows * _cols; 
    }

    // Arithmetic operators
    
    template <typename T>
    Matrix<T> Matrix<T>::operator+(const Matrix& rhs) const {
        _check_size_match(rhs, "addition");
        
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            result._data[i] = _data[i] + rhs._data[i];
        }

        return result;
    }
    
    template <typename T>
    Matrix<T> Matrix<T>::operator-(const Matrix& rhs) const {
        _check_size_match(rhs, "subtraction");
        
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            result._data[i] = _data[i] - rhs._data[i];
        }

        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator*(T scalar) const {
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            result._data[i] = _data[i] * scalar;
        }
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator/(T scalar) const {
        _check_divisor(scalar);
        
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            result._data[i] = _data[i] / scalar;
        }
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::operator*(const Matrix& rhs) const {
        _check_mult_size(rhs, "multiplication");
        
        Matrix result(_rows, rhs._cols);
        for (size_t i = 0; i < _rows; i++) {
            for (size_t j = 0; j < rhs._cols; j++) {
                T sum = T();
                for (size_t k = 0; k < _cols; k++) {
                    sum += (*this)(i, k) * rhs(k, j);
                }
                result(i, j) = sum;
            }
        }

        return result;
    }
    
    template <typename T>
    Vector<T> Matrix<T>::operator*(const Vector<T>& vec) const {
        if (_cols != vec.size()) {
            throw std::invalid_argument("Matrix columns must match vector size for multiplication");
        }
        
        Vector<T> result(_rows);
        for (size_t i = 0; i < _rows; i++) {
            T sum = T();
            for (size_t j = 0; j < _cols; j++) {
                sum += (*this)(i, j) * vec[j];
            }
            result[i] = sum;
        }
        return result;
    }
    
    template <typename T>
    Matrix<T>& Matrix<T>::operator+=(const Matrix& rhs) {
        _check_size_match(rhs, "compound addition");
        
        for (size_t i = 0; i < _rows * _cols; i++) {
            _data[i] += rhs._data[i];
        }

        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator-=(const Matrix& rhs) {
        _check_size_match(rhs, "compound subtraction");
        
        for (size_t i = 0; i < _rows * _cols; i++) {
            _data[i] -= rhs._data[i];
        }

        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator*=(T scalar) {
        for (size_t i = 0; i < _rows * _cols; i++) {
            _data[i] *= scalar;
        }

        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator/=(T scalar) {
        _check_divisor(scalar);
        
        for (size_t i = 0; i < _rows * _cols; i++) {
            _data[i] /= scalar;
        }

        return *this;
    }

    // Linear algebra operators

    template <typename T>
    T Matrix<T>::trace() const {
        _check_square("trace");
        
        T result = T();
        for (size_t i = 0; i < _rows; i++) {
            result += _data[_flat_index(i, i)];
        }

        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::transpose() const {
        Matrix result(_cols, _rows);
        for (size_t i = 0; i < _rows; i++) {
            for (size_t j = 0; j < _cols; j++) {
                result(j, i) = (*this)(i, j);
            }
        }

        return result;
    }

    template <typename T>
    double Matrix<T>::norm() const {
        double sum_sq = 0.0;
        for (size_t i = 0; i < _rows * _cols; i++) {
            sum_sq += static_cast<double>(_data[i] * _data[i]);
        }

        return std::sqrt(sum_sq);
    }

    template <typename T>
    T Matrix<T>::norm_max() const {
        if (empty()) {
            throw std::invalid_argument("Cannot compute norm of empty matrix");
        }
        
        T max_val = std::abs(_data[0]);
        for (size_t i = 1; i < _rows * _cols; i++) {
            T abs_val = std::abs(_data[i]);
            if (abs_val > max_val) {
                max_val = abs_val;
            }
        }

        return max_val;
    }

    template <typename T>
    Matrix<T> Matrix<T>::normalized() const {
        double n = norm();
        if (n < _eps) {
            throw std::invalid_argument("Cannot normalize zero matrix");
        }

        return *this / static_cast<T>(n);
    }

    template <typename T>
    bool Matrix<T>::is_orthogonal() const {
        _check_square("orthogonality check");
        
        Matrix product = (*this) * transpose();
        Matrix identity = Matrix::identity(_rows);
        
        return product == identity;
    }

    template <typename T>
    bool Matrix<T>::is_symmetric() const {
        if (!is_square()) {
            return false;
        }
        
        for (size_t i = 0; i < _rows; i++) {
            for (size_t j = i + 1; j < _cols; j++) {
                if (std::abs((*this)(i, j) - (*this)(j, i)) > _eps) {
                    return false;
                }
            }
        }

        return true;
    }

    template <typename T>
    bool Matrix<T>::is_diagonal() const {
        if (!is_square()) {
            return false;
        }
        
        for (size_t i = 0; i < _rows; i++) {
            for (size_t j = 0; j < _cols; j++) {
                if (i != j && std::abs((*this)(i, j)) > _eps) {
                    return false;
                }
            }
        }

        return true;
    }

    // Element-wise operations

    template <typename T>
    Matrix<T> Matrix<T>::element_wise_multiply(const Matrix& other) const {
        _check_size_match(other, "element-wise multiplication");
        
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            result._data[i] = _data[i] * other._data[i];
        }
    
        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::element_wise_divide(const Matrix& other) const {
        _check_size_match(other, "element-wise division");
        
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows * _cols; i++) {
            _check_divisor(other._data[i]);
            result._data[i] = _data[i] / other._data[i];
        }

        return result;
    }

    // Comparison operators

    template <typename T>
    bool Matrix<T>::operator==(const Matrix& rhs) const {
        if (_rows != rhs._rows || _cols != rhs._cols) {
            return false;
        }
        
        for (size_t i = 0; i < _rows * _cols; i++) {
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

    template <typename T>
    bool Matrix<T>::operator!=(const Matrix& rhs) const {
        return !(*this == rhs);
    }

    // Static methods
    
    template <typename T>
    Matrix<T> Matrix<T>::identity(size_t size) {
        Matrix result(size, size);
        for (size_t i = 0; i < size; i++) {
            result(i, i) = T{1};
        }

        return result;
    }
    
    template <typename T>
    Matrix<T> Matrix<T>::zeros(size_t rows, size_t cols) {
        return Matrix<T>(rows, cols, T{0});
    }
    
    template <typename T>
    Matrix<T> Matrix<T>::ones(size_t rows, size_t cols) {
        return Matrix<T>(rows, cols, T{1});
    }

    template <typename T>
    Matrix<T> Matrix<T>::diagonal(const Vector<T>& diag) {
        size_t size = diag.size();

        Matrix result(size, size);
        for (size_t i = 0; i < size; i++) {
            result(i, i) = diag[i];
        }

        return result;
    }

    template <typename T>
    Matrix<T> Matrix<T>::from_function(size_t rows, size_t cols, T (*func)(size_t, size_t)) {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result(i, j) = func(i, j);
            }
        }

        return result;
    }

    // Friend methods
    
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
        os << "Matrix<" << typeid(T).name() << ">[" << m._rows << "x" << m._cols << "]:\n";
        for (size_t i = 0; i < m._rows; i++) {
            os << "[";
            for (size_t j = 0; j < m._cols; j++) {
                os << m(i, j);
                if (j + 1 < m._cols) {
                    os << ", ";
                }
            }
            os << "]" << (i + 1 < m._rows ? "\n" : "");
        }

        return os;
    }
    
    // External methods
    
    template <typename T>
    Matrix<T> operator*(T scalar, const Matrix<T>& mat) {
        return mat * scalar;
    }
    
    template <typename T>
    Vector<T> operator*(const Vector<T>& vec, const Matrix<T>& mat) {
        if (vec.size() != mat.rows()) {
            throw std::invalid_argument("Vector size must match matrix rows for multiplication");
        }
        
        Vector<T> result(mat.cols());
        for (size_t j = 0; j < mat.cols(); j++) {
            T sum = T();
            for (size_t i = 0; i < mat.rows(); i++) {
                sum += vec[i] * mat(i, j);
            }
            result[j] = sum;
        }

        return result;
    }
}
