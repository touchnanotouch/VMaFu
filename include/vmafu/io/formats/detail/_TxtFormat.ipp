// io/formats/detail/_TxtFormat.ipp


namespace vmafu {
    namespace io {
        namespace formats {
            // Constructor

            TxtFormat::TxtFormat(
                bool trim_lines,
                bool skip_empty_lines,
                bool split_by_whitespace
            ) : _parser(trim_lines, skip_empty_lines, split_by_whitespace) {}

            // Getter

            const parsers::TxtParser& TxtFormat::parser() const noexcept {
                return _parser;
            }

            // Read methods

            std::string TxtFormat::read(
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

            std::string TxtFormat::read_stream(
                std::istream& stream
            ) const {
                std::ostringstream oss;
                std::string line;

                while (std::getline(stream, line)) {
                    oss << line;

                    if (stream.peek() != EOF) {
                        oss << "\n";
                    }
                }

                return oss.str();
            }

            std::string TxtFormat::read_chunk(
                std::istream& stream,
                size_t chunk_size
            ) const {
                std::ostringstream oss;
                std::string line;

                size_t bytes_read = 0;

                while (bytes_read < chunk_size && std::getline(stream, line)) {
                    oss << line;

                    if (stream.peek() != EOF) {
                        oss << "\n";
                    }

                    bytes_read += line.size() + 1;
                }

                return oss.str();
            }

            std::string TxtFormat::read_chunks(
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

            std::string TxtFormat::read_stream_chunks(
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

            void TxtFormat::write(
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

            void TxtFormat::write_stream(
                std::ostream& stream,
                const std::string& content
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid content for text file");
                }
                
                stream << content;

                if (!stream) {
                    throw std::runtime_error("Failed to write to stream");
                }
            }

            void TxtFormat::write_chunk(
                std::ostream& stream,
                const std::string& chunk
            ) const {
                stream.write(chunk.c_str(), chunk.size());

                if (!stream) {
                    throw std::runtime_error("Failed to write chunk to stream");
                }
            }

            void TxtFormat::write_chunks(
                const std::string& filename,
                const std::string& content,
                size_t chunk_size
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid content for text file: " + filename);
                }

                std::ofstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Cannot open file for writing: " + filename);
                }

                write_stream_chunks(file, content, chunk_size);

                file.close();
            }

            void TxtFormat::write_stream_chunks(
                std::ostream& stream,
                const std::string& content,
                size_t chunk_size
            ) const {
                if (!validate(content)) {
                    throw std::runtime_error("Invalid content for stream chunks");
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

            bool TxtFormat::validate(
                const std::string& content
            ) const {
                return _parser.validate(content);
            }

            bool TxtFormat::validate_stream(
                std::istream& stream
            ) const {
                std::string content;
                std::string line;
                
                while (std::getline(stream, line)) {
                    content += line + "\n";
                }

                return validate(content);
            }
        
            FormatPtr TxtFormat::create(
                bool trim_lines,
                bool skip_empty_lines,
                bool split_by_whitespace
            ) {
                return std::make_shared<TxtFormat>(
                    trim_lines, skip_empty_lines, split_by_whitespace
                );
            }
        }
    }
}
