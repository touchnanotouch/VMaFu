// io/formats/_IFormat.hpp


#pragma once


#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>


namespace vmafu {
    namespace io {
        namespace formats {
            class IFormat {
                public:
                    // Destructor

                    virtual ~IFormat() = default;

                    // Read methods

                    virtual std::string read(
                        const std::string& filename
                    ) const = 0;

                    virtual std::string read_stream(
                        std::istream& stream
                    ) const = 0;

                    virtual std::string read_chunk(
                        std::istream& stream,
                        size_t chunk_size = 2048
                    ) const = 0;

                    virtual std::string read_chunks(
                        const std::string& filename,
                        size_t chunk_size = 2048
                    ) const = 0;

                    virtual std::string read_stream_chunks(
                        std::istream& stream,
                        size_t chunk_size = 2048
                    ) const = 0;

                    // Write methods

                    virtual void write(
                        const std::string& filename,
                        const std::string& content
                    ) const = 0;

                    virtual void write_stream(
                        std::ostream& stream,
                        const std::string& content
                    ) const = 0;

                    virtual void write_chunk(
                        std::ostream& stream,
                        const std::string& chunk
                    ) const = 0;

                    virtual void write_chunks(
                        const std::string& filename,
                        const std::string& content,
                        size_t chunk_size = 2048
                    ) const = 0;

                    virtual void write_stream_chunks(
                        std::ostream& stream,
                        const std::string& content,
                        size_t chunk_size = 2048
                    ) const = 0;

                    // Validation methods

                    virtual bool validate(
                        const std::string& content
                    ) const = 0;

                    virtual bool validate_stream(
                        std::istream& stream
                    ) const = 0;
            };

            using FormatPtr = std::shared_ptr<IFormat>;
        }
    }
}
