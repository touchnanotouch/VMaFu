// core/_core.hpp


#pragma once


#include "_Vector.hpp"
#include "_Matrix.hpp"
// #include "_Function.hpp"


namespace vmafu {
    namespace core {
        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Vector<T>& vector);

        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix);
    }

    using core::Vector;
    using core::Matrix;
}


using vmafu::core::operator<<;


#include "detail/_core.ipp"
