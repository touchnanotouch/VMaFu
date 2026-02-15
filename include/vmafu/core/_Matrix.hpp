// core/_Matrix.hpp


#pragma once


#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <type_traits>


namespace vmafu {
    namespace core {
        template <typename T>
        class Matrix {
            private:
                T* _data = nullptr;

                size_t _rows = 0;
                size_t _cols = 0;

                // Helper methods ( memory managment )

                void _allocate(size_t n);
                void _deallocate() noexcept;

            public:
                // Constructors / Destructor

                Matrix();
                explicit Matrix(size_t rows, size_t cols);
                Matrix(size_t rows, size_t cols, const T& init_value);
                Matrix(
                    std::initializer_list<std::initializer_list<T>> init_list
                );

                ~Matrix();

                // Getters

                T* data() noexcept;
                const T* data() const noexcept;

                size_t rows() const noexcept;
                size_t cols() const noexcept;

                size_t size() const noexcept;

                // Setters ( memory managment )

                void swap(Matrix& other_matrix) noexcept;

                void resize(size_t new_rows, size_t new_cols);
                void resize(
                    size_t new_rows,
                    size_t new_cols,
                    const T& fill_value
                );

                // Copy / Move operators

                Matrix(const Matrix& other_matrix);
                Matrix(Matrix&& other_matrix) noexcept;

                Matrix& operator=(const Matrix& other_matrix);
                Matrix& operator=(Matrix&& other_matrix) noexcept;

                // Access methods

                T& operator()(size_t row, size_t col) noexcept;
                const T& operator()(size_t row, size_t col) const noexcept;

                T& operator[](size_t index) noexcept;
                const T& operator[](size_t index) const noexcept;

                T& at(size_t row, size_t col);
                const T& at(size_t row, size_t col) const;

                // Iterators

                T* begin() noexcept;
                const T* begin() const noexcept;

                T* end() noexcept;
                const T* end() const noexcept;

                // Static methods

                static Matrix identity(size_t size);
                static Matrix zeros(size_t rows, size_t cols);
                static Matrix ones(size_t rows, size_t cols);
                static Matrix constant(
                    size_t rows,
                    size_t cols,
                    const T& fill_value
                );
                static Matrix from_function(
                    size_t rows,
                    size_t cols,
                    T (*func)(T, T)
                );
        };
    }
}


#include "detail/_Matrix.ipp"
