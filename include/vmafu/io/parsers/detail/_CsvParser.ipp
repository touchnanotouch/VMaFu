// io/parsers/detail/_CsvParser.ipp


namespace vmafu {
    namespace io {
        namespace parsers {
            // Helper methods

            std::vector<std::string> CsvParser::split_line(
                const std::string& line
            ) const {
                std::vector<std::string> fields;
                std::string current_field;

                bool in_quotes = false;

                for (size_t i = 0; i < line.length(); i++) {
                    char c = line[i];

                    if (c == '"') {
                        if (in_quotes && i + 1 < line.length() && line[i + 1] == '"') {
                            current_field += '"';
                            i++;
                        } else {
                            in_quotes = !in_quotes;
                        }
                    } else if (c == _delimiter && !in_quotes) {
                        fields.push_back(unescape_field(current_field));

                        current_field.clear();
                    } else {
                        current_field += c;
                    }
                }

                if (!current_field.empty() || line.back() == _delimiter) {
                    fields.push_back(unescape_field(current_field));
                }

                if (in_quotes) {
                    throw std::runtime_error("Unclosed quotes in CSV line: " + line);
                }

                if (_trim_cells) {
                    for (auto& field : fields) {
                        size_t start = 0;
                        size_t end = field.length();

                        while (
                            start < end && std::isspace(static_cast<unsigned char>(field[start]))
                        ) {
                            start++;
                        }
                        while (
                            end > start && std::isspace(static_cast<unsigned char>(field[end - 1]))
                        ) {
                            end--;
                        }

                        if (start > 0 || end < field.length()) {
                            field = field.substr(start, end - start);
                        }
                    }
                }

                return fields;
            }

            std::string CsvParser::escape_field(
                const std::string& field
            ) const {
                bool needs_quotes = false;

                if (
                    field.find(_delimiter) != std::string::npos ||
                    field.find('"') != std::string::npos ||
                    field.find('\n') != std::string::npos ||
                    field.find('\r') != std::string::npos ||
                    (_trim_cells && (field.front() == ' ' || field.back() == ' '))
                ) {
                    needs_quotes = true;
                }

                if (!needs_quotes) {
                    return field;
                }

                std::string escaped;
                escaped.reserve(field.length() + 2);

                escaped += '"';
                
                for (char c : field) {
                    if (c == '"') {
                        escaped += "\"\"";
                    } else {
                        escaped += c;
                    }
                }

                escaped += '"';

                return escaped;
            }

            std::string CsvParser::unescape_field(
                const std::string& field
            ) const {
                std::string result = field;

                if (result.length() >= 2 && result.front() == '"' && result.back() == '"') {
                    std::string unescaped;
                    unescaped.reserve(result.length());

                    for (size_t i = 1; i < result.length() - 1; i++) {
                        if (result[i] == '"' && i + 1 < result.length() - 1 && result[i + 1] == '"') {
                            unescaped += '"';
                            i++;
                        } else {
                            unescaped += result[i];
                        }
                    }

                    result = unescaped;
                }

                if (_trim_cells && !result.empty()) {
                    size_t start = 0;
                    size_t end = result.length();

                    while (
                        start < end && std::isspace(static_cast<unsigned char>(result[start]))
                    ) {
                        start++;
                    }
                    while (
                        end > start && std::isspace(static_cast<unsigned char>(result[end - 1]))
                    ) {
                        end--;
                    }

                    if (start > 0 || end < result.length()) {
                        result = result.substr(start, end - start);
                    }
                }

                return result;
            }

            // Constructor

            CsvParser::CsvParser(
                char delimiter,
                bool trim_cells,
                bool has_header
            ) : _delimiter(delimiter), 
                _trim_cells(trim_cells), 
                _has_header(has_header) {}

            // Getters

            char CsvParser::delimiter() const noexcept {
                return _delimiter;
            }

            bool CsvParser::trim_cells() const noexcept {
                return _trim_cells;
            }

            bool CsvParser::has_header() const noexcept {
                return _has_header;
            }

            // Parse method

            std::vector<std::vector<std::string>> CsvParser::parse(
                const std::string& content
            ) const {
                std::istringstream iss(content);
                std::string line;

                std::vector<std::vector<std::string>> result;

                bool first_line = true;

                while (std::getline(iss, line)) {
                    if (first_line && _has_header) {
                        first_line = false;
                        continue;
                    }

                    if (!line.empty()) {
                        result.push_back(split_line(line));
                    }
                }

                return result;
            }

            // Validation method

            bool CsvParser::validate(const std::string& content) const {
                if (content.empty()) {
                    return true;
                }

                try {
                    auto data = parse(content);

                    if (!data.empty()) {
                        size_t expected_columns = data[0].size();

                        for (size_t i = 1; i < data.size(); i++) {
                            if (data[i].size() != expected_columns) {
                                return false;
                            }
                        }
                    }

                    return true;
                } catch (...) {
                    return false;
                }
            }
            
            // Static method

            ParserPtr CsvParser::create(
                char delimiter,
                bool trim_cells,
                bool has_header 
            ) {
                return std::make_shared<CsvParser>(
                    delimiter, trim_cells, has_header
                );
            }
        }
    }
}
