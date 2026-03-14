// parallel/mpi/linalg/detail/_operations.ipp


#pragma once


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace linalg {
                namespace internal {
                    template <typename T>
                    vmafu::core::Matrix<T> allgather_matrix(
                        const containers::MatrixMPI<T>& matrix,
                        const communication::Communicator& comm
                    ) {
                        int comm_size = comm.size();

                        const auto& info = matrix.distribution_info();

                        size_t rows = info.global_rows;
                        size_t cols = info.global_cols;

                        vmafu::core::Vector<size_t> all_local_rows(comm_size);
                        vmafu::core::Vector<size_t> all_local_cols(comm_size);
                        vmafu::core::Vector<size_t> all_row_offsets(comm_size);
                        vmafu::core::Vector<size_t> all_col_offsets(comm_size);

                        vmafu::core::Vector<int> counts(comm_size, 1);
                        vmafu::core::Vector<int> disps(comm_size);

                        for (int i = 0; i < comm_size; i++) {
                            disps[i] = i;
                        }

                        comm.allgatherv(
                            &info.local_rows, 1, all_local_rows.data(),
                            counts.data(), disps.data()
                        );
                        comm.allgatherv(
                            &info.local_cols, 1, all_local_cols.data(),
                            counts.data(), disps.data()
                        );
                        comm.allgatherv(
                            &info.row_offset, 1, all_row_offsets.data(),
                            counts.data(), disps.data()
                        );
                        comm.allgatherv(
                            &info.col_offset, 1, all_col_offsets.data(),
                            counts.data(), disps.data()
                        );

                        vmafu::core::Matrix<T> global(rows, cols);

                        if (
                            info.type == distribution::MatrixDistributionType::BLOCK_ROWS
                        ) {
                            vmafu::core::Vector<int> recvcounts(comm_size);
                            vmafu::core::Vector<int> recvdisps(comm_size);

                            for (int i = 0; i < comm_size; i++) {
                                recvcounts[i] = static_cast<int>(
                                    all_local_rows[i] * cols
                                );
                                recvdisps[i] = static_cast<int>(
                                    all_row_offsets[i] * cols
                                );
                            }

                            comm.allgatherv(
                                matrix.local_matrix().data(),
                                static_cast<int>(info.local_rows * cols),
                                global.data(),
                                recvcounts.data(),
                                recvdisps.data()
                            );
                        } else if (
                            info.type == distribution::MatrixDistributionType::BLOCK_COLS
                        ) {
                            vmafu::core::Vector<int> recvcounts(comm_size);
                            vmafu::core::Vector<int> recvdisps(comm_size);

                            int total_size = 0;

                            for (int i = 0; i < comm_size; i++) {
                                recvcounts[i] = static_cast<int>(
                                    rows * all_local_cols[i]
                                );
                                recvdisps[i] = total_size;

                                total_size += recvcounts[i];
                            }

                            vmafu::core::Vector<T> recv_buf(total_size);

                            comm.allgatherv(
                                matrix.local_matrix().data(),
                                static_cast<int>(rows * info.local_cols),
                                recv_buf.data(),
                                recvcounts.data(),
                                recvdisps.data()
                            );

                            for (int p = 0; p < comm_size; p++) {
                                size_t col_start = all_col_offsets[p];
                                size_t local_cols = all_local_cols[p];

                                for (size_t i = 0; i < rows; i++) {
                                    for (size_t j = 0; j < local_cols; j++) {
                                        global(i, col_start + j) = recv_buf[recvdisps[p] + i * local_cols + j];
                                    }
                                }
                            }
                        } else {
                            const auto& local_mat = matrix.local_matrix();

                            vmafu::core::Vector<T> send_buf(info.local_rows * info.local_cols);

                            for (size_t i = 0; i < info.local_rows; i++) {
                                for (size_t j = 0; j < info.local_cols; j++) {
                                    send_buf[i * info.local_cols + j] = local_mat(i, j);
                                }
                            }

                            vmafu::core::Vector<int> recvcounts_2d(comm_size);
                            vmafu::core::Vector<int> recvdisps_2d(comm_size);

                            int total_size = 0;

                            for (int i = 0; i < comm_size; i++) {
                                recvcounts_2d[i] = static_cast<int>(
                                    all_local_rows[i] * all_local_cols[i]
                                );
                                recvdisps_2d[i] = total_size;

                                total_size += recvcounts_2d[i];
                            }

                            vmafu::core::Vector<T> recv_buf(total_size);

                            comm.allgatherv(
                                send_buf.data(),
                                static_cast<int>(info.local_rows * info.local_cols),
                                recv_buf.data(),
                                recvcounts_2d.data(),
                                recvdisps_2d.data()
                            );

                            for (int p = 0; p < comm_size; p++) {
                                size_t row_off = all_row_offsets[p];
                                size_t col_off = all_col_offsets[p];
                                size_t lrows = all_local_rows[p];
                                size_t lcols = all_local_cols[p];

                                for (size_t i = 0; i < lrows; i++) {
                                    for (size_t j = 0; j < lcols; j++) {
                                        global(row_off + i, col_off + j) = recv_buf[recvdisps_2d[p] + i * lcols + j];
                                    }
                                }
                            }
                        }

                        return global;
                    }
                    
                }
 
                template <typename T>
                containers::MatrixMPI<T> multiply(
                    const containers::MatrixMPI<T>& A,
                    const containers::MatrixMPI<T>& B,
                    int root,
                    const communication::Communicator& comm
                ) {
                    if (A.global_cols() != B.global_rows()) {
                        throw std::invalid_argument(
                            "operations::multiply: Matrix dimensions are not compatible"
                        );
                    }

                    size_t N = A.global_rows();
                    size_t M = A.global_cols();
                    size_t P = B.global_cols();

                    vmafu::core::Matrix<T> global_B = internal::allgather_matrix(
                        B, comm
                    );

                    vmafu::core::Matrix<T> local_A_rows;

                    if (A.distribution_info().type == distribution::MatrixDistributionType::BLOCK_ROWS) {
                        local_A_rows = A.local_matrix();
                    } else {
                        vmafu::core::Matrix<T> global_A = internal::allgather_matrix(
                            A, comm
                        );

                        auto dist_rows = distribution::matrix_distribution_info(
                            distribution::MatrixDistributionType::BLOCK_ROWS, N, M, comm
                        );

                        local_A_rows = vmafu::core::Matrix<T>(
                            dist_rows.local_rows, M
                        );

                        for (size_t i = 0; i < dist_rows.local_rows; i++) {
                            for (size_t j = 0; j < M; j++) {
                                local_A_rows(i, j) = global_A(dist_rows.row_offset + i, j);
                            }
                        }
                    }

                    vmafu::core::Matrix<T> local_C(local_A_rows.rows(), P);

                    for (size_t i = 0; i < local_A_rows.rows(); i++) {
                        for (size_t j = 0; j < P; j++) {
                            T sum = T(0);

                            for (size_t k = 0; k < M; k++) {
                                sum += local_A_rows(i, k) * global_B(k, j);
                            }

                            local_C(i, j) = sum;
                        }
                    }

                    auto dist_result = distribution::matrix_distribution_info(
                        distribution::MatrixDistributionType::BLOCK_ROWS, N, P, comm
                    );

                    containers::MatrixMPI<T> result(comm);

                    result.set_local_matrix(local_C);
                    result.set_dist_info(dist_result);

                    return result;
                }

                template <typename T>
                containers::VectorMPI<T> multiply(
                    const containers::MatrixMPI<T>& A,
                    const containers::VectorMPI<T>& x,
                    int root,
                    const communication::Communicator& comm
                ) {
                    if (A.global_cols() != x.global_size()) {
                        throw std::invalid_argument(
                            "operations::multiply: Matrix columns must equal vector size"
                        );
                    }

                    int comm_size = comm.size();

                    size_t N = A.global_rows();
                    size_t M = A.global_cols();

                    const auto& x_info = x.distribution_info();

                    vmafu::core::Vector<size_t> all_local_sizes(comm_size);
                    vmafu::core::Vector<size_t> all_offsets(comm_size);
                    vmafu::core::Vector<int> counts(comm_size, 1);
                    vmafu::core::Vector<int> disps(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        disps[i] = i;
                    }

                    comm.allgatherv(
                        &x_info.local_size, 1, all_local_sizes.data(),
                        counts.data(), disps.data()
                    );
                    comm.allgatherv(
                        &x_info.offset, 1, all_offsets.data(),
                        counts.data(), disps.data()
                    );

                    vmafu::core::Vector<int> gather_counts(comm_size);
                    vmafu::core::Vector<int> gather_disps(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        gather_counts[i] = static_cast<int>(all_local_sizes[i]);
                        gather_disps[i] = static_cast<int>(all_offsets[i]);
                    }

                    vmafu::core::Vector<T> global_x(M);

                    comm.allgatherv(
                        x.local_vector().data(),
                        static_cast<int>(x_info.local_size),
                        global_x.data(),
                        gather_counts.data(),
                        gather_disps.data()
                    );

                    const auto& A_info = A.distribution_info();
                    const auto& local_A = A.local_matrix();

                    vmafu::core::Vector<T> local_y;

                    if (
                        A_info.type == distribution::MatrixDistributionType::BLOCK_ROWS
                    ) {
                        local_y = vmafu::core::Vector<T>(A_info.local_rows);

                        for (size_t i = 0; i < A_info.local_rows; i++) {
                            T sum = T(0);

                            for (size_t j = 0; j < M; j++) {
                                sum += local_A(i, j) * global_x[j];
                            }

                            local_y[i] = sum;
                        }
                    } else if (
                        A_info.type == distribution::MatrixDistributionType::BLOCK_COLS
                    ) {
                        vmafu::core::Vector<T> partial_result(N, T(0));

                        for (size_t j = 0; j < A_info.local_cols; j++) {
                            T x_val = global_x[A_info.col_offset + j];

                            for (size_t i = 0; i < N; i++) {
                                partial_result[i] += local_A(i, j) * x_val;
                            }
                        }

                        vmafu::core::Vector<T> global_y(N);

                        comm.allreduce(
                            partial_result.data(), global_y.data(),
                            static_cast<int>(N), MPI_SUM
                        );

                        auto dist_result = distribution::vector_distribution_info(
                            distribution::VectorDistributionType::BLOCK, N, comm
                        );
                        
                        local_y = vmafu::core::Vector<T>(dist_result.local_size);

                        for (size_t i = 0; i < dist_result.local_size; i++) {
                            local_y[i] = global_y[dist_result.offset + i];
                        }

                        containers::VectorMPI<T> result(comm);

                        result.set_local_vector(local_y);
                        result.set_dist_info(dist_result);

                        return result;
                    } else {
                        vmafu::core::Matrix<T> global_A = internal::allgather_matrix(A, comm);
                        
                        auto dist_rows = distribution::matrix_distribution_info(
                            distribution::MatrixDistributionType::BLOCK_ROWS, N, M, comm
                        );
                        
                        local_y = vmafu::core::Vector<T>(dist_rows.local_rows);

                        for (size_t i = 0; i < dist_rows.local_rows; i++) {
                            T sum = T(0);

                            for (size_t j = 0; j < M; j++) {
                                sum += global_A(dist_rows.row_offset + i, j) * global_x[j];
                            }

                            local_y[i] = sum;
                        }
                    }

                    auto dist_result = distribution::vector_distribution_info(
                        distribution::VectorDistributionType::BLOCK, N, comm
                    );

                    containers::VectorMPI<T> result(comm);

                    result.set_local_vector(local_y);
                    result.set_dist_info(dist_result);

                    return result;
                }

                template <typename T>
                containers::VectorMPI<T> multiply(
                    const containers::VectorMPI<T>& x,
                    const containers::MatrixMPI<T>& A,
                    int root,
                    const communication::Communicator& comm
                ) {
                    if (x.global_size() != A.global_rows()) {
                        throw std::invalid_argument(
                            "operations::multiply: Vector size must equal matrix rows"
                        );
                    }

                    int comm_size = comm.size();

                    size_t M = A.global_rows();
                    size_t P = A.global_cols();

                    const auto& x_info = x.distribution_info();

                    vmafu::core::Vector<size_t> all_local_sizes(comm_size);
                    vmafu::core::Vector<size_t> all_offsets(comm_size);
                    vmafu::core::Vector<int> counts(comm_size, 1);
                    vmafu::core::Vector<int> disps(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        disps[i] = i;
                    }

                    comm.allgatherv(
                        &x_info.local_size, 1, all_local_sizes.data(),
                        counts.data(), disps.data()
                    );
                    comm.allgatherv(
                        &x_info.offset, 1, all_offsets.data(),
                        counts.data(), disps.data()
                    );

                    vmafu::core::Vector<int> gather_counts(comm_size);
                    vmafu::core::Vector<int> gather_disps(comm_size);

                    for (int i = 0; i < comm_size; i++) {
                        gather_counts[i] = static_cast<int>(all_local_sizes[i]);
                        gather_disps[i] = static_cast<int>(all_offsets[i]);
                    }

                    vmafu::core::Vector<T> global_x(M);

                    comm.allgatherv(
                        x.local_vector().data(),
                        static_cast<int>(x_info.local_size),
                        global_x.data(),
                        gather_counts.data(),
                        gather_disps.data()
                    );

                    const auto& A_info = A.distribution_info();
                    const auto& local_A = A.local_matrix();

                    vmafu::core::Vector<T> global_result(P);

                    if (
                        A_info.type == distribution::MatrixDistributionType::BLOCK_COLS
                    ) {
                        vmafu::core::Vector<T> local_result(A_info.local_cols);

                        for (size_t j = 0; j < A_info.local_cols; j++) {
                            T sum = T(0);

                            for (size_t i = 0; i < M; i++) {
                                sum += global_x[i] * local_A(i, j);
                            }

                            local_result[j] = sum;
                        }

                        comm.allgatherv(
                            &A_info.local_cols, 1, all_local_sizes.data(),
                            counts.data(), disps.data()
                        );
                        comm.allgatherv(
                            &A_info.col_offset, 1, all_offsets.data(),
                            counts.data(), disps.data()
                        );

                        vmafu::core::Vector<int> result_counts(comm_size);
                        vmafu::core::Vector<int> result_disps(comm_size);

                        for (int i = 0; i < comm_size; i++) {
                            result_counts[i] = static_cast<int>(all_local_sizes[i]);
                            result_disps[i] = static_cast<int>(all_offsets[i]);
                        }

                        comm.allgatherv(
                            local_result.data(),
                            static_cast<int>(A_info.local_cols),
                            global_result.data(),
                            result_counts.data(),
                            result_disps.data()
                        );
                    } else if (
                        A_info.type == distribution::MatrixDistributionType::BLOCK_ROWS
                    ) {
                        vmafu::core::Vector<T> partial_result(P, T(0));

                        for (size_t i = 0; i < A_info.local_rows; i++) {
                            T x_val = global_x[A_info.row_offset + i];

                            for (size_t j = 0; j < A_info.local_cols; j++) {
                                partial_result[j] += x_val * local_A(i, j);
                            }
                        }

                        comm.allreduce(
                            partial_result.data(), global_result.data(),
                            static_cast<int>(P), MPI_SUM
                        );
                    } else {
                        vmafu::core::Matrix<T> global_A = internal::allgather_matrix(
                            A, comm
                        );

                        for (size_t j = 0; j < P; j++) {
                            T sum = T(0);

                            for (size_t i = 0; i < M; i++) {
                                sum += global_x[i] * global_A(i, j);
                            }

                            global_result[j] = sum;
                        }
                    }

                    auto dist_result = distribution::vector_distribution_info(
                        distribution::VectorDistributionType::BLOCK, P, comm
                    );

                    containers::VectorMPI<T> result(comm);

                    vmafu::core::Vector<T> local_part(dist_result.local_size);
                    for (size_t i = 0; i < dist_result.local_size; i++) {
                        local_part[i] = global_result[dist_result.offset + i];
                    }

                    result.set_local_vector(local_part);
                    result.set_dist_info(dist_result);

                    return result;
                }
            }
        }
    }
}
