// parallel/mpi/distribution/_distribution.hpp


#pragma once


#include <cmath>
#include <vector>
#include <stdexcept>

#include "../../../core/_Vector.hpp"
#include "../../../core/_Matrix.hpp"
#include "../communication/_communication.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace distribution {
                // Distributions enums

                enum class VectorDistributionType {
                    BLOCK,
                    CYCLIC
                };

                enum class MatrixDistributionType {
                    BLOCK_ROWS,
                    BLOCK_COLS,
                    BLOCK_2D,
                    CYCLIC_ROWS,
                    CYCLIC_COLS
                };

                // Distribution structs

                struct VectorDistributionInfo {
                    size_t global_size;

                    size_t local_size;

                    size_t offset;

                    VectorDistributionType type;
                };

                struct MatrixDistributionInfo {
                    size_t global_rows;
                    size_t global_cols;

                    size_t local_rows;
                    size_t local_cols;

                    size_t row_offset;
                    size_t col_offset;

                    int grid_rows;
                    int grid_cols;

                    int grid_row;
                    int grid_col;

                    MatrixDistributionType type;

                    bool is_global_element_owner(
                        size_t i,
                        size_t j
                    ) const;

                    std::pair<size_t, size_t> global_to_local(
                        size_t i,
                        size_t j
                    ) const;

                    std::pair<size_t, size_t> local_to_global(
                        size_t i_local,
                        size_t j_local
                    ) const;
                };

                // Factory methods

                inline VectorDistributionInfo vector_distribution_info(
                    VectorDistributionType type,
                    size_t size,
                    const communication::Communicator& comm
                );

                inline MatrixDistributionInfo matrix_distribution_info(
                    MatrixDistributionType type,
                    size_t rows,
                    size_t cols,
                    const communication::Communicator& comm
                );

                inline std::pair<int, int> process_grid(
                    int total_procs,
                    size_t rows,
                    size_t cols
                );

                // Distribution methods

                template <typename T>
                inline void scatter(
                    const vmafu::core::Vector<T>& global,
                    const VectorDistributionInfo& info,
                    vmafu::core::Vector<T>& local,
                    int root,
                    const communication::Communicator& comm
                );

                template <typename T>
                inline void scatter(
                    const vmafu::core::Matrix<T>& global,
                    const MatrixDistributionInfo& info,
                    vmafu::core::Matrix<T>& local,
                    int root,
                    const communication::Communicator& comm
                );

                template <typename T>
                inline void gather(
                    vmafu::core::Vector<T>& global,
                    const VectorDistributionInfo& info,
                    const vmafu::core::Vector<T>& local,
                    int root,
                    const communication::Communicator& comm
                );

                template <typename T>
                inline void gather(
                    vmafu::core::Matrix<T>& global,
                    const MatrixDistributionInfo& info,
                    const vmafu::core::Matrix<T>& local,
                    int root,
                    const communication::Communicator& comm
                );
            }
        }
    }
}


#include "detail/_distribution.ipp"
