// io/serializers/detail/_VectorSerializer.ipp


namespace vmafu {
    namespace io {
        namespace serializers {
            // Constructor

            template <typename T>
            VectorSerializer<T>::VectorSerializer(
                int precision
            ) : _precision(precision) {}

            // Getter

            template <typename T>
            int VectorSerializer<T>::precision() const noexcept {
                return _precision;
            }

            // Serialization methods

            template <typename T>
            std::vector<std::vector<std::string>> VectorSerializer<T>::serialize(
                const Vector<T>& vector
            ) const {
                size_t size = vector.size();

                std::vector<std::vector<std::string>> result;

                std::vector<std::string> row_data;
                row_data.reserve(size);

                for (size_t i = 0; i < size; i++) {
                    std::ostringstream oss;
                    
                    if constexpr (std::is_floating_point_v<T>) {
                        oss << std::setprecision(_precision);

                        double val = static_cast<double>(vector[i]);
                        if (std::abs(val) < 1e-4 || std::abs(val) >= 1e6) {
                            oss << std::scientific;
                        } else {
                            oss << std::fixed;
                        }

                        oss << val;
                    } else {
                        oss << vector[i];
                    }

                    row_data.push_back(oss.str());
                }

                result.push_back(row_data);

                return result;
            }

            template <typename T>
            Vector<T> VectorSerializer<T>::deserialize(
                const std::vector<std::vector<std::string>>& string_data
            ) const {
                if (string_data.empty()) {
                    return Vector<T>();
                }

                if (string_data.size() != 1) {
                    throw std::runtime_error(
                        "Invalid data format: expected 1 row from serialize(), got " + 
                        std::to_string(string_data.size()) + " rows"
                    );
                }

                const auto& row = string_data[0];

                size_t size = row.size();

                Vector<T> result(size);
                for (size_t i = 0; i < size; i++) {
                    std::istringstream iss(row[i]);
                    T value;

                    if (!(iss >> value)) {
                        throw std::runtime_error(
                            "Failed to parse value at column " + 
                            std::to_string(i) + ": " + row[i]
                        );
                    }

                    result[i] = value;
                }

                return result;
            }

            // Static method

            template <typename T>
            SerializerPtr<Vector<T>> VectorSerializer<T>::create(
                int precision
            ) {
                return std::make_shared<VectorSerializer<T>>(precision);
            }
        }
    }
}
