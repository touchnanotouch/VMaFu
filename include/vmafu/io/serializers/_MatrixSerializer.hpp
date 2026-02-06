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


namespace vmafu {
    namespace io {
        namespace serializers {
            template <typename T>
            class MatrixSerializer : public ISerializer<Matrix<T>> {
                private:
                    int _precision = 6;

                public:
                    // Constructor

                    explicit MatrixSerializer(
                        int precision = 6
                    );

                    // Getter

                    int precision() const noexcept;

                    // Serialization methods

                    std::vector<std::vector<std::string>> serialize(
                        const Matrix<T>& matrix
                    ) const override;

                    Matrix<T> deserialize(
                        const std::vector<std::vector<std::string>>& string_data
                    ) const override;

                    // Static method

                    static SerializerPtr<Matrix<T>> create(
                        int precision = 6
                    );
                };
        }

        using serializers::MatrixSerializer;
    }
}


#include "detail/_MatrixSerializer.ipp"
