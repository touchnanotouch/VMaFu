// parallel/mpi/wrappers/core/detail/MatrixMPI.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace wrappers {
                // Setters

                template <typename T>
                void MatrixMPI<T>::set_local_matrix(const Matrix<T>& matrix) {
                    _local_matrix = matrix;
                }

                template <typename T>
                void MatrixMPI<T>::set_comm(const Communicator& comm) {
                    _comm = comm;
                }

                template <typename T>
                void MatrixMPI<T>::set_dist_info(const DistributionInfo& info) {
                    _dist_info = info;
                }

                // Constructors

                template <typename T>
                MatrixMPI<T>::MatrixMPI(

                ) : _comm(world()) {

                }

                template <typename T>
                MatrixMPI<T>::MatrixMPI(
                    const Communicator& comm
                ) : _comm(comm) {

                }

                template <typename T>
                MatrixMPI<T>::MatrixMPI(
                    const Matrix<T>& global_matrix,
                    DistributionInfo dist_info,
                    int root,
                    const Communicator& comm
                ) : _dist_info(dist_info), _comm(comm) {
                    if (_comm.rank() == root) {
                        if (
                            global_matrix.rows() != _dist_info.global_rows || \
                            global_matrix.cols() != _dist_info.global_cols
                        ) {
                            throw std::invalid_argument("Global matrix dimensions do not match distribution");
                        }
                    }

                    _local_matrix = Matrix<T>(_dist_info.local_rows, _dist_info.local_cols);

                    scatter(
                        global_matrix,
                        _dist_info,
                        _local_matrix, 
                        root,
                        _comm
                    );
                }

                // Getters

                template <typename T>
                const Matrix<T>& MatrixMPI<T>::local_matrix() const noexcept {
                    return _local_matrix;
                }

                template <typename T>
                const Communicator& MatrixMPI<T>::communicator() const noexcept {
                    return _comm;
                }

                template <typename T>
                const DistributionInfo& MatrixMPI<T>::dist_info() const noexcept {
                    return _dist_info;
                }

                template <typename T>
                size_t MatrixMPI<T>::local_rows() const noexcept {
                    return _local_matrix.rows();
                }

                template <typename T>
                size_t MatrixMPI<T>::local_cols() const noexcept {
                    return _local_matrix.cols();
                }

                template <typename T>
                size_t MatrixMPI<T>::global_rows() const noexcept {
                    return _dist_info.global_rows;
                }

                template <typename T>
                size_t MatrixMPI<T>::global_cols() const noexcept {
                    return _dist_info.global_cols;
                }

                // MPI methods

                template <typename T>
                MatrixMPI<T> MatrixMPI<T>::multiply_simple(
                    const MatrixMPI<T>& other,
                    int root
                ) const {
                    int rank = _comm.rank();
                    int size = _comm.size();

                    size_t N = _dist_info.global_rows;
                    size_t M = _dist_info.global_cols;
                    size_t P = other._dist_info.global_cols;

                    // 1. Определяем оптимальное распределение для результата

                    DistributionInfo dist_result = internal::data::distribution_info(
                        N, P,
                        DistributionType::BLOCK_ROWS,
                        _comm
                    );

                    // 2. Подготавливаем матрицу A для умножения

                    Matrix<T> local_A;
                    if (_dist_info.type == DistributionType::BLOCK_ROWS) {
                        local_A = _local_matrix;
                    } else {
                        DistributionInfo dist_A_rows = internal::data::distribution_info(
                            N, M,
                            DistributionType::BLOCK_ROWS, 
                            _comm
                        );

                        Matrix<T> global_A;
                        if (rank == root) {
                            global_A = Matrix<T>(N, M);
                        }

                        internal::gather(global_A, _dist_info, _local_matrix, root, _comm);

                        local_A = Matrix<T>(dist_A_rows.local_rows, M);

                        internal::scatter(global_A, dist_A_rows, local_A, root, _comm);
                    }

                    // 3. Получаем полную матрицу B на всех процессах

                    Matrix<T> full_B;

                    if (size == 1) {
                        full_B = other._local_matrix;
                    } else {
                        Matrix<T> global_B;
                        if (rank == root) {
                            global_B = Matrix<T>(M, P);
                        }

                        internal::gather(global_B, other._dist_info, other._local_matrix, root, _comm);

                        if (rank == root) {
                            full_B = global_B;
                        } else {
                            full_B = Matrix<T>(M, P);
                        }

                        _comm.broadcast(full_B.data(), static_cast<int>(M * P), root);
                    }

                    // 4. Локальное умножение (каждый процесс умножает свои строки A на полную B)

                    Matrix<T> local_C(local_A.rows(), P);

                    for (size_t i = 0; i < local_A.rows(); i++) {
                        for (size_t j = 0; j < P; j++) {
                            T sum = T(0);

                            for (size_t k = 0; k < local_A.cols(); k++) {
                                sum += local_A(i, k) * full_B(k, j);
                            }

                            local_C(i, j) = sum;
                        }
                    }

                    // 5. Создаём результат

                    MatrixMPI<T> result(_comm);

                    result.set_local_matrix(local_C);
                    result.set_dist_info(dist_result);

                    return result;
                }

                template <typename T>
                MatrixMPI<T> MatrixMPI<T>::multiply(
                    const MatrixMPI<T>& other,
                    MultiplicationMethod method,
                    int root
                ) const {
                    if (_dist_info.global_cols != other._dist_info.global_rows) {
                        throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
                    }

                    if (_comm.get() != other._comm.get()) {
                        throw std::invalid_argument("Matrices must use the same communicator");
                    }

                    switch (method) {
                        case MultiplicationMethod::SIMPLE:
                            return multiply_simple(other, root);
                        case MultiplicationMethod::CANNON:
                            // TODO: реализовать (затычка)
                            return other;
                        default:
                            throw std::invalid_argument("Unsupported multiplication method");
                    }
                }

                // Static methods
            }
        }
    }
}
