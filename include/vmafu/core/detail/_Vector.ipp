// core/detail/_Vector.ipp


namespace vmafu {
    namespace core {
        // Helper methods ( memory managment )

        template <typename T>
        void Vector<T>::_allocate(size_t n) {
            if (n) {
                _data = new T[n]();
            }
        }

        template <typename T>
        void Vector<T>::_deallocate() noexcept {
            delete[] _data;
            _data = nullptr;
        }

        // Constructors / Destructor

        template <typename T>
        Vector<T>::Vector() = default;

        template <typename T>
        Vector<T>::Vector(size_t size) : _size(size) {
            _allocate(_size);
        }

        template <typename T>
        Vector<T>::Vector(size_t size, const T& init_value) : _size(size) {
            _allocate(_size);
            std::fill(_data, _data + _size, init_value);
        }

        template <typename T>
        Vector<T>::Vector(
            std::initializer_list<T> init_list
        ) : _size(init_list.size()) {
            _allocate(_size);
            std::copy(init_list.begin(), init_list.end(), _data);
        }

        template <typename T>
        Vector<T>::~Vector() {
            _deallocate();
        }

        // Getters

        template <typename T>
        T* Vector<T>::data() noexcept {
            return _data;
        }

        template <typename T>
        const T* Vector<T>::data() const noexcept {
            return _data;
        }

        template <typename T>
        size_t Vector<T>::size() const noexcept {
            return _size;
        }

        // Setters ( memory managment )

        template <typename T>
        void Vector<T>::swap(Vector& other_vector) noexcept {
            std::swap(_data, other_vector._data);
            std::swap(_size, other_vector._size);
        };

        template <typename T>
        void Vector<T>::resize(size_t new_size) {
            if (new_size == _size) {
                return;
            }
        
            T* new_data = nullptr;

            if (new_size > 0) {
                new_data = new T[new_size]();

                size_t min_size = std::min(_size, new_size);

                for (size_t i = 0; i < min_size; i++) {
                    new_data[i] = _data[i];
                }
            }
            
            _deallocate();

            _data = new_data;
            _size = new_size;
        }

        template <typename T>
        void Vector<T>::resize(size_t new_size, T fill_value) {
            if (new_size == _size) {
                return;
            }
        
            T* new_data = nullptr;

            if (new_size > 0) {
                new_data = new T[new_size];

                size_t min_size = std::min(_size, new_size);

                for (size_t i = 0; i < min_size; i++) {
                    new_data[i] = _data[i];
                }

                if (new_size > _size) {
                    std::fill(
                        new_data + _size, new_data + new_size, fill_value
                    );
                }
            }
            
            _deallocate();

            _data = new_data;
            _size = new_size;
        }

        // Copy / Move operators

        template <typename T>
        Vector<T>::Vector(
            const Vector& other_vector
        ) : _size(other_vector._size) {
            _allocate(_size);
            std::copy(other_vector._data, other_vector._data + _size, _data);
        }

        template <typename T>
        Vector<T>::Vector(Vector&& other_vector) noexcept
        : _data(other_vector._data), _size(other_vector._size) {
            other_vector._data = nullptr;
            other_vector._size = 0;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator=(const Vector& other_vector) {
            if (this != &other_vector) {
                Vector temp(other_vector);
                swap(temp);
            }

            return *this;
        }

        template <typename T>
        Vector<T>& Vector<T>::operator=(Vector&& other_vector) noexcept {
            if (this != &other_vector) {
                _deallocate();

                _data = other_vector._data;
                _size = other_vector._size;

                other_vector._data = nullptr;
                other_vector._size = 0;
            }

            return *this;
        }

        // Access methods

        template <typename T>
        T& Vector<T>::operator[](size_t index) noexcept {
            return _data[index];
        }

        template <typename T>
        const T& Vector<T>::operator[](size_t index) const noexcept {
            return _data[index];
        }

        template <typename T>
        T& Vector<T>::at(size_t index) {
            if (index >= _size) {
                throw std::out_of_range("Vector::at(): index out of range");
            }

            return _data[index];
        }

        template <typename T>
        const T& Vector<T>::at(size_t index) const {
            if (index >= _size) {
                throw std::out_of_range("Vector::at(): index out of range");
            }

            return _data[index];
        }

        template <typename T>
        T& Vector<T>::front() noexcept {
            return _data[0];
        }

        template <typename T>
        const T& Vector<T>::front() const noexcept {
            return _data[0];
        }

        template <typename T>
        T& Vector<T>::back() noexcept {
            return _data[_size - 1];
        }

        template <typename T>
        const T& Vector<T>::back() const noexcept {
            return _data[_size - 1];
        }

        // Iterators

        template <typename T>
        T* Vector<T>::begin() noexcept {
            return _data;
        }

        template <typename T>
        const T* Vector<T>::begin() const noexcept {
            return _data;
        }

        template <typename T>
        T* Vector<T>::end() noexcept {
            return _data + _size;
        }

        template <typename T>
        const T* Vector<T>::end() const noexcept {
            return _data + _size;
        }

        // Static methods

        template <typename T>
        Vector<T> Vector<T>::unit(size_t size, size_t index) {
            if (index >= size) {
                throw std::out_of_range("Vector::unit(): index out of range");
            }

            Vector<T> result(size);
            result[index] = T{1};

            return result;
        }

        template <typename T>
        Vector<T> Vector<T>::zeros(size_t size) {
            return Vector(size);
        }

        template <typename T>
        Vector<T> Vector<T>::ones(size_t size) {
            return Vector(size, T{1});
        }

        template <typename T>
        Vector<T> Vector<T>::constant(size_t size, const T& fill_value) {
            return Vector(size, fill_value);
        }

        template <typename T>
        Vector<T> Vector<T>::from_function(size_t size, T (*func)(T)) {
                Vector result(size);
                for (size_t i = 0; i < size; i++) {
                    result[i] = func(i);
                }

                return result;
            }
    }
}
