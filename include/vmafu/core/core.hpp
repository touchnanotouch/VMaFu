// core/core.hpp


#pragma once


#include "_Vector.hpp"
#include "_Matrix.hpp"
#include "_Function.hpp"

#include "_core.hpp"


namespace vmafu {
    // Core

    using core::Vector;
    using core::Matrix;
    using core::Function;

    // Aliases

    template <typename T>
    using Function1D = Function<T, T>;
    
    template <typename T>
    using Function2D = Function<T, T, T>;
    
    template <typename T>  
    using Function3D = Function<T, T, T, T>;
}


// _core.hpp

using vmafu::core::operator<<;
