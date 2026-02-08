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
                        Communicator _comm;
                        Matrix<T> _local_matrix;
                        DistributionInfo _dist_info;

                        // Helper methods

                        MatrixMPI<T> multiply_simple(const MatrixMPI<T>& other, int root) const;

                        // Friend methods

                        template <typename U>
                        friend MatrixMPI<U> load_matrix(
                            const std::string& filename,
                            DistributionType dist_type,
                            int root,
                            const Communicator& comm
                        );
                        
                        template <typename U>
                        friend void save_matrix(
                            const std::string& filename,
                            const MatrixMPI<U>& matrix,
                            int root,
                            const Communicator& comm
                        );

                    protected:
                        // Setters

                        void set_comm(const Communicator& comm);
                        void set_local_matrix(const Matrix<T>& matrix);
                        void set_dist_info(const DistributionInfo& info);
                        
                    public:
                        // Constructors

                        MatrixMPI();
                        MatrixMPI(const Communicator& comm);

                        MatrixMPI(
                            const Matrix<T>& global_matrix,
                            DistributionInfo dist_info,
                            int root = 0,
                            const Communicator& comm = world()
                        );

                        // Getters

                        const Communicator& communicator() const noexcept;
                        const Matrix<T>& local_matrix() const noexcept;
                        const DistributionInfo& dist_info() const noexcept;

                        size_t local_rows() const noexcept;
                        size_t local_cols() const noexcept;

                        size_t global_rows() const noexcept;
                        size_t global_cols() const noexcept;

                        // MPI opeators

                        // MatrixMPI<T> operator*(T scalar) const;
                        // MatrixMPI<T> operator+(const MatrixMPI<T>& other) const;
                        // MatrixMPI<T> operator-(const MatrixMPI<T>& other) const;

                        // MPI methods

                        MatrixMPI<T> multiply(
                            const MatrixMPI<T>& other,
                            MultiplicationMethod method = MultiplicationMethod::SIMPLE,
                            int root = 0
                        ) const;

                        // Static methods

                    };
            }

            using wrappers::MatrixMPI;
        }
    }
}


#include "detail/_MatrixMPI.ipp"
