// io/serializers/detail/_MatrixSerializer.ipp


namespace vmafu {
    namespace io {
        namespace serializers {
            // Constructor

            template <typename T>
            MatrixSerializer<T>::MatrixSerializer(
                int precision
            ) :  _precision(precision) {}

            // Getter

            template <typename T>
            int MatrixSerializer<T>::precision() const noexcept {
                return _precision;
            }

            // Serialization methods

            template <typename T>
            std::vector<std::vector<std::string>> MatrixSerializer<T>::serialize(
                const Matrix<T>& matrix
            ) const {
                size_t rows = matrix.rows();
                size_t cols = matrix.cols();

                std::vector<std::vector<std::string>> result;

                for (size_t i = 0; i < rows; i++) {
                    std::vector<std::string> row_data;
                    row_data.reserve(cols);

                    for (size_t j = 0; j < cols; j++) {
                        std::ostringstream oss;

                        if constexpr (std::is_floating_point_v<T>) {
                            oss << std::setprecision(_precision);

                            double val = static_cast<double>(matrix(i, j));
                            if (std::abs(val) < 1e-4 || std::abs(val) >= 1e6) {
                                oss << std::scientific;
                            } else {
                                oss << std::fixed;
                            }

                            oss << val;
                        } else {
                            oss << matrix(i, j);
                        }

                        row_data.push_back(oss.str());
                    }

                    result.push_back(row_data);
                }

                return result;
            }

            template <typename T>
            Matrix<T> MatrixSerializer<T>::deserialize(
                const std::vector<std::vector<std::string>>& string_data
            ) const {
                if (string_data.empty()) {
                    return Matrix<T>();
                }

                size_t rows = string_data.size();
                size_t cols = string_data[0].size();

                for (size_t i = 0; i < rows; i++) {
                    if (string_data[i].size() != cols) {
                        throw std::invalid_argument(
                            "Inconsistent column count in data at row " + 
                            std::to_string(i) + 
                            " (expected " + std::to_string(cols) + 
                            ", got " + std::to_string(string_data[i].size()) + ")"
                        );
                    }
                }

                Matrix<T> matrix(rows, cols);

                for (size_t i = 0; i < rows; i++) {
                    const auto& row = string_data[i];

                    for (size_t j = 0; j < cols; j++) {
                        std::istringstream iss(row[j]);
                        T value;

                        if (!(iss >> value)) {
                            throw std::runtime_error(
                                "Failed to parse value at row " + 
                                std::to_string(i) + ", column " + 
                                std::to_string(j) + ": " + row[j]
                            );
                        }

                        matrix(i, j) = value;
                    }
                }

                return matrix;
            }

            // Static method
            
            template <typename T>
            SerializerPtr<Matrix<T>> MatrixSerializer<T>::create(
                int precision
            ) {
                return std::make_shared<MatrixSerializer<T>>(
                    precision
                );
            }
        }
    }
}
