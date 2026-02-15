// parallel/mpi/_mpi.hpp
 

#pragma once


#include <string>

#include "wrappers/core/_MatrixMPI.hpp"
#include "__internal/data/_distribution.hpp"

#include "../../core/_Matrix.hpp"
#include "../../core/_core.hpp"
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

            template <typename T>
            std::ostream& operator<<(std::ostream& os, const wrappers::MatrixMPI<T>& matrix);
        }
    }
}


using vmafu::parallel::mpi::operator<<;


#include "detail/_mpi.ipp"
