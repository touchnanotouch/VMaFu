// parallel/mpi/containers/detail/_MatrixMPI.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace containers {
                // Constructors

                template <typename T>
                MatrixMPI<T>::MatrixMPI() : _comm(communication::world()) {}

                template <typename T>
                MatrixMPI<T>::MatrixMPI(
                    const communication::Communicator& comm
                ) : _comm(comm) {}

                template <typename T>
                MatrixMPI<T>::MatrixMPI(
                    const vmafu::core::Matrix<T>& global_matrix,
                    distribution::MatrixDistributionInfo dist_info,
                    int root,
                    const communication::Communicator& comm
                ) : _dist_info(dist_info), _comm(comm) {
                    if (_comm.rank() == root) {
                        if (
                            global_matrix.rows() != _dist_info.global_rows ||
                            global_matrix.cols() != _dist_info.global_cols
                        ) {
                            throw std::invalid_argument(
                                "MatrixMPI::MatrixMPI(): Global matrix dimensions do not match distribution"
                            );
                        }
                    }

                    _local_matrix = vmafu::core::Matrix<T>(
                        _dist_info.local_rows, _dist_info.local_cols
                    );

                    distribution::scatter(
                        global_matrix, _dist_info, _local_matrix,
                        root, _comm
                    );
                }

                // Getters

                template <typename T>
                const vmafu::core::Matrix<T>& MatrixMPI<T>::local_matrix() const noexcept {
                    return _local_matrix;
                }

                template <typename T>
                const distribution::MatrixDistributionInfo& MatrixMPI<T>::distribution_info() const noexcept {
                    return _dist_info;
                }

                template <typename T>
                const communication::Communicator& MatrixMPI<T>::communicator() const noexcept {
                    return _comm;
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

                // Setters

                template <typename T>
                void MatrixMPI<T>::set_local_matrix(
                    const vmafu::core::Matrix<T>& matrix
                ) {
                    _local_matrix = matrix;
                }

                template <typename T>
                void MatrixMPI<T>::set_dist_info(
                    const distribution::MatrixDistributionInfo& dist_info
                ) {
                    _dist_info = dist_info;
                }

                template <typename T>
                void MatrixMPI<T>::set_comm(
                    const communication::Communicator& comm
                ) {
                    _comm = comm;
                }
            }
        }
    }
}
