// parallel/mpi/wrappers/core/MatrixMPI.hpp


#pragma once


#include "../../communication/_communicator.hpp"
#include "../../__internal/data/_distribution.hpp"

#include "../../../../core/_Matrix.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace wrappers {
                // Multiplications enum

                enum class MultiplicationMethod {
                    SIMPLE,          // Scatter-Gather
                    CANNON          // Алгоритм Кэннона (для квадратных)
                };

                // MatrixMPI class

                template <typename T>
                class MatrixMPI {
                    private:
                        vmafu::Matrix<T> _local_data;

                        Communicator _comm;
                        internal::DistributionInfo _dist_info;

                        // Helper methods

                        void _scatter_matrix(
                            const vmafu::Matrix<T>& global,
                            const Communicator& comm,
                            int root = 0
                        );

                        void _gather_matrix(
                            vmafu::Matrix<T>& global,
                            const Communicator& comm,
                            int root = 0
                        ) const;

                    public:
                        // Constructors

                        MatrixMPI();
                        MatrixMPI(const Communicator& comm);

                        MatrixMPI(
                            const vmafu::Matrix<T>& global_matrix,
                            const Communicator& comm,
                            internal::DistributionType dist_type = internal::DistributionType::BLOCK_ROWS,
                            int root = 0
                        );

                        // Getters

                        const vmafu::Matrix<T>& local_data() const noexcept;

                        size_t local_rows() const noexcept;
                        size_t local_cols() const noexcept;

                        size_t global_rows() const noexcept;
                        size_t global_cols() const noexcept;

                        // Gather method

                        vmafu::Matrix<T> gather(int root = 0) const;

                        // MPI opeators

                        MatrixMPI<T> operator*(T scalar) const;
                        MatrixMPI<T> operator+(const MatrixMPI<T>& other) const;
                        MatrixMPI<T> operator-(const MatrixMPI<T>& other) const;

                        // MPI methods

                        MatrixMPI<T> multiply(
                            const MatrixMPI<T>& other,
                            MultiplicationMethod method = MultiplicationMethod::SIMPLE
                        ) const;

                        // Static methods

                        static MatrixMPI<T> zeros(
                            size_t rows,
                            size_t cols,
                            const Communicator& comm,
                            internal::DistributionType dist_type = internal::DistributionType::BLOCK_ROWS
                        );

                        static MatrixMPI<T> identity(
                            size_t size,
                            const Communicator& comm,
                            internal::DistributionType dist_type = internal::DistributionType::BLOCK_ROWS
                        );
                    };
            }

            using wrappers::MatrixMPI;
        }
    }
}


#include "detail/_MatrixMPI.ipp"
