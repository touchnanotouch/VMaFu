// parallel/mpi/mpi.hpp


#pragma once


#include "config/config.hpp"
#include "communication/communication.hpp"
#include "distribution/distribution.hpp"
#include "containers/containers.hpp"
#include "utils/utils.hpp"

#include "_mpi.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            // Config

            using config::init;
            using config::finalize;

            // Communication

            using communication::world;

            using communication::rank;
            using communication::size;

            using communication::barrier;
            using communication::broadcast;

            using communication::reduce;
            using communication::allreduce;

            using communication::allgather;

            using communication::Communicator;

            // Distribution

            using distribution::VectorDistributionType;
            using distribution::VectorDistributionInfo;
            using distribution::vector_distribution_info;

            using distribution::MatrixDistributionType;
            using distribution::MatrixDistributionInfo;
            using distribution::matrix_distribution_info;

            using distribution::scatter;
            using distribution::gather;

            // Containers

            using containers::MatrixMPI;
            using containers::VectorMPI;
            using containers::MultiplicationMethod;

            // _mpi.hpp

            using mpi::load_vector;
            using mpi::load_matrix;

            using mpi::save_vector;
            using mpi::save_matrix;
        }
    }
}


// _mpi.hpp

using vmafu::parallel::mpi::operator<<;
