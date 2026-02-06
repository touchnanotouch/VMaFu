// parallel/mpi/wrappers/core/detail/MatrixMPI.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace wrappers {
                // Helper methods

                template<typename T>
                void MatrixMPI<T>::_scatter_matrix(
                    const vmafu::Matrix<T>& global,
                    const Communicator& comm,
                    int root
                ) {
                    internal::scatter(
                        global, _local_data, _dist_info, comm, root
                    );
                }

                template<typename T>
                void MatrixMPI<T>::_gather_matrix(
                    vmafu::Matrix<T>& global,
                    const Communicator& comm,
                    int root
                ) const {
                    internal::gather(
                        _local_data, global, _dist_info, comm, root
                    );
                }

                // Constructors

                template<typename T>
                MatrixMPI<T>::MatrixMPI() : _comm(Communicator::world()) {
                    // Пустая матрица
                }

                template<typename T>
                MatrixMPI<T>::MatrixMPI(const Communicator& comm) : _comm(comm) {
                    // Пустая матрица с пользовательским коммуникатором
                }

                template<typename T>
                MatrixMPI<T>::MatrixMPI(
                    const vmafu::Matrix<T>& global_matrix,
                    const Communicator& comm,
                    internal::DistributionType dist_type,
                    int root
                ) : _comm(comm) {
                    // Создаем распределение
                    _dist_info = internal::distribution_info(
                        global_matrix.rows(),
                        global_matrix.cols(),
                        dist_type,
                        comm
                    );

                    // Распределяем данные
                    if (comm.rank() == root) {
                        // Проверяем размеры на корневом процессе
                        if (global_matrix.rows() != _dist_info.global_rows ||
                            global_matrix.cols() != _dist_info.global_cols) {
                            throw std::invalid_argument("Global matrix dimensions do not match distribution");
                        }
                    }

                    _scatter_matrix(global_matrix, comm, root);
                }

                // Getters

                template<typename T>
                const vmafu::Matrix<T>& MatrixMPI<T>::local_data() const noexcept {
                    return _local_data;
                }

                template<typename T>
                size_t MatrixMPI<T>::local_rows() const noexcept {
                    return _local_data.rows();
                }

                template<typename T>
                size_t MatrixMPI<T>::local_cols() const noexcept {
                    return _local_data.cols();
                }

                template<typename T>
                size_t MatrixMPI<T>::global_rows() const noexcept {
                    return _dist_info.global_rows;
                }

                template<typename T>
                size_t MatrixMPI<T>::global_cols() const noexcept {
                    return _dist_info.global_cols;
                }

                // Gather method
                
                template<typename T>
                vmafu::Matrix<T> MatrixMPI<T>::gather(int root) const {
                    vmafu::Matrix<T> global;
                    if (_comm.rank() == root) {
                        global = vmafu::Matrix<T>(_dist_info.global_rows, _dist_info.global_cols);
                    }

                    _gather_matrix(global, _comm, root);

                    return global;
                }

                // MPI operators (скалярные операции - выполняются локально)

                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::operator*(T scalar) const {
                    MatrixMPI<T> result(_comm);

                    result._dist_info = _dist_info;
                    result._local_data = _local_data * scalar;

                    return result;
                }

                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::operator+(const MatrixMPI<T>& other) const {
                    // Проверяем совместимость распределений
                    if (_dist_info.global_rows != other._dist_info.global_rows ||
                        _dist_info.global_cols != other._dist_info.global_cols) {
                        throw std::invalid_argument("Matrix dimensions do not match");
                    }

                    // Проверяем, что распределения одинаковы
                    if (_dist_info.type != other._dist_info.type ||
                        _local_data.rows() != other._local_data.rows() ||
                        _local_data.cols() != other._local_data.cols()) {
                        throw std::invalid_argument("Matrix distributions are not compatible");
                    }

                    MatrixMPI<T> result(_comm);

                    result._dist_info = _dist_info;
                    result._local_data = _local_data + other._local_data;

                    return result;
                }

                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::operator-(const MatrixMPI<T>& other) const {
                    // Проверяем совместимость распределений
                    if (_dist_info.global_rows != other._dist_info.global_rows ||
                        _dist_info.global_cols != other._dist_info.global_cols) {
                        throw std::invalid_argument("Matrix dimensions do not match");
                    }

                    // Проверяем, что распределения одинаковы
                    if (_dist_info.type != other._dist_info.type ||
                        _local_data.rows() != other._local_data.rows() ||
                        _local_data.cols() != other._local_data.cols()) {
                        throw std::invalid_argument("Matrix distributions are not compatible");
                    }

                    MatrixMPI<T> result(_comm);

                    result._dist_info = _dist_info;
                    result._local_data = _local_data - other._local_data;

                    return result;
                }

                // MPI methods (умножение матриц)

                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::multiply(
                    const MatrixMPI<T>& other,
                    MultiplicationMethod method
                ) const {
                    // Проверяем совместимость размеров
                    if (_dist_info.global_cols != other._dist_info.global_rows) {
                        throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
                    }
                    
                    MatrixMPI<T> result(_comm);
                    
                    if (method == MultiplicationMethod::SIMPLE) {
                        // Простое scatter-gather умножение (неэффективное)
                        // TODO: Реализовать более эффективный алгоритм
                        
                        // Собираем обе матрицы на всех процессах (неэффективно!)
                        auto global_A = gather(0);
                        auto global_B = other.gather(0);
                        
                        // Рассылаем результат умножения
                        vmafu::Matrix<T> global_C;
                        if (_comm.rank() == 0) {
                            global_C = global_A * global_B;
                        }
                        
                        // Создаем распределение для результата
                        result._dist_info = internal::distribution_info(
                            _dist_info.global_rows,
                            other._dist_info.global_cols,
                            _dist_info.type,
                            _comm
                        );
                        
                        // Распределяем результат
                        result._scatter_matrix(global_C, _comm, 0);
                        
                    } else if (method == MultiplicationMethod::CANNON) {
                        // Алгоритм Кэннона (для квадратных матриц и 2D распределения)
                        // TODO: Реализовать алгоритм Кэннона
                        throw std::runtime_error("Cannon algorithm not yet implemented");
                    }
                    
                    return result;
                }
                
                // Static methods
                
                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::zeros(
                    size_t rows,
                    size_t cols,
                    const Communicator& comm,
                    internal::DistributionType dist_type
                ) {
                    MatrixMPI<T> mat(comm);
                    mat._dist_info = Distributor::create_distribution(rows, cols, dist_type, comm);
                    mat._local_data = vmafu::Matrix<T>::zeros(
                        mat._dist_info.local_rows,
                        mat._dist_info.local_cols
                    );
                    return mat;
                }
                
                template<typename T>
                MatrixMPI<T> MatrixMPI<T>::identity(
                    size_t size,
                    const Communicator& comm,
                    internal::DistributionType dist_type
                ) {
                    MatrixMPI<T> mat(comm);
                    mat._dist_info = Distributor::create_distribution(size, size, dist_type, comm);
                    
                    // Создаем локальную часть единичной матрицы
                    mat._local_data = vmafu::Matrix<T>::zeros(
                        mat._dist_info.local_rows,
                        mat._dist_info.local_cols
                    );
                    
                    // Устанавливаем 1 на диагонали в локальных блоках
                    for (size_t i = 0; i < mat._local_data.rows(); i++) {
                        for (size_t j = 0; j < mat._local_data.cols(); j++) {
                            auto [global_i, global_j] = mat._dist_info.local_to_global(i, j);
                            if (global_i == global_j) {
                                mat._local_data(i, j) = static_cast<T>(1);
                            }
                        }
                    }
                    
                    return mat;
                }
                
            }
        }
    }
}
