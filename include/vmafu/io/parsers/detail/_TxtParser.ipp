// io/parsers/detail/_TxtParser.ipp

namespace vmafu {
    namespace io {
        namespace parsers {
            // Helper methods

            std::string TxtParser::process_line(
                const std::string& line
            ) const {
                std::string processed = line;

                if (_trim_lines) {
                    size_t start = 0;
                    while (
                        start < processed.length() && 
                        std::isspace(static_cast<unsigned char>(processed[start]))
                    ) {
                        start++;
                    }

                    size_t end = processed.length();
                    while (
                        end > start && 
                        std::isspace(static_cast<unsigned char>(processed[end - 1]))
                    ) {
                        end--;
                    }
                    
                    if (start > 0 || end < processed.length()) {
                        processed = processed.substr(start, end - start);
                    }
                }

                return processed;
            }

            std::vector<std::string> TxtParser::split_by_whitespace(
                const std::string& line
            ) const {
                std::vector<std::string> tokens;
                std::istringstream iss(line);

                std::string token;

                while (iss >> token) {
                    tokens.push_back(token);
                }

                return tokens;
            }


            // Constructor

            TxtParser::TxtParser(
                bool trim_lines,
                bool skip_empty_lines,
                bool split_by_whitespace
            ) : _trim_lines(trim_lines),
                _skip_empty_lines(skip_empty_lines),
                _split_by_whitespace(split_by_whitespace) {}

            // Getters

            bool TxtParser::trim_lines() const noexcept {
                return _trim_lines;
            }

            bool TxtParser::skip_empty_lines() const noexcept {
                return _skip_empty_lines;
            }

            bool TxtParser::split_by_whitespace() const noexcept {
                return _split_by_whitespace;
            }

            // Parsing methods

            std::vector<std::vector<std::string>> TxtParser::parse(
                const std::string& content
            ) const {
                std::istringstream iss(content);
                std::string line;

                std::vector<std::vector<std::string>> result;

                while (std::getline(iss, line)) {
                    std::string processed_line = process_line(line);

                    if (_skip_empty_lines && processed_line.empty()) {
                        continue;
                    }

                    std::vector<std::string> line_vector;

                    if (_split_by_whitespace) {
                        line_vector = split_by_whitespace(processed_line);
                    } else {
                        line_vector.push_back(processed_line);
                    }

                    result.push_back(line_vector);
                }

                return result;
            }

            std::string TxtParser::unparse(
                const std::vector<std::vector<std::string>>& data
            ) const {
                std::ostringstream oss;
                
                for (size_t i = 0; i < data.size(); i++) {
                    const auto& row = data[i];
                    
                    if (_split_by_whitespace) {
                        for (size_t j = 0; j < row.size(); j++) {
                            oss << row[j];

                            if (j + 1 < row.size()) {
                                oss << " ";
                            }
                        }
                    } else {
                        if (!row.empty()) {
                            oss << row[0];
                        }
                    }
                    
                    if (i + 1 < data.size()) {
                        oss << "\n";
                    }
                }
                
                return oss.str();
            }

            // Validation method

            bool TxtParser::validate(const std::string& content) const {
                return true;
            }

            // Static method

            ParserPtr TxtParser::create(
                bool trim_lines,
                bool skip_empty_lines,
                bool split_by_whitespace
            ) {
                return std::make_shared<TxtParser>(
                    trim_lines, skip_empty_lines, split_by_whitespace
                );
            }
        }
    }
}
