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

#include "../../core/_Matrix.hpp"
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
                
                // Getters
                int precision() const noexcept;
                
                // Setters
                void set_precision(int precision);
                
                // ISerializer implementation
                std::vector<std::vector<std::string>> serialize(
                    const Vector<T>& vector
                ) const override;
                
                Vector<T> deserialize(
                    const std::vector<std::vector<std::string>>& string_data
                ) const override;
                
                // Static factory methods
                static SerializerPtr<Vector<T>> create(int precision = 6);
                
                // Additional convenience methods
                std::vector<std::string> serialize_to_row(
                    const Vector<T>& vector
                ) const;
                
                std::vector<std::string> serialize_to_column(
                    const Vector<T>& vector
                ) const;
                
                Vector<T> deserialize_from_row(
                    const std::vector<std::string>& row_data
                ) const;
                
                Vector<T> deserialize_from_column(
                    const std::vector<std::vector<std::string>>& column_data
                ) const;
            };
        }

        using serializers::VectorSerializer;
    }
}


#include "detail/_VectorSerializer.ipp"
