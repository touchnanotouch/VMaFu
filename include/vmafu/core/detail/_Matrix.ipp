// core/detail/_Matrix.ipp


namespace vmafu {
    namespace core {
        // Helper methods ( memory managment )

        template <typename T>
        void Matrix<T>::_allocate(size_t n) {
            if (n) {
                _data = new T[n]();
            }
        }

        template <typename T>
        void Matrix<T>::_deallocate() noexcept {
            delete[] _data;
            _data = nullptr;
        }

        // Constructors / Destructor

        template <typename T>
        Matrix<T>::Matrix() = default;

        template <typename T>
        Matrix<T>::Matrix(
            size_t rows,
            size_t cols
        ) : _rows(rows), _cols(cols) {
            _allocate(_rows * _cols);
        }

        template <typename T>
        Matrix<T>::Matrix(size_t rows, size_t cols, const T& init_value) 
        : _rows(rows), _cols(cols) {
            _allocate(_rows * _cols);
            std::fill(_data, _data + _rows * _cols, init_value);
        }

        template <typename T>
        Matrix<T>::Matrix(
            std::initializer_list<std::initializer_list<T>> init_list
        ) {
            _rows = init_list.size();

            if (_rows > 0) {
                _cols = init_list.begin()->size();

                _allocate(_rows * _cols);

                size_t i = 0;

                for (const auto& row : init_list) {
                    if (row.size() != _cols) {
                        _deallocate();

                        throw std::invalid_argument(
                            "Matrix::Matrix(): all rows must have same size"
                        );
                    }

                    try {
                        std::copy(row.begin(), row.end(), _data + i * _cols);
                    } catch (...) {
                        _deallocate();

                        throw;
                    }

                    i++;
                }
            }
        }

        template <typename T>
        Matrix<T>::~Matrix() {
            _deallocate();
        }

        // Getters

        template <typename T>
        T* Matrix<T>::data() noexcept {
            return _data;
        }

        template <typename T>
        const T* Matrix<T>::data() const noexcept {
            return _data;
        }

        template <typename T>
        size_t Matrix<T>::rows() const noexcept {
            return _rows;
        }

        template <typename T>
        size_t Matrix<T>::cols() const noexcept {
            return _cols;
        }

        template <typename T>
        size_t Matrix<T>::size() const noexcept {
            return _rows * _cols;
        }

        // Setters ( memory managment )

        template <typename T>
        void Matrix<T>::swap(Matrix& other_matrix) noexcept {
            std::swap(_data, other_matrix._data);
            std::swap(_rows, other_matrix._rows);
            std::swap(_cols, other_matrix._cols);
        }

        template <typename T>
        void Matrix<T>::resize(size_t new_rows, size_t new_cols) {
            if (new_rows == _rows && new_cols == _cols) {
                return;
            }

            T* new_data = nullptr;
            
            if (new_rows > 0 && new_cols > 0) {
                new_data = new T[new_rows * new_cols]();

                size_t min_rows = std::min(_rows, new_rows);
                size_t min_cols = std::min(_cols, new_cols);

                for (size_t i = 0; i < min_rows; i++) {
                    for (size_t j = 0; j < min_cols; j++) {
                        new_data[i * new_cols + j] = _data[i * _cols + j];
                    }
                }
            }

            _deallocate();

            _data = new_data;
            _rows = new_rows;
            _cols = new_cols;
        }

        template <typename T>
        void Matrix<T>::resize(
            size_t new_rows,
            size_t new_cols,
            const T& fill_value
        ) {
            if (new_rows == _rows && new_cols == _cols) {
                return;
            }

            T* new_data = nullptr;

            if (new_rows > 0 && new_cols > 0) {
                new_data = new T[new_rows * new_cols];

                size_t min_rows = std::min(_rows, new_rows);
                size_t min_cols = std::min(_cols, new_cols);

                for (size_t i = 0; i < min_rows; i++) {
                    for (size_t j = 0; j < min_cols; j++) {
                        new_data[i * new_cols + j] = _data[i * _cols + j];
                    }
                }

                for (size_t i = 0; i < new_rows; i++) {
                    for (size_t j = 0; j < new_cols; j++) {
                        if (i >= _rows || j >= _cols) {
                            new_data[i * new_cols + j] = fill_value;
                        }
                    }
                }
            }

            _deallocate();

            _data = new_data;
            _rows = new_rows;
            _cols = new_cols;
        }

