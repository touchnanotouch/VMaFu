// parallel/mpi/__internal/data/_distribution.hpp


#pragma once


#include <mpi.h>
#include <cmath>
#include <vector>
#include <stdexcept>

#include "../../../../core/_Matrix.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace internal {
                namespace data {
                    // Distributions enum 

                    enum class DistributionType {
                        BLOCK_ROWS,
                        BLOCK_COLS,
                        BLOCK_2D,
                        CYCLIC_ROWS,
                        CYCLIC_COLS
                    };

                    // Distribution struct

                    struct DistributionInfo {
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

                        DistributionType type;

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

                    static std::pair<int, int> process_grid(
                        int total_procs,
                        size_t rows,
                        size_t cols
                    );

                    static DistributionInfo distribution_info(
                        DistributionType type,
                        size_t rows,
                        size_t cols,
                        const Communicator& comm
                    );

                    // Distribution methods

                    template <typename T>
                    static void scatter(
                        const Matrix<T>& global,
                        const DistributionInfo& info,
                        Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    );

                    template <typename T>
                    static void gather(
                        Matrix<T>& global,
                        const DistributionInfo& info,
                        const Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    );
                }

                using data::process_grid;
                using data::distribution_info;

                using data::gather;
                using data::scatter;
            }

            using internal::data::DistributionType;
            using internal::data::DistributionInfo;
        }
    }
}


#include "detail/_distribution.ipp"
