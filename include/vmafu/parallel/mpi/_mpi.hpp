// parallel/mpi/_mpi.hpp
 

#pragma once


#include <string>

#include "wrappers/wrappers.hpp"
#include "communication/communication.hpp"
#include "__internal/internal.hpp"

#include "../../core/_Matrix.hpp"
#include "../../io/_io.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            template <typename T>
            MatrixMPI<T> load_matrix(
                const std::string& filename,
                DistributionType dist_type = DistributionType::BLOCK_ROWS,
                int root = 0,
                const Communicator& comm = world()
            );

            template <typename T>
            void save_matrix(
                const std::string& filename,
                const wrappers::MatrixMPI<T>& matrix,
                int root = 0,
                const Communicator& comm = world()
            );
        }
    }
}


#include "detail/_mpi.ipp"
