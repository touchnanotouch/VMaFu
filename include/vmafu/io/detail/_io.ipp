// io/detail/_io.ipp


namespace vmafu {
    namespace io {
        FormatPtr create_format(const std::string& filename) {
            size_t dot_pos = filename.find_last_of('.');

            if (dot_pos != std::string::npos) {
                std::string ext = filename.substr(dot_pos);
                
                if (ext == ".csv" || ext == ".CSV") {
                    return CsvFormat::create();
                } else if (
                    ext == ".txt" || ext == ".TXT" || \
                    ext == ".dat" || ext == ".DAT"
                ) {
                    return TxtFormat::create();
                }
            }

            return TxtFormat::create();
        }

        ParserPtr create_parser(const std::string& filename) {
            size_t dot_pos = filename.find_last_of('.');

            if (dot_pos != std::string::npos) {
                std::string ext = filename.substr(dot_pos);
                
                if (ext == ".csv" || ext == ".CSV") {
                    return CsvParser::create();
                } else if (
                    ext == ".txt" || ext == ".TXT" || \
                    ext == ".dat" || ext == ".DAT"
                ) {
                    return TxtParser::create();
                }
            }

            return TxtParser::create();
        }

        template <typename T>
        SerializerPtr<Matrix<T>> create_serializer(const Matrix<T>&) {
            return MatrixSerializer<T>::create();
        }

        template <typename T>
        SerializerPtr<Vector<T>> create_serializer(const Vector<T>&) {
            return VectorSerializer<T>::create();
        }

        template <typename T>
        Vector<T> load_vector(const std::string& filename) {
            FormatPtr format = create_format(filename);
            ParserPtr parser = create_parser(filename);

            auto serializer = create_serializer(Vector<T>());

            return serializer->deserialize(
                parser->parse(format->read(filename))
            );
        }

        template <typename T>
        Matrix<T> load_matrix(const std::string& filename) {
            FormatPtr format = create_format(filename);
            ParserPtr parser = create_parser(filename);

            auto serializer = create_serializer(Matrix<T>());

            return serializer->deserialize(
                parser->parse(format->read(filename))
            );
        }

        template <typename T>
        void save_vector(
            const std::string& filename,
            const Vector<T>& data
        ) {
            FormatPtr format = create_format(filename);
            ParserPtr parser = create_parser(filename);

            auto serializer = create_serializer(data);
            
            format->write(
                filename, parser->unparse(serializer->serialize(data))
            );
        }

        template <typename T>
        void save_matrix(
            const std::string& filename,
            const Matrix<T>& data
        ) {
            FormatPtr format = create_format(filename);
            ParserPtr parser = create_parser(filename);

            auto serializer = create_serializer(data);

            format->write(
                filename, parser->unparse(serializer->serialize(data))
            );
        }

        template <typename T>
        Vector<T> load(
            const std::string& filename,
            const Vector<T>&
        ) {
            return load_vector<T>(filename);
        }

        template <typename T>
        Matrix<T> load(
            const std::string& filename,
            const Matrix<T>&
        ) {
            return load_matrix<T>(filename);
        }

        template <typename T>
        T load(const std::string& filename) {
            return load(filename, T{});
        }

        template <typename T>
        void save(const std::string& filename, const T& data) {
            FormatPtr format = create_format(filename);
            ParserPtr parser = create_parser(filename);

            auto serializer = create_serializer(data);

            format->write(
                filename, parser->unparse(serializer->serialize(data))
            );
        }
    }
}
