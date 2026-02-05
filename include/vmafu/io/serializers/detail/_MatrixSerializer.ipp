// io/serializers/detail/_MatrixSerializer.ipp


namespace vmafu {
    namespace io {
        namespace serializers {
            // Constructor
            
            template <typename T>
            MatrixSerializer<T>::MatrixSerializer(
                char delimiter,
                int precision,
                bool include_dimensions
            ) : _delimiter(delimiter),
                _precision(precision),
                _include_dimensions(include_dimensions) {}
            
            // Getters
            
            template <typename T>
            char MatrixSerializer<T>::delimiter() const noexcept {
                return _delimiter;
            }
            
            template <typename T>
            int MatrixSerializer<T>::precision() const noexcept {
                return _precision;
            }
            
            template <typename T>
            bool MatrixSerializer<T>::include_dimensions() const noexcept {
                return _include_dimensions;
            }
            
            // Setters
            
            template <typename T>
            void MatrixSerializer<T>::set_delimiter(char delimiter) {
                _delimiter = delimiter;
            }
            
            template <typename T>
            void MatrixSerializer<T>::set_precision(int precision) {
                _precision = precision;
            }
            
            template <typename T>
            void MatrixSerializer<T>::set_include_dimensions(bool include) {
                _include_dimensions = include;
            }
            
            // ISerializer implementation
            
