// linalg/detail/_operations.ipp


namespace vmafu {
    namespace linalg {
        // Vector-Vector operations

        template <typename T>
        Vector<T> operator+(const Vector<T>& lhs, const Vector<T>& rhs) {
            if (lhs.size() != rhs.size()) {
                throw std::invalid_argument(
                    "linalg::operator+: Vector sizes must match"
                );
            }

            Vector<T> result(lhs.size());
            for (size_t i = 0; i < lhs.size(); i++) {
                result[i] = lhs[i] + rhs[i];
            }

            return result;
        }

        template <typename T>
        Vector<T> operator-(const Vector<T>& lhs, const Vector<T>& rhs) {
            if (lhs.size() != rhs.size()) {
                throw std::invalid_argument(
                    "linalg::operator-: Vector sizes must match"
                );
            }

            Vector<T> result(lhs.size());
            for (size_t i = 0; i < lhs.size(); i++) {
                result[i] = lhs[i] - rhs[i];
            }

            return result;
        }

        template <typename T>
        Vector<T> operator-(const Vector<T>& vector) {
            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = -vector[i];
            }

            return result;
        }

        //  Vector-Scalar operations

        template <typename T>
        Vector<T> operator*(const Vector<T>& vector, T scalar) {
            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = vector[i] * scalar;
            }

            return result;
        }

        template <typename T>
        Vector<T> operator*(T scalar, const Vector<T>& vector) {
            return vector * scalar;
        }

        template <typename T>
        Vector<T> operator/(const Vector<T>& vector, T scalar) {
            VMAFU_IF_CONSTEXPR(VMAFU_IS_FLOATING_POINT_V(T)) {
                if (std::abs(scalar) < 1e-10) {
                    throw std::invalid_argument(
                        "linalg::operator/: Division by zero"
                    );
                }
            } else {
                if (scalar == T{0}) {
                    throw std::invalid_argument(
                        "linalg::operator/: Division by zero"
                    );
                }
            }

            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = vector[i] / scalar;
            }

            return result;
        }

        template <typename T>
        Vector<T> operator+(const Vector<T>& vector, T scalar) {
            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = vector[i] + scalar;
            }

            return result;
        }

        template <typename T>
        Vector<T> operator+(T scalar, const Vector<T>& vector) {
            return vector + scalar;
        }

        template <typename T>
        Vector<T> operator-(const Vector<T>& vector, T scalar) {
            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = vector[i] - scalar;
            }

            return result;
        }

        template <typename T>
        Vector<T> operator-(T scalar, const Vector<T>& vector) {
            Vector<T> result(vector.size());
            for (size_t i = 0; i < vector.size(); i++) {
                result[i] = scalar - vector[i];
            }

            return result;
        }

        //  Matrix-Matrix operations

        template <typename T>
        Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs) {
            if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols()) {
                throw std::invalid_argument(
                    "linalg::operator+: Matrix dimensions must match"
                );
            }

            Matrix<T> result(lhs.rows(), lhs.cols());
            for (size_t i = 0; i < lhs.size(); i++) {
                result[i] = lhs[i] + rhs[i];
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs) {
            if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols()) {
                throw std::invalid_argument(
                    "linalg::operator-: Matrix dimensions must match"
                );
            }

            Matrix<T> result(lhs.rows(), lhs.cols());
            for (size_t i = 0; i < lhs.size(); i++) {
                result[i] = lhs[i] - rhs[i];
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& matrix) {
            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = -matrix[i];
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
            if (lhs.cols() != rhs.rows()) {
                throw std::invalid_argument(
                    "linalg::operator*: Incompatible dimensions for matrix multiplication"
                );
            }

            Matrix<T> result(lhs.rows(), rhs.cols());

            // Простая реализация O(n^3)

            for (size_t i = 0; i < lhs.rows(); i++) {
                for (size_t j = 0; j < rhs.cols(); j++) {
                    T sum = T();
                    for (size_t k = 0; k < lhs.cols(); k++) {
                        sum += lhs(i, k) * rhs(k, j);
                    }

                    result(i, j) = sum;
                }
            }

            return result;
        }

        // Matrix-Scalar operations

        template <typename T>
        Matrix<T> operator*(const Matrix<T>& matrix, T scalar) {
            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = matrix[i] * scalar;
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator*(T scalar, const Matrix<T>& matrix) {
            return matrix * scalar;
        }

        template <typename T>
        Matrix<T> operator/(const Matrix<T>& matrix, T scalar) {
            VMAFU_IF_CONSTEXPR(VMAFU_IS_FLOATING_POINT_V(T)) {
                if (std::abs(scalar) < 1e-10) {
                    throw std::invalid_argument(
                        "linalg::operator/: Division by zero"
                    );
                }
            } else {
                if (scalar == T{0}) {
                    throw std::invalid_argument(
                        "linalg::operator/: Division by zero"
                    );
                }
            }

            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = matrix[i] / scalar;
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator+(const Matrix<T>& matrix, T scalar) {
            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = matrix[i] + scalar;
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator+(T scalar, const Matrix<T>& matrix) {
            return matrix + scalar;
        }

        template <typename T>
        Matrix<T> operator-(const Matrix<T>& matrix, T scalar) {
            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = matrix[i] - scalar;
            }

            return result;
        }

        template <typename T>
        Matrix<T> operator-(T scalar, const Matrix<T>& matrix) {
            Matrix<T> result(matrix.rows(), matrix.cols());
            for (size_t i = 0; i < matrix.size(); i++) {
                result[i] = scalar - matrix[i];
            }

            return result;
        }

        // Matrix-Vector operations

        template <typename T>
        Vector<T> operator*(const Matrix<T>& matrix, const Vector<T>& vector) {
            if (matrix.cols() != vector.size()) {
                throw std::invalid_argument(
                    "linalg::operator*: Matrix cols must match vector size"
                );
            }

            Vector<T> result(matrix.rows());
            for (size_t i = 0; i < matrix.rows(); i++) {
                T sum = T();
                for (size_t j = 0; j < matrix.cols(); j++) {
                    sum += matrix(i, j) * vector[j];
                }

                result[i] = sum;
            }

            return result;
        }

        template <typename T>
        Vector<T> operator*(const Vector<T>& vector, const Matrix<T>& matrix) {
            if (vector.size() != matrix.rows()) {
                throw std::invalid_argument(
                    "linalg::operator*: Vector size must match matrix rows"
                );
            }

            Vector<T> result(matrix.cols());
            for (size_t j = 0; j < matrix.cols(); j++) {
                T sum = T();
                for (size_t i = 0; i < matrix.rows(); i++) {
                    sum += vector[i] * matrix(i, j);
                }

                result[j] = sum;
            }

            return result;
        }
    }
}
