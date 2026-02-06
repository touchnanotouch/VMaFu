// io/formats/_TxtFormat.hpp


#pragma once


#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "_IFormat.hpp"

#include "../parsers/_TxtParser.hpp"


namespace vmafu {
    namespace io {
        namespace formats {
            class TxtFormat : public IFormat {
                private:
                    parsers::TxtParser _parser;

                public:
                    // Constructor

                    explicit TxtFormat(
                        bool trim_lines = true,
                        bool skip_empty_lines = true,
                        bool split_by_whitespace = false
                    );

                    // Getter

                    const parsers::TxtParser& parser() const noexcept;

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

                    // Static method

                    static FormatPtr create(
                        bool trim_lines = true,
                        bool skip_empty_lines = true,
                        bool split_by_whitespace = false
                    );
            };
        }

        using formats::TxtFormat;
    }
}


#include "detail/_TxtFormat.ipp"
