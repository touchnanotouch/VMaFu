// linalg/operations.hpp


#pragma once


#include <stdexcept>
#include <cmath>

#include "../core/_Vector.hpp"
#include "../core/_Matrix.hpp"
#include "../utils/_compat.hpp"


namespace vmafu {
    namespace linalg {
        // Vector-Vector operations

        // Vector addition

        template <typename T>
        Vector<T> operator+(const Vector<T>& lhs, const Vector<T>& rhs);

        // Vector subtraction

        template <typename T>
        Vector<T> operator-(const Vector<T>& lhs, const Vector<T>& rhs);

        // Unary minus (negation)
        
        template <typename T>
        Vector<T> operator-(const Vector<T>& vector);

        // Vector-Scalar operations

        // Vector * scalar

        template <typename T>
        Vector<T> operator*(const Vector<T>& vector, T scalar);

        // scalar * Vector

        template <typename T>
        Vector<T> operator*(T scalar, const Vector<T>& vector);

        // Vector / scalar

        template <typename T>
        Vector<T> operator/(const Vector<T>& vector, T scalar);

        // Vector + scalar

        template <typename T>
        Vector<T> operator+(const Vector<T>& vector, T scalar);

        // scalar + Vector

        template <typename T>
        Vector<T> operator+(T scalar, const Vector<T>& vector);

        // Vector - scalar

        template <typename T>
        Vector<T> operator-(const Vector<T>& vector, T scalar);

        // scalar - Vector

        template <typename T>
        Vector<T> operator-(T scalar, const Vector<T>& vector);

        //  Matrix-Matrix operations

        // Matrix addition

        template <typename T>
        Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs);

        // Matrix subtraction

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs);

        // Unary minus (negation)

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& m);

        // Matrix multiplication

        template <typename T>
        Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs);

        //  Matrix-Scalar operations

        // Matrix * scalar

        template <typename T>
        Matrix<T> operator*(const Matrix<T>& matrix, T scalar);

        // scalar * Matrix

        template <typename T>
        Matrix<T> operator*(T scalar, const Matrix<T>& matrix);

        // Matrix / scalar

        template <typename T>
        Matrix<T> operator/(const Matrix<T>& m, T scalar);

        // Matrix + scalar

        template <typename T>
        Matrix<T> operator+(const Matrix<T>& matrix, T scalar);

        // scalar + Matrix

        template <typename T>
        Matrix<T> operator+(T scalar, const Matrix<T>& matrix);

        // Matrix - scalar

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& matrix, T scalar);

        // scalar - Matrix

        template <typename T>
        Matrix<T> operator-(T scalar, const Matrix<T>& matrix);

        // Matrix-Vector operations

        // Matrix * Vector (matrix-vector multiplication)

        template <typename T>
        Vector<T> operator*(const Matrix<T>& matrix, const Vector<T>& vector);

        // Vector * Matrix (vector-matrix multiplication, result is row vector)

        template <typename T>
        Vector<T> operator*(const Vector<T>& vector, const Matrix<T>& matrix);
    }
}


#include "detail/_operations.ipp"
