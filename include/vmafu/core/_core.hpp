// core/_core.hpp


#pragma once


#include <iostream>


#include "_Vector.hpp"
#include "_Matrix.hpp"
#include "_Function.hpp"


namespace vmafu {
    namespace core {
        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Vector<T>& vector);

        template <typename T>
        std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix);

        template<typename ResultT, typename... ArgTs>
        std::ostream& operator<<(
            std::ostream& os,
            const Function<ResultT, ArgTs...>& function
        );
    }
}


#include "detail/_core.ipp"
