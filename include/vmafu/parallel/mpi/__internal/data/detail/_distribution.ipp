// parallel/mpi/__internal/data/detail/_distribution.ipp


#pragma once


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace internal {
                namespace data {
                    // Distribution struct

                    bool DistributionInfo::is_global_element_owner(
                        size_t i, size_t j
                    ) const {
                        switch(type) {
                            case DistributionType::BLOCK_ROWS:
                                return (i >= row_offset && i < row_offset + local_rows);
                            case DistributionType::BLOCK_COLS:
                                return (j >= col_offset && j < col_offset + local_cols);
                            case DistributionType::BLOCK_2D:
                                return (
                                    i >= row_offset && i < row_offset + local_rows && \
                                    j >= col_offset && j < col_offset + local_cols
                                );
                            default:
                                return false;
                        }
                    }

                    std::pair<size_t, size_t> DistributionInfo::global_to_local(
                        size_t i,
                        size_t j
                    ) const {
                        if (!is_global_element_owner(i, j)) {
                            throw std::out_of_range("Global element not owned by this process");
                        }

                        return {i - row_offset, j - col_offset};
                    }

                    std::pair<size_t, size_t> DistributionInfo::local_to_global(
                        size_t i_local,
                        size_t j_local
                    ) const {
                        if (i_local >= local_rows || j_local >= local_cols) {
                            throw std::out_of_range("Local index out of range");
                        }

                        return {row_offset + i_local, col_offset + j_local};
                    }

                    // Distribution methods

                    std::pair<int, int> process_grid(
                        int total_procs,
                        size_t rows,
                        size_t cols
                    ) {
                        int grid_rows = static_cast<int>(
                            std::sqrt(total_procs)
                        );

                        while (total_procs % grid_rows != 0) {
                            grid_rows--;
                        }

                        int grid_cols = total_procs / grid_rows;

                        return {grid_rows, grid_cols};
                    }

                    DistributionInfo distribution_info(
                        size_t rows,
                        size_t cols,
                        DistributionType type,
                        const Communicator& comm
                    ) {
                        DistributionInfo info;
                        info.global_rows = rows;
                        info.global_cols = cols;
                        info.type = type;
                        
                        int rank = comm.rank();
                        int size = comm.size();
                        
                        switch(type) {
                            case DistributionType::BLOCK_ROWS: {
                                size_t block_size = rows / size;
                                size_t remainder = rows % size;
                                
                                info.local_rows = block_size + (rank < remainder ? 1 : 0);
                                info.local_cols = cols;

                                info.row_offset = 0;
                                
                                for (int i = 0; i < rank; i++) {
                                    size_t prev_block = rows / size + (i < remainder ? 1 : 0);
                                    info.row_offset += prev_block;
                                }
                                
                                info.col_offset = 0;
                                info.grid_rows = size;
                                info.grid_cols = 1;
                                info.grid_row = rank;
                                info.grid_col = 0;
                                break;
                            }
                                
                            case DistributionType::BLOCK_COLS: {
                                // Блочное распределение по столбцам
                                size_t block_size = cols / size;
                                size_t remainder = cols % size;
                                
                                info.local_rows = rows;
                                info.local_cols = block_size + (rank < remainder ? 1 : 0);
                                info.col_offset = 0;
                                
                                for (int i = 0; i < rank; i++) {
                                    size_t prev_block = cols / size + (i < remainder ? 1 : 0);
                                    info.col_offset += prev_block;
                                }
                                
                                info.row_offset = 0;
                                info.grid_rows = 1;
                                info.grid_cols = size;
                                info.grid_row = 0;
                                info.grid_col = rank;
                                break;
                            }
                                
                            case DistributionType::BLOCK_2D: {
                                // 2D блочное распределение
                                auto [grid_rows, grid_cols] = process_grid(size, rows, cols);
                                info.grid_rows = grid_rows;
                                info.grid_cols = grid_cols;
                                
                                // Вычисляем координаты процесса в сетке
                                info.grid_row = rank / grid_cols;
                                info.grid_col = rank % grid_cols;
                                
                                // Распределяем строки
                                size_t rows_per_proc = rows / grid_rows;
                                size_t rows_remainder = rows % grid_rows;
                                
                                size_t cols_per_proc = cols / grid_cols;
                                size_t cols_remainder = cols % grid_cols;
                                
                                info.local_rows = rows_per_proc + (info.grid_row < rows_remainder ? 1 : 0);
                                info.local_cols = cols_per_proc + (info.grid_col < cols_remainder ? 1 : 0);
                                
                                // Вычисляем смещения
                                info.row_offset = info.grid_row * rows_per_proc + 
                                                std::min<size_t>(info.grid_row, rows_remainder);
                                info.col_offset = info.grid_col * cols_per_proc + 
                                                std::min<size_t>(info.grid_col, cols_remainder);
                                break;
                            }
                                
                            default:
                                throw std::invalid_argument("Unsupported distribution type");
                        }
                        
                        return info;
                    }

                    template <typename T>
                    void scatter(
                        const vmafu::Matrix<T>& global,
                        vmafu::Matrix<T>& local,
                        const DistributionInfo& info,
                        const Communicator& comm,
                        int root
                    ) {
                        int rank = comm.rank();
                        int size = comm.size();

                        if (rank == root) {
                            if (global.rows() != info.global_rows || global.cols() != info.global_cols) {
                                throw std::invalid_argument("Global matrix dimensions do not match distribution info");
                            }
                        }

                        switch(info.type) {
                            case DistributionType::BLOCK_ROWS: {
                                std::vector<int> sendcounts(size, 0);
                                std::vector<int> displs(size, 0);
                                
                                size_t block_size = info.global_rows / size;
                                size_t remainder = info.global_rows % size;
                                
                                size_t offset = 0;
                                for (int i = 0; i < size; i++) {
                                    size_t local_rows = block_size + (i < remainder ? 1 : 0);

                                    sendcounts[i] = static_cast<int>(local_rows * info.global_cols);
                                    displs[i] = static_cast<int>(offset * info.global_cols);

                                    offset += local_rows;
                                }

                                local = vmafu::Matrix<T>(info.local_rows, info.local_cols);

                                MPI_Scatterv(
                                    (rank == root) ? const_cast<T*>(global.data()) : nullptr,
                                    sendcounts.data(),
                                    displs.data(),
                                    Communicator::mpi_type<T>(),
                                    local.data(),
                                    static_cast<int>(info.local_rows * info.local_cols),
                                    Communicator::mpi_type<T>(),
                                    root,
                                    comm.get()
                                );
                                break;
                            }
                                
                            case DistributionType::BLOCK_COLS: {
                                std::vector<int> sendcounts(size, 0);
                                std::vector<int> displs(size, 0);
                                
                                size_t block_size = info.global_cols / size;
                                size_t remainder = info.global_cols % size;
                                
                                size_t offset = 0;
                                for (int i = 0; i < size; i++) {
                                    size_t local_cols = block_size + (i < remainder ? 1 : 0);
                                    sendcounts[i] = static_cast<int>(info.global_rows * local_cols);
                                    displs[i] = static_cast<int>(offset * info.global_rows);
                                    offset += local_cols;
                                }

                                local = vmafu::Matrix<T>(info.local_rows, info.local_cols);

                                MPI_Scatterv(
                                    (rank == root) ? const_cast<T*>(global.data()) : nullptr,
                                    sendcounts.data(),
                                    displs.data(),
                                    Communicator::mpi_type<T>(),
                                    local.data(),
                                    static_cast<int>(info.local_rows * info.local_cols),
                                    Communicator::mpi_type<T>(),
                                    root,
                                    comm.get()
                                );
                                break;
                            }
                                
                            case DistributionType::CYCLIC_ROWS: {
                                // Циклическое распределение требует специальной логики
                                // Здесь упрощенная версия - собираем/рассылаем всю матрицу
                                // В реальности нужен более сложный алгоритм
                                if (rank == root) {
                                    local = global;  // На корне вся матрица
                                } else {
                                    local = vmafu::Matrix<T>(info.local_rows, info.local_cols);
                                }
                                
                                // Рассылаем размеры
                                size_t dims[2] = {info.global_rows, info.global_cols};
                                comm.broadcast(dims, 2, root);
                                
                                // Рассылаем всю матрицу
                                if (rank == root) {
                                    for (int i = 1; i < size; i++) {
                                        // Отправляем соответствующие строки каждому процессу
                                        // TODO: Реализовать правильную циклическую рассылку
                                    }
                                }
                                throw std::runtime_error("CYCLIC_ROWS scatter not fully implemented");
                                break;
                            }
                                
                            default:
                                throw std::runtime_error("Scatter for this distribution type not yet implemented");
                        }
                    }

                    // Сбор локальных матриц в глобальную
                    template <typename T>
                    void gather(
                        const vmafu::Matrix<T>& local,
                        vmafu::Matrix<T>& global,
                        const DistributionInfo& info,
                        const Communicator& comm,
                        int root
                    ) {
                        int rank = comm.rank();
                        int size = comm.size();

                        if (local.rows() != info.local_rows || local.cols() != info.local_cols) {
                            throw std::invalid_argument("Local matrix dimensions do not match distribution info");
                        }

                        switch(info.type) {
                            case DistributionType::BLOCK_ROWS: {
                                std::vector<int> recvcounts(size, 0);
                                std::vector<int> displs(size, 0);
                                
                                size_t block_size = info.global_rows / size;
                                size_t remainder = info.global_rows % size;
                                
                                size_t offset = 0;
                                for (int i = 0; i < size; i++) {
                                    size_t local_rows = block_size + (i < remainder ? 1 : 0);
                                    recvcounts[i] = static_cast<int>(local_rows * info.global_cols);
                                    displs[i] = static_cast<int>(offset * info.global_cols);
                                    offset += local_rows;
                                }

                                if (rank == root) {
                                    global = vmafu::Matrix<T>(info.global_rows, info.global_cols);
                                }

                                MPI_Gatherv(
                                    const_cast<T*>(local.data()),
                                    static_cast<int>(info.local_rows * info.local_cols),
                                    Communicator::mpi_type<T>(),
                                    (rank == root) ? global.data() : nullptr,
                                    recvcounts.data(),
                                    displs.data(),
                                    Communicator::mpi_type<T>(),
                                    root,
                                    comm.get()
                                );
                                break;
                            }
                                
                            case DistributionType::BLOCK_COLS: {
                                std::vector<int> recvcounts(size, 0);
                                std::vector<int> displs(size, 0);
                                
                                size_t block_size = info.global_cols / size;
                                size_t remainder = info.global_cols % size;
                                
                                size_t offset = 0;
                                for (int i = 0; i < size; i++) {
                                    size_t local_cols = block_size + (i < remainder ? 1 : 0);
                                    recvcounts[i] = static_cast<int>(info.global_rows * local_cols);
                                    displs[i] = static_cast<int>(offset * info.global_rows);
                                    offset += local_cols;
                                }

                                if (rank == root) {
                                    global = vmafu::Matrix<T>(info.global_rows, info.global_cols);
                                }

                                MPI_Gatherv(
                                    const_cast<T*>(local.data()),
                                    static_cast<int>(info.local_rows * info.local_cols),
                                    Communicator::mpi_type<T>(),
                                    (rank == root) ? global.data() : nullptr,
                                    recvcounts.data(),
                                    displs.data(),
                                    Communicator::mpi_type<T>(),
                                    root,
                                    comm.get()
                                );
                                break;
                            }
                                
                            default:
                                throw std::runtime_error("Gather for this distribution type not yet implemented");
                        }
                    }
                }
            }
        }
    }
}
