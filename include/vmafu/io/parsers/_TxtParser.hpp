// io/parsers/_TxtParser.hpp

#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "_IParser.hpp"

namespace vmafu {
    namespace io {
        namespace parsers {
            class TxtParser : public IParser {
            private:
                bool _trim_lines;
                bool _skip_empty_lines;
                bool _split_by_whitespace;
                
                // Helper methods

                std::string process_line(const std::string& line) const;
                std::vector<std::string> split_by_whitespace(const std::string& line) const;
                
            public:
                // Constructor

                explicit TxtParser(
                    bool trim_lines = true,
                    bool skip_empty_lines = true,
                    bool split_by_whitespace = false
                );
                
                // Getters

                bool trim_lines() const noexcept;
                bool skip_empty_lines() const noexcept;
                bool split_by_whitespace() const noexcept;

                // Parse method

                std::vector<std::vector<std::string>> parse(
                    const std::string& content
                ) const override;
                
                // Validation method

                bool validate(const std::string& content) const override;
                
                // Static method

                static ParserPtr create(
                    bool trim_lines = true,
                    bool skip_empty_lines = true,
                    bool split_by_whitespace = false
                );
            };
        }
    }
}


#include "detail/_TxtParser.ipp"
