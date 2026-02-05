// io/parsers/_IParser.hpp


#pragma once


#include <vector>
#include <string>
#include <sstream>


namespace vmafu {
    namespace io {
        namespace parsers {
            class IParser {
                public:
                    // Destructor

                    virtual ~IParser() = default;

                    // Parsing methodd

                    virtual std::vector<std::vector<std::string>> parse(
                        const std::string& content
                    ) const = 0;

                    virtual std::string unparse(
                        const std::vector<std::vector<std::string>>& data
                    ) const = 0;
                    
                    // Validation method

                    virtual bool validate(
                        const std::string& content
                    ) const = 0;
            };

            using ParserPtr = std::shared_ptr<IParser>;
        }
    }
}