        // Copy / Move operators

        template <typename T>
        Matrix<T>::Matrix(const Matrix& other_matrix)
        : _rows(other_matrix._rows), _cols(other_matrix._cols) {
            _allocate(_rows * _cols);
            std::copy(
                other_matrix._data, other_matrix._data + _rows * _cols, _data
            );
        }

        template <typename T>
        Matrix<T>::Matrix(Matrix&& other_matrix) noexcept
        : _data(other_matrix._data), _rows(other_matrix._rows), \
          _cols(other_matrix._cols) {
            other_matrix._data = nullptr;
            other_matrix._rows = 0;
            other_matrix._cols = 0;
        }

        template <typename T>
        Matrix<T>& Matrix<T>::operator=(const Matrix& other_matrix) {
            if (this != &other_matrix) {
                Matrix temp(other_matrix);
                swap(temp);
            }

            return *this;
        }

        template <typename T>
        Matrix<T>& Matrix<T>::operator=(Matrix&& other_matrix) noexcept {
            if (this != &other_matrix) {
                _deallocate();

                _data = other_matrix._data;
                _rows = other_matrix._rows;
                _cols = other_matrix._cols;

                other_matrix._data = nullptr;
                other_matrix._rows = 0;
                other_matrix._cols = 0;
            }

            return *this;
        }

        // Access methods

        template <typename T>
        T& Matrix<T>::operator()(size_t row, size_t col) noexcept {
            return _data[row * _cols + col];
        }

        template <typename T>
        const T& Matrix<T>::operator()(size_t row, size_t col) const noexcept {
            return _data[row * _cols + col];
        }

        template <typename T>
        T& Matrix<T>::operator[](size_t index) noexcept {
            return _data[index];
        }

        template <typename T>
        const T& Matrix<T>::operator[](size_t index) const noexcept {
            return _data[index];
        }

        template <typename T>
        T& Matrix<T>::at(size_t row, size_t col) {
            if (row >= _rows || col >= _cols) {
                throw std::out_of_range("Matrix::at(): index out of range");
            }

            return _data[row * _cols + col];
        }

        template <typename T>
        const T& Matrix<T>::at(size_t row, size_t col) const {
            if (row >= _rows || col >= _cols) {
                throw std::out_of_range("Matrix::at(): index out of range");
            }

            return _data[row * _cols + col];
        }

        // Iterators

        template <typename T>
        T* Matrix<T>::begin() noexcept {
            return _data;
        }

        template <typename T>
        const T* Matrix<T>::begin() const noexcept {
            return _data;
        }

        template <typename T>
        T* Matrix<T>::end() noexcept {
            return _data + _rows * _cols;
        }

        template <typename T>
        const T* Matrix<T>::end() const noexcept {
            return _data + _rows * _cols;
        }

        // Static methods

        template <typename T>
        Matrix<T> Matrix<T>::identity(size_t n) {
            Matrix result(n, n);
            for (size_t i = 0; i < n; i++) {
                result(i, i) = T{1};
            }

            return result;
        }

        template <typename T>
        Matrix<T> Matrix<T>::zeros(size_t rows, size_t cols) {
            return Matrix(rows, cols);
        }

        template <typename T>
        Matrix<T> Matrix<T>::ones(size_t rows, size_t cols) {
            return Matrix(rows, cols, T{1});
        }

        template <typename T>
        Matrix<T> Matrix<T>::constant(
            size_t rows,
            size_t cols,
            const T& fill_value
        ) {
            return Matrix(rows, cols, fill_value);
        }

        template <typename T>
        Matrix<T> Matrix<T>::from_function(
            size_t rows,
            size_t cols,
            T (*func)(T, T)
        ) {
            Matrix result(rows, cols);
            for (size_t i = 0; i < rows; i++) {
                for (size_t j = 0; j < cols; j++) {
                    result(i, j) = func(i, j);
                }
            }

            return result;
        }
    }
}
