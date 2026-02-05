// io/serializers/_MatrixSerializer.hpp

#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <memory>
#include <iomanip>

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
                
            public:
                // Constructor
                explicit MatrixSerializer(
                    char delimiter = ',',
                    int precision = 6
                );
                
                // Getters
                char delimiter() const noexcept;
                int precision() const noexcept;
                
                // Setters
                void set_delimiter(char delimiter);
                void set_precision(int precision);
                
                // ISerializer implementation
                std::vector<std::vector<std::string>> serialize(
                    const Matrix<T>& matrix
                ) const override;
                
                Matrix<T> deserialize(
                    const std::vector<std::vector<std::string>>& string_data
                ) const override;
                
                // Static factory methods
                static SerializerPtr<Matrix<T>> create(
                    char delimiter = ',',
                    int precision = 6
                );
            };
        }
    }
}


#include "detail/_MatrixSerializer.ipp"
