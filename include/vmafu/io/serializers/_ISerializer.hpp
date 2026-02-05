// io/serializers/_ISerializer.hpp


#pragma once


#include <vector>
#include <string>


namespace vmafu {
    namespace io {
        namespace serializers {
            template <typename DataT>
            class ISerializer {
                public:
                    virtual ~ISerializer() = default;

                    virtual std::vector<std::vector<std::string>> serialize(
                        const DataT& data
                    ) const = 0;

                    virtual DataT deserialize(
                        const std::vector<std::vector<std::string>>& string_data
                    ) const = 0;
                };

            template <typename DataT>
            using SerializerPtr = std::shared_ptr<ISerializer<DataT>>;
        }
    }
}
