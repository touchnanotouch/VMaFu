// io/serializers/_VectorSerializer.hpp


#pragma once


#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <memory>
#include <iomanip>

#include "_ISerializer.hpp"

#include "../../core/_Vector.hpp"


namespace vmafu {
    namespace io {
        namespace serializers {
            template <typename T>
            class VectorSerializer : public ISerializer<Vector<T>> {
                private:
                    int _precision = 6;

                public:
                    // Constructor

                    explicit VectorSerializer(int precision = 6);

                    // Getter

                    int precision() const noexcept;

                    // Serialization methods

                    std::vector<std::vector<std::string>> serialize(
                        const Vector<T>& vector
                    ) const override;

                    Vector<T> deserialize(
                        const std::vector<std::vector<std::string>>& string_data
                    ) const override;

                    // Static method

                    static SerializerPtr<Vector<T>> create(int precision = 6);
                };
        }

        using serializers::VectorSerializer;
    }
}


#include "detail/_VectorSerializer.ipp"
