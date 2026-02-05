// io/parsers/_CsvParser.hpp


#pragma once


#include <vector>
#include <string>
#include <sstream>

#include "_IParser.hpp"


namespace vmafu {
    namespace io {
        namespace parsers {
            class CsvParser: public IParser {
                private:
                    char _delimiter;
                    bool _trim_cells;
                    bool _has_header;

                    // Helper methods

                    std::vector<std::string> split_line(
                        const std::string& line
                    ) const;

                    std::string escape_field(
                        const std::string& field
                    ) const;

                    std::string unescape_field(
                        const std::string& field
                    ) const;

                public:
                    // Constructor

                    explicit CsvParser(
                        char delimiter = ',',
                        bool trim_cells = true,
                        bool has_header = false
                    );
 
                    // Getters

                    char delimiter() const noexcept;
                    bool trim_cells() const noexcept;
                    bool has_header() const noexcept;

                    // Parse method

                    std::vector<std::vector<std::string>> parse(
                        const std::string& content
                    ) const override;

                    // Validation method

                    bool validate(const std::string& content) const override;

                    // Static method

                    static ParserPtr create(
                        char delimiter = ',',
                        bool trim_cells = true,
                        bool has_header = false
                    );
            };
        }
    }
}


#include "detail/_CsvParser.ipp"