            template <typename T>
            std::vector<std::vector<std::string>> MatrixSerializer<T>::serialize(
                const Matrix<T>& matrix
            ) const {
                std::vector<std::vector<std::string>> result;
                
                size_t rows = matrix.rows();
                size_t cols = matrix.cols();
                
                // Если нужно, добавляем строку с размерами
                if (_include_dimensions) {
                    std::vector<std::string> dim_row;
                    dim_row.push_back("rows=" + std::to_string(rows));
                    dim_row.push_back("cols=" + std::to_string(cols));
                    result.push_back(dim_row);
                }
                
                // Сериализуем данные матрицы
                for (size_t i = 0; i < rows; ++i) {
                    std::vector<std::string> row_data;
                    row_data.reserve(cols);
                    
                    for (size_t j = 0; j < cols; ++j) {
                        std::ostringstream oss;
                        
                        if constexpr (std::is_floating_point_v<T>) {
                            oss.precision(_precision);
                            oss << std::fixed << matrix(i, j);
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
                
                size_t start_row = 0;
                size_t rows = string_data.size();
                size_t cols = string_data[0].size();
                
                // Пропускаем строку с размерами если она есть
                if (_include_dimensions && !string_data.empty()) {
                    const auto& first_row = string_data[0];
                    if (first_row.size() >= 2) {
                        // Парсим строку вида "rows=3,cols=4"
                        // Для простоты просто пропускаем эту строку
                        start_row = 1;
                        rows = string_data.size() - 1;
                        
                        if (rows > 0) {
                            cols = string_data[1].size();
                        } else {
                            cols = 0;
                        }
                    }
                }
                
                // Проверяем консистентность данных
                for (size_t i = start_row; i < string_data.size(); ++i) {
                    if (string_data[i].size() != cols) {
                        throw std::invalid_argument(
                            "Inconsistent column count in CSV data at row " + 
                            std::to_string(i - start_row)
                        );
                    }
                }
                
                // Создаем и заполняем матрицу
                Matrix<T> matrix(rows, cols);
                
                for (size_t i = 0; i < rows; ++i) {
                    const auto& row = string_data[start_row + i];
                    
                    for (size_t j = 0; j < cols; ++j) {
                        std::istringstream iss(row[j]);
                        T value;
                        
                        if (!(iss >> value)) {
                            throw std::runtime_error(
                                "Failed to parse value at row " + 
                                std::to_string(i) + ", column " + 
                                std::to_string(j) + ": " + row[j]
                            );
                        }
                        
                        // Проверяем, что весь контент был прочитан
                        std::string remaining;
                        if (iss >> remaining) {
                            throw std::runtime_error(
                                "Extra characters in value at row " + 
                                std::to_string(i) + ", column " + 
                                std::to_string(j) + ": " + row[j]
                            );
                        }
                        
                        matrix(i, j) = value;
                    }
                }
                
                return matrix;
            }
            
            // Additional utility methods
            
            template <typename T>
            std::vector<std::vector<std::string>> MatrixSerializer<T>::serialize_with_header(
                const Matrix<T>& matrix,
                const std::vector<std::string>& col_headers,
                const std::vector<std::string>& row_headers
            ) const {
                std::vector<std::vector<std::string>> result;
                size_t rows = matrix.rows();
                size_t cols = matrix.cols();
                
                // Добавляем заголовок столбцов (если есть)
                if (!col_headers.empty()) {
                    std::vector<std::string> header_row;
                    
                    // Пустая ячейка для угла
                    if (!row_headers.empty()) {
                        header_row.push_back("");
                    }
                    
                    // Заголовки столбцов
                    for (size_t j = 0; j < cols && j < col_headers.size(); ++j) {
                        header_row.push_back(col_headers[j]);
                    }
                    
                    // Дополняем если заголовков меньше чем столбцов
                    for (size_t j = col_headers.size(); j < cols; ++j) {
                        header_row.push_back("col_" + std::to_string(j));
                    }
                    
                    result.push_back(header_row);
                }
                
                // Данные с заголовками строк (если есть)
                for (size_t i = 0; i < rows; ++i) {
                    std::vector<std::string> row_data;
                    
                    // Заголовок строки
                    if (!row_headers.empty()) {
                        if (i < row_headers.size()) {
                            row_data.push_back(row_headers[i]);
                        } else {
                            row_data.push_back("row_" + std::to_string(i));
                        }
                    }
                    
                    // Данные матрицы
                    for (size_t j = 0; j < cols; ++j) {
                        std::ostringstream oss;
                        
                        if constexpr (std::is_floating_point_v<T>) {
                            oss.precision(_precision);
                            oss << std::fixed << matrix(i, j);
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
            Matrix<T> MatrixSerializer<T>::deserialize_with_header(
                const std::vector<std::vector<std::string>>& string_data,
                std::vector<std::string>& col_headers,
                std::vector<std::string>& row_headers
            ) const {
                col_headers.clear();
                row_headers.clear();
                
                if (string_data.empty()) {
                    return Matrix<T>();
                }
                
                // Определяем наличие заголовков
                bool has_col_headers = false;
                bool has_row_headers = false;
                
                if (!string_data.empty()) {
                    // Предполагаем, что первая строка может быть заголовком столбцов
                    // Проверяем, содержит ли она нечисловые значения
                    has_col_headers = true;
                    for (const auto& cell : string_data[0]) {
                        std::istringstream iss(cell);
                        T test_value;
                        if (iss >> test_value) {
                            // Если можно распарсить как число, то это данные
                            std::string remaining;
                            if (!(iss >> remaining)) {
                                has_col_headers = false;
                                break;
                            }
                        }
                    }
                }
                
                // Извлекаем заголовки столбцов
                size_t data_start_row = 0;
                if (has_col_headers) {
                    col_headers = string_data[0];
                    data_start_row = 1;
                }
                
                // Извлекаем заголовки строк и данные
                size_t data_start_col = 0;
                if (has_col_headers && !string_data[data_start_row].empty()) {
                    // Проверяем первую ячейку на нечисловое значение
                    std::istringstream iss(string_data[data_start_row][0]);
                    T test_value;
                    if (!(iss >> test_value)) {
                        has_row_headers = true;
                        data_start_col = 1;
                    }
                }
                
                // Извлекаем данные
                size_t rows = string_data.size() - data_start_row;
                size_t cols = (rows > 0) ? 
                    string_data[data_start_row].size() - data_start_col : 0;
                
                Matrix<T> matrix(rows, cols);
                
                for (size_t i = 0; i < rows; ++i) {
                    const auto& row = string_data[data_start_row + i];
                    
                    // Извлекаем заголовок строки
                    if (has_row_headers && row.size() > data_start_col) {
                        if (i < row.size()) {
                            row_headers.push_back(row[0]);
                        }
                    }
                    
                    // Извлекаем данные
                    for (size_t j = 0; j < cols; ++j) {
                        size_t col_idx = data_start_col + j;
                        
                        if (col_idx >= row.size()) {
                            throw std::invalid_argument(
                                "Missing data at row " + std::to_string(i) + 
                                ", column " + std::to_string(j)
                            );
                        }
                        
                        std::istringstream iss(row[col_idx]);
                        T value;
                        
                        if (!(iss >> value)) {
                            throw std::runtime_error(
                                "Failed to parse value at row " + 
                                std::to_string(i) + ", column " + 
                                std::to_string(j) + ": " + row[col_idx]
                            );
                        }
                        
                        matrix(i, j) = value;
                    }
                }
                
                return matrix;
            }
            
            // Static factory methods
            
            template <typename T>
            SerializerPtr<Matrix<T>> MatrixSerializer<T>::create(
                char delimiter,
                int precision,
                bool include_dimensions
            ) {
                return std::make_shared<MatrixSerializer<T>>(
                    delimiter, precision, include_dimensions
                );
            }
        }
    }
}
