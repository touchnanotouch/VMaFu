#pragma once

#include <mpi.h>
#include <vector>
#include <cstddef>

namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace internal {
                namespace data {
                    // Distributions enum 

                    enum class DistributionType {
                        BLOCK_ROWS,
                        BLOCK_COLS,
                        BLOCK_2D,        // ( для алгоритма Кэннона )
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

                    // Distribution methods

                    // Создание 2D сетки процессов
                    static std::pair<int, int> process_grid(
                        int total_procs,
                        size_t rows,
                        size_t cols
                    );

                    // Инициализация информации о распределении
                    static DistributionInfo distribution_info(
                        size_t rows,
                        size_t cols,
                        DistributionType type,
                        const Communicator& comm
                    );

                    // Распределение данных
                    template <typename T>
                    static void scatter(
                        const vmafu::Matrix<T>& global,
                        const DistributionInfo& info,
                        vmafu::Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    );

                    // Сбор данных
                    template <typename T>
                    static void gather(
                        vmafu::Matrix<T>& global,
                        const DistributionInfo& info,
                        const vmafu::Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    );
                }

                using data::process_grid, data::distribution_info;
                using data::gather, data::scatter;
            }

            using internal::data::DistributionType;
            using internal::data::DistributionInfo;
        }
    }
}


#include "detail/_distribution.ipp"
