// core/detail/_core.ipp


namespace vmafu {
    namespace core {
        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Vector<T>& vector) {
            os << "Vector<" \
               << typeid(T).name() << ">[" << vector.size() << "]: " \
               << "[";

            for (size_t i = 0; i < vector.size(); i++) {
                os << vector[i];

                if (i + 1 < vector.size()) {
                    os << ", ";
                }
            }

            os << "]";

            return os;
        }

        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
            os << "Matrix<" \
               << typeid(T).name() << ">[" \
               << matrix.rows() << "x" << matrix.cols() \
               << "]\n";

            for (size_t i = 0; i < matrix.rows(); i++) {
                os << "[";

                for (size_t j = 0; j < matrix.cols(); j++) {
                    os << matrix(i, j);

                    if (j + 1 < matrix.cols()) {
                        os << ", ";
                    }
                }

                os << "]";

                if (i + 1 < matrix.rows()) {
                    os << "\n";
                }
            }

            return os;
        }
    }
}
