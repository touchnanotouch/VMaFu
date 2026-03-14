// parallel/mpi/linalg/_operations.hpp


#pragma once


#include <cmath>

#include "../../../core/_Vector.hpp"
#include "../../../core/_Matrix.hpp"

#include "../communication/_communication.hpp"
#include "../distribution/_distribution.hpp"
#include "../containers/_VectorMPI.hpp"
#include "../containers/_MatrixMPI.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace linalg {
                template <typename T>
                containers::MatrixMPI<T> multiply(
                    const containers::MatrixMPI<T>& A,
                    const containers::MatrixMPI<T>& B,
                    int root = 0,
                    const communication::Communicator& comm = communication::world()
                );

                template <typename T>
                containers::VectorMPI<T> multiply(
                    const containers::MatrixMPI<T>& A,
                    const containers::VectorMPI<T>& x,
                    int root = 0,
                    const communication::Communicator& comm = communication::world()
                );

                template <typename T>
                containers::VectorMPI<T> multiply(
                    const containers::VectorMPI<T>& x,
                    const containers::MatrixMPI<T>& A,
                    int root = 0,
                    const communication::Communicator& comm = communication::world()
                );
            }
        }
    }
}


#include "detail/_operations.ipp"
