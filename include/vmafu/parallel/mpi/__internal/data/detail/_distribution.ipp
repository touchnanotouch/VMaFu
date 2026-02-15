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
                            case DistributionType::BLOCK_ROWS: {
                                return (
                                    i >= row_offset && \
                                    i < row_offset + local_rows
                                );
                            }
                            case DistributionType::BLOCK_COLS: {
                                return (
                                    j >= col_offset && \
                                    j < col_offset + local_cols
                                );
                            }
                            case DistributionType::BLOCK_2D: {
                                return (
                                    i >= row_offset && \
                                    i < row_offset + local_rows && \
                                    j >= col_offset && \
                                    j < col_offset + local_cols
                                );
                            }
                            default: {
                                return false;
                            }
                        }
                    }

                    std::pair<size_t, size_t> DistributionInfo::global_to_local(
                        size_t i,
                        size_t j
                    ) const {
                        if (!is_global_element_owner(i, j)) {
                            throw std::out_of_range(
                                "global_to_local(): Global element not owned by this process"
                            );
                        }

                        return {i - row_offset, j - col_offset};
                    }

                    std::pair<size_t, size_t> DistributionInfo::local_to_global(
                        size_t i_local,
                        size_t j_local
                    ) const {
                        if (i_local >= local_rows || j_local >= local_cols) {
                            throw std::out_of_range(
                                "local_to_global(): Local index out of range"
                            );
                        }

                        return {row_offset + i_local, col_offset + j_local};
                    }

                    // Factory methods

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
                        DistributionType type,
                        size_t rows,
                        size_t cols,
                        const Communicator& comm
                    ) {
                        int rank = comm.rank();
                        int size = comm.size();

                        DistributionInfo info;

                        info.type = type;

                        info.global_rows = rows;
                        info.global_cols = cols;

                        switch(type) {
                            case DistributionType::BLOCK_ROWS: {
                                size_t block_size = rows / size;
                                size_t remainder = rows % size;
                                
                                info.local_rows = block_size + (
                                    rank < remainder ? 1 : 0
                                );
                                info.local_cols = cols;

                                info.row_offset = 0;
                                for (int i = 0; i < rank; i++) {
                                    size_t prev_block = rows / size + (
                                        i < remainder ? 1 : 0
                                    );

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
                                size_t block_size = cols / size;
                                size_t remainder = cols % size;

                                info.local_rows = rows;
                                info.local_cols = block_size + (
                                    rank < remainder ? 1 : 0
                                );

                                info.col_offset = 0;
                                for (int i = 0; i < rank; i++) {
                                    size_t prev_block = cols / size + (
                                        i < remainder ? 1 : 0
                                    );

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
                                auto grids = process_grid(size, rows, cols);

                                auto grid_rows = grids.first;
                                auto grid_cols = grids.second;

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

                                info.local_rows = rows_per_proc + (
                                    info.grid_row < rows_remainder ? 1 : 0
                                );

                                info.local_cols = cols_per_proc + (
                                    info.grid_col < cols_remainder ? 1 : 0
                                );

                                // Вычисляем смещения

                                size_t row_min = std::min<size_t>(
                                    info.grid_row, rows_remainder
                                );

                                size_t col_min = std::min<size_t>(
                                    info.grid_col, cols_remainder
                                );

                                info.row_offset = (
                                    info.grid_row * rows_per_proc + row_min
                                );

                                info.col_offset = (
                                    info.grid_col * cols_per_proc + col_min
                                );

                                break;
                            }
                            case DistributionType::CYCLIC_ROWS: {
                                // TODO: реализовать ( затычка )

                                throw std::runtime_error(
                                    "distribution_info(): CYCLIC_ROWS distribution not implemented yet"
                                );

                                break;
                            }
                            case DistributionType::CYCLIC_COLS: {
                                // TODO: реализовать ( затычка )

                                throw std::runtime_error(
                                    "distribution_info(): CYCLIC_COLS distribution not implemented yet"
                                );

                                break;
                            }
                            default: {
                                throw std::invalid_argument(
                                    "distribution_info(): Unsupported distribution type"
                                );
                            }
                        }
                        
                        return info;
                    }

                    // Distribution methods

                    template <typename T>
                    void scatter(
                        const Matrix<T>& global,
                        const DistributionInfo& info,
                        Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    ) {
                        int rank = comm.rank();
                        int size = comm.size();

                        if (rank == root) {
                            if (
                                global.rows() != info.global_rows || \
                                global.cols() != info.global_cols
                            ) {
                                throw std::invalid_argument(
                                    "scatter(): Global matrix dimensions do not match distribution info"
                                );
                            }
                        }

                        local = Matrix<T>(info.local_rows, info.local_cols);

                        // Получаем информацию о распределении от всех процессов

                        std::vector<size_t> all_local_rows, all_local_cols;
                        std::vector<size_t> all_row_offsets, all_col_offsets;

                        all_local_rows.resize(size);
                        all_local_cols.resize(size);

                        all_row_offsets.resize(size);
                        all_col_offsets.resize(size);

                        // Собираем информацию о распределении от всех процессов

                        MPI_Allgather(
                            &info.local_rows,
                            1,
                            Communicator::mpi_type<T>(),
                            all_local_rows.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.local_cols,
                            1,
                            Communicator::mpi_type<T>(),
                            all_local_cols.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.row_offset,
                            1,
                            Communicator::mpi_type<T>(),
                            all_row_offsets.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.col_offset,
                            1,
                            Communicator::mpi_type<T>(),
                            all_col_offsets.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        // Рассчитываем sendcounts и displs на основе информации о распределении

                        std::vector<int> sendcounts(size);
                        std::vector<int> displs(size);

                        for (int i = 0; i < size; i++) {
                            switch(info.type) {
                                case DistributionType::BLOCK_ROWS: {
                                    sendcounts[i] = static_cast<int>(all_local_rows[i] * info.global_cols);
                                    displs[i] = static_cast<int>(all_row_offsets[i] * info.global_cols);

                                    break;
                                }
                                case DistributionType::BLOCK_COLS: {
                                    sendcounts[i] = static_cast<int>(info.global_rows * all_local_cols[i]);
                                    displs[i] = static_cast<int>(all_col_offsets[i] * info.global_rows);

                                    break;
                                }
                                case DistributionType::BLOCK_2D: {
                                    sendcounts[i] = static_cast<int>(all_local_rows[i] * all_local_cols[i]);
                                    displs[i] = static_cast<int>(all_row_offsets[i] * info.global_cols + all_col_offsets[i]);

                                    break;
                                }
                                case DistributionType::CYCLIC_ROWS: {
                                    // TODO: реализовать ( затычка )

                                    throw std::runtime_error(
                                        "scatter(): CYCLIC_ROWS scatter not implemented yet"
                                    );

                                    break;
                                }
                                case DistributionType::CYCLIC_COLS: {
                                    // TODO: реализовать ( затычка )

                                    throw std::runtime_error(
                                        "scatter(): CYCLIC_COLS scatter not implemented yet"
                                    );

                                    break;
                                }
                                default: {
                                    throw std::runtime_error(
                                        "scatter(): Unknown distribution type"
                                    );
                                }
                            }
                        }

                        if (info.type == DistributionType::BLOCK_2D) {                            
                            if (rank == root) {
                                // Root процесс рассылает данные всем

                                for (int dest = 0; dest < size; dest++) {
                                    if (dest == rank) {
                                        // Копируем данные для себя

                                        for (size_t i = 0; i < info.local_rows; i++) {
                                            for (size_t j = 0; j < info.local_cols; j++) {
                                                auto globals = info.local_to_global(i, j);

                                                auto global_i = globals.first;
                                                auto global_j = globals.second;

                                                local(i, j) = global(global_i, global_j);
                                            }
                                        }
                                    } else if (sendcounts[dest] > 0) {
                                        // Создаем буфер для отправки

                                        std::vector<T> send_buffer(sendcounts[dest]);

                                        // Копируем подматрицу в буфер

                                        size_t rows = all_local_rows[dest];
                                        size_t cols = all_local_cols[dest];
                                        size_t row_start = all_row_offsets[dest];
                                        size_t col_start = all_col_offsets[dest];

                                        for (size_t i = 0; i < rows; i++) {
                                            for (size_t j = 0; j < cols; j++) {
                                                send_buffer[i * cols + j] = global(row_start + i, col_start + j);
                                            }
                                        }

                                        // Отправляем буфер

                                        MPI_Send(
                                            send_buffer.data(),
                                            sendcounts[dest],
                                            Communicator::mpi_type<T>(),
                                            dest,
                                            0,
                                            comm.get()
                                        );
                                    }
                                }
                            } else if (sendcounts[rank] > 0) {
                                // Получаем данные от root процесса

                                std::vector<T> recv_buffer(sendcounts[rank]);

                                MPI_Recv(
                                    recv_buffer.data(),
                                    sendcounts[rank],
                                    Communicator::mpi_type<T>(),
                                    root,
                                    0,
                                    comm.get(),
                                    MPI_STATUS_IGNORE
                                );

                                // Копируем из буфера в локальную матрицу

                                for (size_t i = 0; i < info.local_rows; i++) {
                                    for (size_t j = 0; j < info.local_cols; j++) {
                                        local(i, j) = recv_buffer[i * info.local_cols + j];
                                    }
                                }
                            }
                        } else {
                            // Для BLOCK_ROWS и BLOCK_COLS используем стандартный Scatterv

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
                        }
                    }

                    template <typename T>
                    void gather(
                        Matrix<T>& global,
                        const DistributionInfo& info,
                        const Matrix<T>& local,
                        int root,
                        const Communicator& comm
                    ) {
                        int rank = comm.rank();
                        int size = comm.size();

                        if (
                            local.rows() != info.local_rows || \
                            local.cols() != info.local_cols
                        ) {
                            throw std::invalid_argument(
                                "gather(): Local matrix dimensions do not match distribution info"
                            );
                        }

                        // Получаем информацию о распределении от всех процессов

                        std::vector<size_t> all_local_rows, all_local_cols;
                        std::vector<size_t> all_row_offsets, all_col_offsets;

                        all_local_rows.resize(size);
                        all_local_cols.resize(size);

                        all_row_offsets.resize(size);
                        all_col_offsets.resize(size);

                        // Собираем информацию о распределении от всех процессов

                        MPI_Allgather(
                            &info.local_rows,
                            1,
                            Communicator::mpi_type<T>(),
                            all_local_rows.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.local_cols,
                            1,
                            Communicator::mpi_type<T>(),
                            all_local_cols.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.row_offset,
                            1,
                            Communicator::mpi_type<T>(),
                            all_row_offsets.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        MPI_Allgather(
                            &info.col_offset,
                            1,
                            Communicator::mpi_type<T>(),
                            all_col_offsets.data(),
                            1,
                            Communicator::mpi_type<T>(),
                            comm.get()
                        );

                        std::vector<int> recvcounts(size);
                        std::vector<int> displs(size);

                        for (int i = 0; i < size; i++) {
                            switch(info.type) {
                                case DistributionType::BLOCK_ROWS: {
                                    recvcounts[i] = static_cast<int>(all_local_rows[i] * info.global_cols);
                                    displs[i] = static_cast<int>(all_row_offsets[i] * info.global_cols);

                                    break;
                                }
                                case DistributionType::BLOCK_COLS: {
                                    recvcounts[i] = static_cast<int>(info.global_rows * all_local_cols[i]);
                                    displs[i] = static_cast<int>(all_col_offsets[i] * info.global_rows);

                                    break;
                                }
                                case DistributionType::BLOCK_2D: {
                                    recvcounts[i] = static_cast<int>(all_local_rows[i] * all_local_cols[i]);
                                    displs[i] = static_cast<int>(all_row_offsets[i] * info.global_cols + all_col_offsets[i]);

                                    break;
                                }
                                case DistributionType::CYCLIC_ROWS: {
                                    // TODO: реализовать ( затычка )

                                    throw std::runtime_error(
                                        "gather(): CYCLIC_ROWS gather not implemented yet"
                                    );

                                    break;
                                }
                                case DistributionType::CYCLIC_COLS: {
                                    // TODO: реализовать ( затычка )

                                    throw std::runtime_error(
                                        "gather(): CYCLIC_COLS gather not implemented yet"
                                    );

                                    break;
                                }
                                default: {
                                    throw std::runtime_error(
                                        "gather(): Unknown distribution type"
                                    );
                                }
                            }
                        }

                        if (info.type == DistributionType::BLOCK_2D) {                            
                            if (rank == root) {
                                // Создаем глобальную матрицу

                                global = Matrix<T>(
                                    info.global_rows, info.global_cols
                                );
                                
                                // Сначала обрабатываем локальные данные

                                for (size_t i = 0; i < info.local_rows; i++) {
                                    for (size_t j = 0; j < info.local_cols; j++) {
                                            auto globals = info.local_to_global(i, j);

                                            auto global_i = globals.first;
                                            auto global_j = globals.second;

                                        global(global_i, global_j) = local(i, j);
                                    }
                                }
                                
                                // Затем получаем данные от других процессов

                                for (int src = 0; src < size; src++) {
                                    if (src != rank && recvcounts[src] > 0) {
                                        // Создаем буфер для приема

                                        std::vector<T> recv_buffer(recvcounts[src]);

                                        // Получаем данные

                                        MPI_Recv(
                                            recv_buffer.data(),
                                            recvcounts[src],
                                            Communicator::mpi_type<T>(),
                                            src,
                                            0,
                                            comm.get(),
                                            MPI_STATUS_IGNORE
                                        );

                                        // Копируем из буфера в глобальную матрицу

                                        size_t rows = all_local_rows[src];
                                        size_t cols = all_local_cols[src];
                                        size_t row_start = all_row_offsets[src];
                                        size_t col_start = all_col_offsets[src];
                                        
                                        for (size_t i = 0; i < rows; i++) {
                                            for (size_t j = 0; j < cols; j++) {
                                                global(row_start + i, col_start + j) = recv_buffer[i * cols + j];
                                            }
                                        }
                                    }
                                }
                            } else if (recvcounts[rank] > 0) {
                                // Создаем буфер для отправки

                                std::vector<T> send_buffer(recvcounts[rank]);
                                
                                // Копируем локальную матрицу в буфер

                                for (size_t i = 0; i < info.local_rows; i++) {
                                    for (size_t j = 0; j < info.local_cols; j++) {
                                        send_buffer[i * info.local_cols + j] = local(i, j);
                                    }
                                }
                                
                                // Отправляем данные на root процесс

                                MPI_Send(
                                    send_buffer.data(),
                                    recvcounts[rank],
                                    Communicator::mpi_type<T>(),
                                    root,
                                    0,
                                    comm.get()
                                );
                            }
                        } else {
                            // Для BLOCK_ROWS и BLOCK_COLS используем стандартный Gatherv

                            if (rank == root) {
                                global = Matrix<T>(
                                    info.global_rows, info.global_cols
                                );
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
                        }
                    }
                }
            }
        }
    }
}
