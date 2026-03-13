// parallel/mpi/containers/_MatrixMPI.hpp


#pragma once


#include "../../../core/_Matrix.hpp"
#include "../communication/communication.hpp"
#include "../distribution/distribution.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {            
            namespace containers {
                // Multiplications enum

                enum class MultiplicationMethod {
                    SIMPLE,          // Scatter-Gather
                    CANNON          // Алгоритм Кэннона (для квадратных)
                };

                // MatrixMPI class

                template <typename T>
                class MatrixMPI {
                    private:
                        communication::Communicator _comm;
                        Matrix<T> _local_matrix;
                        distribution::MatrixDistributionInfo _dist_info;

                        // Helper methods

                        MatrixMPI<T> multiply_simple(const MatrixMPI<T>& other, int root) const;

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

                    protected:
                        // Setters

                        void set_comm(const communication::Communicator& comm);
                        void set_local_matrix(const Matrix<T>& matrix);
                        void set_dist_info(const distribution::MatrixDistributionInfo& dist_info);
                        
                    public:
                        // Constructors

                        MatrixMPI();
                        MatrixMPI(const communication::Communicator& comm);

                        MatrixMPI(
                            const Matrix<T>& global_matrix,
                            distribution::MatrixDistributionInfo dist_info,
                            int root = 0,
                            const communication::Communicator& comm = communication::world()
                        );

                        // Getters

                        const communication::Communicator& communicator() const noexcept;
                        const Matrix<T>& local_matrix() const noexcept;
                        const distribution::MatrixDistributionInfo& distribution_info() const noexcept;

                        size_t local_rows() const noexcept;
                        size_t local_cols() const noexcept;

                        size_t global_rows() const noexcept;
                        size_t global_cols() const noexcept;

                        // MPI methods

                        MatrixMPI<T> multiply(
                            const MatrixMPI<T>& other,
                            MultiplicationMethod method = MultiplicationMethod::SIMPLE,
                            int root = 0
                        ) const;
                };
            }
        }
    }
}


#include "detail/_MatrixMPI.ipp"
