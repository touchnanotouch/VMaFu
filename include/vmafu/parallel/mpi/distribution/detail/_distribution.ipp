// parallel/mpi/distribution/detail/_distribution.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace distribution {
                // Struct methods

                bool MatrixDistributionInfo::is_global_element_owner(
                    size_t i, size_t j
                ) const {
                    switch(type) {
                        case MatrixDistributionType::BLOCK_ROWS: {
                            return (
                                i >= row_offset && i < row_offset + local_rows
                            );
                        }
                        case MatrixDistributionType::BLOCK_COLS: {
                            return (
                                j >= col_offset && j < col_offset + local_cols
                            );
                        }
                        case MatrixDistributionType::BLOCK_2D: {
                            return (
                                i >= row_offset && i < row_offset + local_rows &&
                                j >= col_offset && j < col_offset + local_cols
                            );
                        }
                        default: {
                            return false;
                        }
                    }
                }

                std::pair<size_t, size_t> MatrixDistributionInfo::global_to_local(
                    size_t i,
                    size_t j
                ) const {
                    if (!is_global_element_owner(i, j)) {
                        throw std::out_of_range(
                            "distribution::global_to_local(): Global element not owned by this process"
                        );
                    }

                    return {i - row_offset, j - col_offset};
                }

                std::pair<size_t, size_t> MatrixDistributionInfo::local_to_global(
                    size_t i_local,
                    size_t j_local
                ) const {
                    if (i_local >= local_rows || j_local >= local_cols) {
                        throw std::out_of_range(
                            "distribution::local_to_global(): Local index out of range"
                        );
                    }

                    return {row_offset + i_local, col_offset + j_local};
                }

                // Factory methods

                VectorDistributionInfo vector_distribution_info(
                    VectorDistributionType type,
                    size_t size,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int comm_size = comm.size();

                    VectorDistributionInfo info;

                    info.type = type;
                    info.global_size = size;

                    switch (type) {
                        case VectorDistributionType::BLOCK: {
                            size_t block_size = size / comm_size;
                            size_t remainder = size % comm_size;

                            info.local_size = block_size + (
                                rank < remainder ? 1 : 0
                            );
                            info.offset = 0;

                            for (int i = 0; i < rank; i++) {
                                info.offset += block_size + (
                                    i < remainder ? 1 : 0
                                );
                            }

                            break;
                        }
                        case VectorDistributionType::CYCLIC: {
                            // TODO: реализовать
                            throw std::runtime_error(
                                "distribution::vector_distribution_info(): CYCLIC not implemented yet"
                            );
                        }
                        default: {
                            throw std::invalid_argument(
                                "distribution::vector_distribution_info(): Unsupported distribution type"
                            );
                        }
                    }
                    
                    return info;
                }

                MatrixDistributionInfo matrix_distribution_info(
                    MatrixDistributionType type,
                    size_t rows,
                    size_t cols,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int size = comm.size();

                    MatrixDistributionInfo info;

                    info.type = type;
                    info.global_rows = rows;
                    info.global_cols = cols;

                    switch(type) {
                        case MatrixDistributionType::BLOCK_ROWS: {
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
                        case MatrixDistributionType::BLOCK_COLS: {
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
                        case MatrixDistributionType::BLOCK_2D: {
                            auto grids = process_grid(size, rows, cols);
                            auto grid_rows = grids.first;
                            auto grid_cols = grids.second;

                            info.grid_rows = grid_rows;
                            info.grid_cols = grid_cols;
                            info.grid_row = rank / grid_cols;
                            info.grid_col = rank % grid_cols;
                            
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

                            size_t row_min = std::min<size_t>(
                                info.grid_row, rows_remainder
                            );
                            size_t col_min = std::min<size_t>(
                                info.grid_col, cols_remainder
                            );

                            info.row_offset = info.grid_row * rows_per_proc + row_min;
                            info.col_offset = info.grid_col * cols_per_proc + col_min;

                            break;
                        }
                        case MatrixDistributionType::CYCLIC_ROWS: {
                            throw std::runtime_error(
                                "distribution::matrix_distribution_info(): CYCLIC_ROWS not implemented yet"
                            );
                        }
                        case MatrixDistributionType::CYCLIC_COLS: {
                            throw std::runtime_error(
                                "distribution::matrix_distribution_info(): CYCLIC_COLS not implemented yet"
                            );
                        }
                        default: {
                            throw std::invalid_argument(
                                "distribution::matrix_distribution_info(): Unsupported distribution type"
                            );
                        }
                    }
                    
                    return info;
                }

                std::pair<int, int> process_grid(
                    int total_procs,
                    size_t rows,
                    size_t cols
                ) {
                    int grid_rows = static_cast<int>(std::sqrt(total_procs));

                    while (total_procs % grid_rows != 0) {
                        grid_rows--;
                    }

                    int grid_cols = total_procs / grid_rows;

                    return {grid_rows, grid_cols};
                }

                // Distribution methods

                template <typename T>
                void scatter(
                    const vmafu::core::Vector<T>& global,
                    const VectorDistributionInfo& info,
                    vmafu::core::Vector<T>& local,
                    int root,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int comm_size = comm.size();

                    if (rank == root) {
                        if (global.size() != info.global_size) {
                            throw std::invalid_argument(
                                "distribution::scatter(): Global vector size does not match distribution info"
                            );
                        }
                    }

                    local = vmafu::core::Vector<T>(info.local_size);

                    vmafu::core::Vector<size_t> all_local_sizes(comm_size);
                    vmafu::core::Vector<size_t> all_offsets(comm_size);

                    vmafu::core::Vector<int> recvcounts(comm_size, 1);
                    vmafu::core::Vector<int> displs(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        displs[i] = i;
                    }

                    comm.allgatherv(
                        &info.local_size, 1, all_local_sizes.data(),
                        recvcounts.data(), displs.data()
                    );

                    comm.allgatherv(
                        &info.offset, 1, all_offsets.data(),
                        recvcounts.data(), displs.data()
                    );

                    vmafu::core::Vector<int> sendcounts(comm_size);
                    vmafu::core::Vector<int> scatter_displs(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        sendcounts[i] = static_cast<int>(all_local_sizes[i]);
                        scatter_displs[i] = static_cast<int>(all_offsets[i]);
                    }

                    comm.scatterv(
                        (rank == root) ? global.data() : nullptr,
                        sendcounts.data(),
                        scatter_displs.data(),
                        local.data(),
                        static_cast<int>(info.local_size),
                        root
                    );
                }

                template <typename T>
                void scatter(
                    const vmafu::core::Matrix<T>& global,
                    const MatrixDistributionInfo& info,
                    vmafu::core::Matrix<T>& local,
                    int root,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int size = comm.size();

                    if (rank == root) {
                        if (
                            global.rows() != info.global_rows || 
                            global.cols() != info.global_cols
                        ) {
                            throw std::invalid_argument(
                                "distribution::scatter(): Global matrix dimensions do not match distribution info"
                            );
                        }
                    }

                    local = vmafu::core::Matrix<T>(
                        info.local_rows, info.local_cols
                    );

                    vmafu::core::Vector<size_t> all_local_rows(size);
                    vmafu::core::Vector<size_t> all_local_cols(size);
                    vmafu::core::Vector<size_t> all_row_offsets(size);
                    vmafu::core::Vector<size_t> all_col_offsets(size);

                    vmafu::core::Vector<int> recvcounts(size, 1);
                    vmafu::core::Vector<int> displs(size);

                    for (int i = 0; i < size; i++) {
                        displs[i] = i;
                    }

                    comm.allgatherv(
                        &info.local_rows, 1, all_local_rows.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.local_cols, 1, all_local_cols.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.row_offset, 1, all_row_offsets.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.col_offset, 1, all_col_offsets.data(),
                        recvcounts.data(), displs.data()
                    );

                    vmafu::core::Vector<int> sendcounts(size);
                    vmafu::core::Vector<int> scatter_displs(size);

                    for (int i = 0; i < size; i++) {
                        switch(info.type) {
                            case MatrixDistributionType::BLOCK_ROWS: {
                                sendcounts[i] = static_cast<int>(
                                    all_local_rows[i] * info.global_cols
                                );
                                scatter_displs[i] = static_cast<int>(
                                    all_row_offsets[i] * info.global_cols
                                );
                                break;
                            }
                            case MatrixDistributionType::BLOCK_COLS: {
                                sendcounts[i] = static_cast<int>(
                                    info.global_rows * all_local_cols[i]
                                );
                                scatter_displs[i] = static_cast<int>(
                                    all_col_offsets[i] * info.global_rows
                                );
                                break;
                            }
                            case MatrixDistributionType::BLOCK_2D: {
                                sendcounts[i] = static_cast<int>(
                                    all_local_rows[i] * all_local_cols[i]
                                );
                                scatter_displs[i] = static_cast<int>(
                                    all_row_offsets[i] * info.global_cols + all_col_offsets[i]
                                );
                                break;
                            }
                            default: {
                                throw std::runtime_error(
                                    "distribution::scatter(): Unsupported distribution type"
                                );
                            }
                        }
                    }

                    if (info.type == MatrixDistributionType::BLOCK_2D) {                            
                        if (rank == root) {
                            // Root процесс рассылает данные всем

                            for (int dest = 0; dest < size; dest++) {
                                if (dest == rank) {
                                    for (size_t i = 0; i < info.local_rows; i++) {
                                        for (size_t j = 0; j < info.local_cols; j++) {
                                            auto globals = info.local_to_global(i, j);

                                            local(i, j) = global(globals.first, globals.second);
                                        }
                                    }
                                } else if (sendcounts[dest] > 0) {
                                    vmafu::core::Vector<T> send_buffer(sendcounts[dest]);

                                    size_t rows = all_local_rows[dest];
                                    size_t cols = all_local_cols[dest];
                                    size_t row_start = all_row_offsets[dest];
                                    size_t col_start = all_col_offsets[dest];

                                    for (size_t i = 0; i < rows; i++) {
                                        for (size_t j = 0; j < cols; j++) {
                                            send_buffer[i * cols + j] = global(row_start + i, col_start + j);
                                        }
                                    }

                                    comm.send(
                                        send_buffer.data(), sendcounts[dest],
                                        dest, 0
                                    );
                                }
                            }
                        } else if (sendcounts[rank] > 0) {
                            // Получаем данные от root процесса

                            vmafu::core::Vector<T> recv_buffer(sendcounts[rank]);

                            comm.recv(
                                recv_buffer.data(), sendcounts[rank],
                                root, 0
                            );

                            for (size_t i = 0; i < info.local_rows; i++) {
                                for (size_t j = 0; j < info.local_cols; j++) {
                                    local(i, j) = recv_buffer[i * info.local_cols + j];
                                }
                            }
                        }
                    } else {
                        // Для BLOCK_ROWS и BLOCK_COLS используем scatterv

                        comm.scatterv(
                            (rank == root) ? global.data() : nullptr,
                            sendcounts.data(),
                            scatter_displs.data(),
                            local.data(),
                            static_cast<int>(info.local_rows * info.local_cols),
                            root
                        );
                    }
                }

                template <typename T>
                void gather(
                    vmafu::core::Vector<T>& global,
                    const VectorDistributionInfo& info,
                    const vmafu::core::Vector<T>& local,
                    int root,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int comm_size = comm.size();

                    if (local.size() != info.local_size) {
                        throw std::invalid_argument(
                            "distribution::gather(): Local vector size does not match distribution info"
                        );
                    }

                    vmafu::core::Vector<size_t> all_local_sizes(comm_size);
                    vmafu::core::Vector<size_t> all_offsets(comm_size);

                    vmafu::core::Vector<int> recvcounts(comm_size, 1);
                    vmafu::core::Vector<int> displs(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        displs[i] = i;
                    }

                    comm.allgatherv(
                        &info.local_size, 1, all_local_sizes.data(),
                        recvcounts.data(), displs.data()
                    );

                    comm.allgatherv(
                        &info.offset, 1, all_offsets.data(),
                        recvcounts.data(), displs.data()
                    );

                    vmafu::core::Vector<int> gather_recvcounts(comm_size);
                    vmafu::core::Vector<int> gather_displs(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        gather_recvcounts[i] = static_cast<int>(all_local_sizes[i]);
                        gather_displs[i] = static_cast<int>(all_offsets[i]);
                    }

                    if (rank == root) {
                        global = vmafu::core::Vector<T>(info.global_size);
                    }

                    comm.gatherv(
                        local.data(),
                        static_cast<int>(info.local_size),
                        (rank == root) ? global.data() : nullptr,
                        gather_recvcounts.data(),
                        gather_displs.data(),
                        root
                    );
                }

                template <typename T>
                void gather(
                    Matrix<T>& global,
                    const MatrixDistributionInfo& info,
                    const Matrix<T>& local,
                    int root,
                    const communication::Communicator& comm
                ) {
                    int rank = comm.rank();
                    int size = comm.size();

                    if (local.rows() != info.local_rows || 
                        local.cols() != info.local_cols) {
                        throw std::invalid_argument(
                            "distribution::gather(): Local matrix dimensions do not match distribution info"
                        );
                    }

                    vmafu::core::Vector<size_t> all_local_rows(size);
                    vmafu::core::Vector<size_t> all_local_cols(size);
                    vmafu::core::Vector<size_t> all_row_offsets(size);
                    vmafu::core::Vector<size_t> all_col_offsets(size);

                    vmafu::core::Vector<int> recvcounts(size, 1);
                    vmafu::core::Vector<int> displs(size);

                    for (int i = 0; i < size; i++) {
                        displs[i] = i;
                    }

                    comm.allgatherv(
                        &info.local_rows, 1, all_local_rows.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.local_cols, 1, all_local_cols.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.row_offset, 1, all_row_offsets.data(),
                        recvcounts.data(), displs.data()
                    );
                    comm.allgatherv(
                        &info.col_offset, 1, all_col_offsets.data(),
                        recvcounts.data(), displs.data()
                    );

                    vmafu::core::Vector<int> gather_recvcounts(size);
                    vmafu::core::Vector<int> gather_displs(size);

                    for (int i = 0; i < size; i++) {
                        switch(info.type) {
                            case MatrixDistributionType::BLOCK_ROWS: {
                                gather_recvcounts[i] = static_cast<int>(
                                    all_local_rows[i] * info.global_cols
                                );
                                gather_displs[i] = static_cast<int>(
                                    all_row_offsets[i] * info.global_cols
                                );
                                break;
                            }
                            case MatrixDistributionType::BLOCK_COLS: {
                                gather_recvcounts[i] = static_cast<int>(
                                    info.global_rows * all_local_cols[i]
                                );
                                gather_displs[i] = static_cast<int>(
                                    all_col_offsets[i] * info.global_rows
                                );
                                break;
                            }
                            case MatrixDistributionType::BLOCK_2D: {
                                gather_recvcounts[i] = static_cast<int>(
                                    all_local_rows[i] * all_local_cols[i]
                                );
                                gather_displs[i] = static_cast<int>(
                                    all_row_offsets[i] * info.global_cols + all_col_offsets[i]
                                );
                                break;
                            }
                            default: {
                                throw std::runtime_error(
                                    "distribution::gather(): Unsupported distribution type"
                                );
                            }
                        }
                    }

                    if (info.type == MatrixDistributionType::BLOCK_2D) {
                        if (rank == root) {
                            global = Matrix<T>(info.global_rows, info.global_cols);
                            
                            // Сначала обрабатываем локальные данные

                            for (size_t i = 0; i < info.local_rows; i++) {
                                for (size_t j = 0; j < info.local_cols; j++) {
                                    auto globals = info.local_to_global(i, j);

                                    global(globals.first, globals.second) = local(i, j);
                                }
                            }
                            
                            // Затем получаем данные от других процессов

                            for (int src = 0; src < size; src++) {
                                if (src != rank && gather_recvcounts[src] > 0) {
                                    vmafu::core::Vector<T> recv_buffer(gather_recvcounts[src]);

                                    comm.recv(
                                        recv_buffer.data(), gather_recvcounts[src],
                                        src, 0
                                    );

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
                        } else if (gather_recvcounts[rank] > 0) {
                            vmafu::core::Vector<T> send_buffer(gather_recvcounts[rank]);
                            
                            for (size_t i = 0; i < info.local_rows; i++) {
                                for (size_t j = 0; j < info.local_cols; j++) {
                                    send_buffer[i * info.local_cols + j] = local(i, j);
                                }
                            }
                            
                            comm.send(
                                send_buffer.data(), gather_recvcounts[rank],
                                root, 0
                            );
                        }
                    } else {
                        if (rank == root) {
                            global = Matrix<T>(info.global_rows, info.global_cols);
                        }

                        comm.gatherv(
                            local.data(),
                            static_cast<int>(info.local_rows * info.local_cols),
                            (rank == root) ? global.data() : nullptr,
                            gather_recvcounts.data(),
                            gather_displs.data(),
                            root
                        );
                    }
                }
            }
        }
    }
}
