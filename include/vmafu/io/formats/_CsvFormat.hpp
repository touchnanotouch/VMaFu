// io/formats/_CsvFormat.hpp


#pragma once


#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "_IFormat.hpp"

#include "../parsers/_CsvParser.hpp"


namespace vmafu {
    namespace io {
        namespace formats {
            class CsvFormat : public IFormat {
                private:
                    parsers::CsvParser _parser;

                public:
                    // Constructor

                    explicit CsvFormat(
                        char delimiter = ',',
                        bool trim_cells = true,
                        bool has_header = false
                    );
                    
                    // Getter

                    const parsers::CsvParser& parser() const noexcept;

                    // Read methods

                    std::string read(
                        const std::string& filename
                    ) const override;

                    std::string read_stream(
                        std::istream& stream
                    ) const override;

                    std::string read_chunk(
                        std::istream& stream,
                        size_t chunk_size = 2048
                    ) const override;

                    std::string read_chunks(
                        const std::string& filename,
                        size_t chunk_size = 2048
                    ) const override;

                    std::string read_stream_chunks(
                        std::istream& stream,
                        size_t chunk_size = 2048
                    ) const override;

                    // Write methods

                    void write(
                        const std::string& filename,
                        const std::string& content
                    ) const override;

                    void write_stream(
                        std::ostream& stream,
                        const std::string& content
                    ) const override;

                    void write_chunk(
                        std::ostream& stream,
                        const std::string& chunk
                    ) const override;

                    void write_chunks(
                        const std::string& filename,
                        const std::string& content,
                        size_t chunk_size = 2048
                    ) const override;

                    void write_stream_chunks(
                        std::ostream& stream,
                        const std::string& content,
                        size_t chunk_size = 2048
                    ) const override;

                    // Validation methods

                    bool validate(const std::string& content) const override;
                    bool validate_stream(std::istream& stream) const override;
            };
        }
    }
}


#include "detail/_CsvFormat.ipp"
