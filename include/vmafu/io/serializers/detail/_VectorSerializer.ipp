// io/serializers/detail/_VectorSerializer.ipp


// TODO: че то ии написал, хз смотреть надо (плюс еще и .hpp файл)

namespace vmafu {
    namespace io {
        namespace serializers {
            // Constructor
            
            template <typename T>
            VectorSerializer<T>::VectorSerializer(int precision)
                : _precision(precision) {}
            
            // Getters
            
            template <typename T>
            int VectorSerializer<T>::precision() const noexcept {
                return _precision;
            }
            
            // Setters
            
            template <typename T>
            void VectorSerializer<T>::set_precision(int precision) {
                _precision = precision;
            }
            
            // ISerializer implementation
            
            template <typename T>
            std::vector<std::vector<std::string>> VectorSerializer<T>::serialize(
                const Vector<T>& vector
            ) const {
                std::vector<std::vector<std::string>> result;
                
                size_t size = vector.size();
                
                // Для вектора создаем одну строку (можно сделать и столбец)
                std::vector<std::string> row_data;
                row_data.reserve(size);
                
                for (size_t i = 0; i < size; ++i) {
                    std::ostringstream oss;
                    
                    if constexpr (std::is_floating_point_v<T>) {
                        oss << std::setprecision(_precision);
                        
                        // Автоматический выбор формата
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
                
                // Вектор может быть представлен как:
                // 1. Одна строка (горизонтальный вектор)
                // 2. Один столбец (вертикальный вектор)
                // 3. Матрица 1xN или Nx1
                
                size_t rows = string_data.size();
                
                if (rows == 1) {
                    // Горизонтальный вектор: [x1, x2, ..., xn]
                    const auto& row = string_data[0];
                    size_t size = row.size();
                    
                    Vector<T> result(size);
                    for (size_t i = 0; i < size; ++i) {
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
                    
                } else {
                    // Вертикальный вектор или общий случай
                    size_t cols = string_data[0].size();
                    
                    // Проверяем консистентность
                    for (size_t i = 0; i < rows; ++i) {
                        if (string_data[i].size() != cols) {
                            throw std::invalid_argument(
                                "Inconsistent column count in data at row " + 
                                std::to_string(i)
                            );
                        }
                    }
                    
                    if (cols == 1) {
                        // Вертикальный вектор
                        Vector<T> result(rows);
                        
                        for (size_t i = 0; i < rows; ++i) {
                            std::istringstream iss(string_data[i][0]);
                            T value;
                            
                            if (!(iss >> value)) {
                                throw std::runtime_error(
                                    "Failed to parse value at row " + 
                                    std::to_string(i) + ": " + string_data[i][0]
                                );
                            }
                            
                            result[i] = value;
                        }
                        
                        return result;
                        
                    } else {
                        // Матрица - выбрасываем исключение или конвертируем?
                        throw std::invalid_argument(
                            "Cannot deserialize matrix to vector. "
                            "Data has " + std::to_string(rows) + " rows and " +
                            std::to_string(cols) + " columns. "
                            "Expected 1xN or Nx1."
                        );
                    }
                }
            }
            
            // Static factory methods
            
            template <typename T>
            SerializerPtr<Vector<T>> VectorSerializer<T>::create(int precision) {
                return std::make_shared<VectorSerializer<T>>(precision);
            }
            
            // Additional convenience methods
            
            template <typename T>
            std::vector<std::string> VectorSerializer<T>::serialize_to_row(
                const Vector<T>& vector
            ) const {
                auto serialized = serialize(vector);
                return serialized.empty() ? std::vector<std::string>() : serialized[0];
            }
            
            template <typename T>
            std::vector<std::string> VectorSerializer<T>::serialize_to_column(
                const Vector<T>& vector
            ) const {
                // Конвертируем вектор в столбец
                std::vector<std::string> column_data;
                column_data.reserve(vector.size());
                
                for (size_t i = 0; i < vector.size(); ++i) {
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
                    
                    column_data.push_back(oss.str());
                }
                
                return column_data;
            }
            
            template <typename T>
            Vector<T> VectorSerializer<T>::deserialize_from_row(
                const std::vector<std::string>& row_data
            ) const {
                // Создаем формат, ожидаемый deserialize
                std::vector<std::vector<std::string>> string_data;
                string_data.push_back(row_data);
                
                return deserialize(string_data);
            }
            
            template <typename T>
            Vector<T> VectorSerializer<T>::deserialize_from_column(
                const std::vector<std::vector<std::string>>& column_data
            ) const {
                // column_data должен быть Nx1
                for (const auto& row : column_data) {
                    if (row.size() != 1) {
                        throw std::invalid_argument(
                            "Column data must have exactly one element per row"
                        );
                    }
                }
                
                return deserialize(column_data);
            }
        }
    }
}
