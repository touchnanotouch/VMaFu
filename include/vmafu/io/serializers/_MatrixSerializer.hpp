// io/serializers/_MatrixSerializer.hpp


#pragma once


#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <type_traits>

#include "_ISerializer.hpp"
#include "../../core/_Matrix.hpp"
#include "../../core/_Vector.hpp"


namespace vmafu {
    namespace io {
        namespace serializers {
            template <typename T>
            class MatrixSerializer : public ISerializer<Matrix<T>> {
            private:
                char _delimiter = ',';
                int _precision = 6;
                bool _include_dimensions = false;
                
            public:
                // Constructor
                explicit MatrixSerializer(
                    char delimiter = ',',
                    int precision = 6,
                    bool include_dimensions = false
                );
                
                // Getters
                char delimiter() const noexcept;
                int precision() const noexcept;
                bool include_dimensions() const noexcept;
                
                // Setters
                void set_delimiter(char delimiter);
                void set_precision(int precision);
                void set_include_dimensions(bool include);
                
                // ISerializer implementation
                std::vector<std::vector<std::string>> serialize(
                    const Matrix<T>& matrix
                ) const override;
                
                Matrix<T> deserialize(
                    const std::vector<std::vector<std::string>>& string_data
                ) const override;
                
                // Additional utility methods
                std::vector<std::vector<std::string>> serialize_with_header(
                    const Matrix<T>& matrix,
                    const std::vector<std::string>& col_headers = {},
                    const std::vector<std::string>& row_headers = {}
                ) const;
                
                Matrix<T> deserialize_with_header(
                    const std::vector<std::vector<std::string>>& string_data,
                    std::vector<std::string>& col_headers,
                    std::vector<std::string>& row_headers
                ) const;
                
                // Static factory methods
                static SerializerPtr<Matrix<T>> create(
                    char delimiter = ',',
                    int precision = 6,
                    bool include_dimensions = false
                );
            };
        }
    }
}


#include "detail/_MatrixSerializer.ipp"
