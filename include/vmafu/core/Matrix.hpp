#pragma once


#include <algorithm>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <initializer_list>


// Forward declarations

namespace vmafu {
    template <typename T> class Vector;
}

namespace vmafu {
    // Main class

    template <typename T>
    class Matrix {
        private:
            T* _data = nullptr;

            size_t _rows = 0;
            size_t _cols = 0;
            
            double _eps = 1e-10;

            // Checks

            void _check_index(size_t row, size_t col) const;
            std::pair<size_t, size_t> _check_linear_index(size_t index) const;
            void _check_size_match(const Matrix& rhs, const char* op) const;
            void _check_square(const char* op) const;
            void _check_mult_size(const Matrix& rhs, const char* op) const;
            void _check_divisor(T scalar) const;

            // Helper methods

            T _determinant_gauss() const;

            Matrix<T> _inverse_gauss_jordan() const;

            std::pair<Matrix<T>, Matrix<T>> _lu_decomposition() const;
            std::pair<Matrix<T>, Matrix<T>> _qr_decomposition() const;

        public:
            // Constructors/Destructor

            Matrix();
            Matrix(size_t rows, size_t cols);
            Matrix(size_t rows, size_t cols, T init_val);
            Matrix(std::initializer_list<std::initializer_list<T>> init);

            ~Matrix();

            // Getters

            size_t _flat_index(size_t row, size_t col) const noexcept;

            size_t rows() const noexcept;
            size_t cols() const noexcept;
            size_t size() const noexcept;

            double eps() const noexcept;

            bool empty() const noexcept;
            bool is_square() const noexcept;

            // Setters

            void set_eps(double new_eps);
            void resize(size_t new_rows, size_t new_cols, T value = T());
            void swap(Matrix& other) noexcept;

            // Copy/Move operators

            Matrix(const Matrix& other);
            Matrix(Matrix&& other) noexcept;
            Matrix& operator=(const Matrix& other);
            Matrix& operator=(Matrix&& other) noexcept;

            // Access operators

            T& operator()(size_t row, size_t col);
            const T& operator()(size_t row, size_t col) const;

            T& operator[](size_t index);
            const T& operator[](size_t index) const;

            T& at(size_t row, size_t col);
            const T& at(size_t row, size_t col) const;

            // Row/Col access

            Vector<T> row(size_t index) const;
            Vector<T> col(size_t index) const;
            void set_row(size_t index, const Vector<T>& vec);
            void set_col(size_t index, const Vector<T>& vec);

            // Iterators

            T* begin() noexcept;
            T* end() noexcept;
            const T* begin() const noexcept;
            const T* end() const noexcept;

            // Arithmetic operators

            Matrix operator+(const Matrix& rhs) const;
            Matrix operator-(const Matrix& rhs) const;
            Matrix operator*(T scalar) const;
            Matrix operator/(T scalar) const;
            Matrix operator*(const Matrix& rhs) const;  // Matrix multiplication
            Vector<T> operator*(const Vector<T>& vec) const;  // Matrix-vector multiplication

            Matrix& operator+=(const Matrix& rhs);
            Matrix& operator-=(const Matrix& rhs);
            Matrix& operator*=(T scalar);
            Matrix& operator/=(T scalar);

            // Decomposition types

            enum class DecompositionMethod {
                LU,
                QR,
                GAUSS
            };

            // Linear algebra operators

            T trace() const;
            double condition_number() const;
            
            Matrix transpose() const;

            T determinant() const;
            Matrix inverse() const;

            T determinant(DecompositionMethod method) const;
            Matrix inverse(DecompositionMethod method) const;
            
            T determinant_lu() const;
            T determinant_qr() const;
            Matrix inverse_lu() const; 
            Matrix inverse_qr() const;
            
            DecompositionMethod recommended_method() const;
            
            double norm() const;
            T norm_max() const;
            
            Matrix normalized() const;

            bool is_orthogonal() const;
            bool is_symmetric() const;
            bool is_diagonal() const;
            bool is_well_conditioned() const;

            // Element-wise methods

            Matrix element_wise_multiply(const Matrix& other) const;
            Matrix element_wise_divide(const Matrix& other) const;

            // Comparison operators

            bool operator==(const Matrix& rhs) const;
            bool operator!=(const Matrix& rhs) const;

            // Static methods

            static Matrix identity(size_t size);
            static Matrix zeros(size_t rows, size_t cols);
            static Matrix ones(size_t rows, size_t cols);
            static Matrix diagonal(const Vector<T>& diag);
            static Matrix from_function(size_t rows, size_t cols, T (*func)(size_t, size_t));

            // Friend methods

            template <typename U>
            friend std::ostream& operator<<(std::ostream& os, const Matrix<U>& m);
    };

    // External methods

    template <typename T>
    Matrix<T> operator*(T scalar, const Matrix<T>& mat);
    
    template <typename T>
    Vector<T> operator*(const Vector<T>& vec, const Matrix<T>& mat);
}


#include "detail/Matrix.ipp"
