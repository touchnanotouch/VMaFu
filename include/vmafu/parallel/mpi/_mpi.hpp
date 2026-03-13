// parallel/mpi/_mpi.hpp
 

#pragma once

#include <string>
#include <iostream>

#include "../../core/_Vector.hpp"
#include "../../core/_Matrix.hpp"
#include "../../core/_core.hpp"
#include "../../io/_io.hpp"

#include "communication/_communication.hpp"
#include "distribution/_distribution.hpp"
#include "containers/_VectorMPI.hpp"
#include "containers/_MatrixMPI.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            template <typename T>
            containers::VectorMPI<T> load_vector(
                const std::string& filename,
                distribution::VectorDistributionType dist_type = distribution::VectorDistributionType::BLOCK,
                int root = 0,
                const communication::Communicator& comm = communication::world()
            );

            template <typename T>
            containers::MatrixMPI<T> load_matrix(
                const std::string& filename,
                distribution::MatrixDistributionType dist_type = distribution::MatrixDistributionType::BLOCK_ROWS,
                int root = 0,
                const communication::Communicator& comm = communication::world()
            );

            template <typename T>
            void save_vector(
                const std::string& filename,
                const containers::VectorMPI<T>& vector,
                int root = 0,
                const communication::Communicator& comm = communication::world()
            );

            template <typename T>
            void save_matrix(
                const std::string& filename,
                const containers::MatrixMPI<T>& matrix,
                int root = 0,
                const communication::Communicator& comm = communication::world()
            );

            template <typename T>
            std::ostream& operator<<(std::ostream& os, const containers::VectorMPI<T>& vector);

            template <typename T>
            std::ostream& operator<<(std::ostream& os, const containers::MatrixMPI<T>& matrix);
        }
    }
}


#include "detail/_mpi.ipp"
