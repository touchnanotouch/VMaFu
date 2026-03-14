// parallel/mpi/containers/_MatrixMPI.hpp


#pragma once


#include <string>

#include "../../../core/_Matrix.hpp"

#include "../communication/communication.hpp"
#include "../distribution/distribution.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {            
            namespace containers {
                template <typename T>
                class MatrixMPI {
                    private:
                        vmafu::core::Matrix<T> _local_matrix;
                        distribution::MatrixDistributionInfo _dist_info;
                        communication::Communicator _comm;

                        // Friend methods

                        template <typename U>
                        friend MatrixMPI<U> load_matrix(
                            const std::string& filename,
                            distribution::MatrixDistributionType dist_type,
                            int root,
                            const communication::Communicator& comm
                        );

                        template <typename U>
                        friend void save_matrix(
                            const std::string& filename,
                            const MatrixMPI<U>& matrix,
                            int root,
                            const communication::Communicator& comm
                        );

                    public:
                        // Constructors

                        MatrixMPI();
                        MatrixMPI(const communication::Communicator& comm);

                        MatrixMPI(
                            const vmafu::core::Matrix<T>& global_matrix,
                            distribution::MatrixDistributionInfo dist_info,
                            int root = 0,
                            const communication::Communicator& comm = communication::world()
                        );

                        // Getters

                        const vmafu::core::Matrix<T>& local_matrix() const noexcept;
                        const distribution::MatrixDistributionInfo& distribution_info() const noexcept;
                        const communication::Communicator& communicator() const noexcept;

                        size_t local_rows() const noexcept;
                        size_t local_cols() const noexcept;

                        size_t global_rows() const noexcept;
                        size_t global_cols() const noexcept;

                        // Setters

                        void set_local_matrix(
                            const vmafu::core::Matrix<T>& matrix
                        );
                        void set_dist_info(
                            const distribution::MatrixDistributionInfo& dist_info
                        );
                        void set_comm(
                            const communication::Communicator& comm
                        );
                };
            }
        }
    }
}


#include "detail/_MatrixMPI.ipp"
