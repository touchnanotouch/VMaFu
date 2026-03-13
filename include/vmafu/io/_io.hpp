// io/_io.hpp


#pragma once


#include <string>

#include "formats/formats.hpp"
#include "parsers/parsers.hpp"
#include "serializers/serializers.hpp"

#include "../core/_Vector.hpp"
#include "../core/_Matrix.hpp"


namespace vmafu {
    namespace io {
        FormatPtr create_format(const std::string& filename);

        ParserPtr create_parser(const std::string& filename);

        template <typename T>
        SerializerPtr<Matrix<T>> create_serializer(const Matrix<T>&);

        template <typename T>
        SerializerPtr<Vector<T>> create_serializer(const Vector<T>&);

        template <typename T>
        Vector<T> load_vector(const std::string& filename);

        template <typename T>
        Matrix<T> load_matrix(const std::string& filename);

        template <typename T>
        void save_vector(
            const std::string& filename,
            const Vector<T>& data
        );

        template <typename T>
        void save_matrix(
            const std::string& filename,
            const Matrix<T>& data
        );
    }

    using io::load_vector;
    using io::load_matrix;
    
    using io::save_vector;
    using io::save_matrix;
}


#include "detail/_io.ipp"
