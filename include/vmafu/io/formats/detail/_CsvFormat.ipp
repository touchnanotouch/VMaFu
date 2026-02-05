// io/formats/detail/_CsvFormat.ipp


namespace vmafu {
    namespace io {
        namespace formats {
            // Constructor

            CsvFormat::CsvFormat(
                char delimiter,
                bool trim_cells,
                bool has_header
            ) : _parser(delimiter, trim_cells, has_header) {}

            // Getter

            const parsers::CsvParser& CsvFormat::parser() const noexcept {
                return _parser;
            }

            // Read methods

            std::string CsvFormat::read(
                const std::string& filename
            ) const {
                std::ifstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot open file: " + filename);
                }

                std::string result = read_stream(file);

                file.close();

                return result;
            }

            std::string CsvFormat::read_stream(
                std::istream& stream
            ) const {
                std::ostringstream oss;
                std::string line;

                while (std::getline(stream, line)) {
                    oss << line << "\n";
                }

                return oss.str();
            }

            std::string CsvFormat::read_chunk(
                std::istream& stream,
                size_t chunk_size
            ) const {
                std::ostringstream oss;
                std::string line;

                size_t bytes_read = 0;
                
                while (bytes_read < chunk_size && std::getline(stream, line)) {
                    oss << line << "\n";

                    bytes_read += line.size() + 1;
                }

                return oss.str();
            }

            std::string CsvFormat::read_chunks(
                const std::string& filename,
                size_t chunk_size
            ) const {
                std::ifstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot open file: " + filename);
                }

                std::string result = read_stream_chunks(file, chunk_size);

                file.close();

                return result;
            }

            std::string CsvFormat::read_stream_chunks(
                std::istream& stream,
                size_t chunk_size
            ) const {
                std::ostringstream oss;
                std::string chunk;

                do {
                    chunk = read_chunk(stream, chunk_size);
                    if (!chunk.empty()) {
                        oss << chunk;
                    }
                } while (!chunk.empty());

                return oss.str();
            }

            // Write methods

            void CsvFormat::write(
                const std::string& filename,
                const std::string& content
            ) const {
                std::ofstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot open file for writing: " + filename);
                }

                write_stream(file, content);

                file.close();
            }

            void CsvFormat::write_stream(
                std::ostream& stream,
                const std::string& content
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid CSV content for stream");
                }
                
                stream << content;

                if (!stream) {
                    throw std::runtime_error("Failed to write to stream");
                }
            }

            void CsvFormat::write_chunk(
                std::ostream& stream,
                const std::string& chunk
            ) const {
                stream.write(chunk.c_str(), chunk.size());

                if (!stream) {
                    throw std::runtime_error("Failed to write chunk to stream");
                }
            }

            void CsvFormat::write_chunks(
                const std::string& filename,
                const std::string& content,
                size_t chunk_size
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid CSV content for file: " + filename);
                }

                std::ofstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot open file for writing: " + filename);
                }

                write_stream_chunks(file, content, chunk_size);
                file.close();
            }

            void CsvFormat::write_stream_chunks(
                std::ostream& stream,
                const std::string& content,
                size_t chunk_size
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid CSV content for stream chunks");
                }

                size_t total_size = content.size();

                size_t pos = 0;

                while (pos < total_size) {
                    size_t current_chunk_size = std::min(chunk_size, total_size - pos);

                    std::string chunk = content.substr(pos, current_chunk_size);
                    write_chunk(stream, chunk);

                    pos += current_chunk_size;
                }

                if (!stream) {
                    throw std::runtime_error("Failed to write chunks to stream");
                }
            }

            // Validation methods

            bool CsvFormat::validate(
                const std::string& content
            ) const {
                return _parser.validate(content);
            }

            bool CsvFormat::validate_stream(
                std::istream& stream
            ) const {
                std::string content;
                std::string line;
                
                while (std::getline(stream, line)) {
                    content += line + "\n";
                }

                return validate(content);
            }
        }
    }
}
