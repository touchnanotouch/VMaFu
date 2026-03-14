// parallel/mpi/containers/detail/_VectorMPI.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace containers {
                // Constructors

                template <typename T>
                VectorMPI<T>::VectorMPI() : _comm(communication::world()) {}

                template <typename T>
                VectorMPI<T>::VectorMPI(
                    const communication::Communicator& comm
                ) : _comm(comm) {}

                template <typename T>
                VectorMPI<T>::VectorMPI(
                    const vmafu::core::Vector<T>& global_vector,
                    distribution::VectorDistributionInfo dist_info,
                    int root,
                    const communication::Communicator& comm
                ) : _dist_info(dist_info), _comm(comm) {
                    if (_comm.rank() == root) {
                        if (global_vector.size() != _dist_info.global_size) {
                            throw std::invalid_argument(
                                "VectorMPI::VectorMPI(): Global vector size does not match distribution"
                            );
                        }
                    }

                    _local_vector = vmafu::core::Vector<T>(
                        _dist_info.local_size
                    );

                    distribution::scatter(
                        global_vector, _dist_info, _local_vector,
                        root, _comm
                    );
                }

                // Getters

                template <typename T>
                const Vector<T>& VectorMPI<T>::local_vector() const noexcept {
                    return _local_vector;
                }

                template <typename T>
                const distribution::VectorDistributionInfo& VectorMPI<T>::distribution_info() const noexcept {
                    return _dist_info;
                }

                template <typename T>
                const communication::Communicator& VectorMPI<T>::communicator() const noexcept {
                    return _comm;
                }

                template <typename T>
                size_t VectorMPI<T>::local_size() const noexcept {
                    return _local_vector.size();
                }

                template <typename T>
                size_t VectorMPI<T>::global_size() const noexcept {
                    return _dist_info.global_size;
                }

                // Setters

                template <typename T>
                void VectorMPI<T>::set_local_vector(const Vector<T>& vector) {
                    _local_vector = vector;
                }

                template <typename T>
                void VectorMPI<T>::set_dist_info(const distribution::VectorDistributionInfo& dist_info) {
                    _dist_info = dist_info;
                }

                template <typename T>
                void VectorMPI<T>::set_comm(const communication::Communicator& comm) {
                    _comm = comm;
                }
            }
        }
    }
}
