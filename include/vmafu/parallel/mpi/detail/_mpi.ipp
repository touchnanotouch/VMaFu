// parallel/mpi/detail/_mpi.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            template <typename T>
            containers::VectorMPI<T> load_vector(
                const std::string& filename,
                distribution::VectorDistributionType dist_type,
                int root,
                const communication::Communicator& comm
            ) {
                vmafu::core::Vector<T> global_vector;

                if (comm.rank() == root) {
                    global_vector = vmafu::io::load_vector<T>(filename);
                }

                size_t size = 0;
                if (comm.rank() == root) {
                    size = global_vector.size();
                }

                comm.broadcast(&size, 1, root);

                auto dist_info = distribution::vector_distribution_info(
                    dist_type, size, comm
                );

                return containers::VectorMPI<T>(
                    global_vector, dist_info, root, comm
                );
            }

            template <typename T>
            containers::MatrixMPI<T> load_matrix(
                const std::string& filename,
                distribution::MatrixDistributionType dist_type,
                int root,
                const communication::Communicator& comm
            ) {
                vmafu::core::Matrix<T> global_matrix;

                if (comm.rank() == root) {
                    global_matrix = io::load_matrix<T>(filename);
                }

                size_t dims[2] = {0, 0};
                if (comm.rank() == root) {
                    dims[0] = global_matrix.rows();
                    dims[1] = global_matrix.cols();
                }

                comm.broadcast(dims, 2, root);

                auto dist_info = distribution::matrix_distribution_info(
                    dist_type, dims[0], dims[1], comm
                );

                return containers::MatrixMPI<T>(
                    global_matrix, dist_info, root, comm
                );
            }

            template <typename T>
            void save_vector(
                const std::string& filename,
                const containers::VectorMPI<T>& vector,
                int root,
                const communication::Communicator& comm
            ) {
                vmafu::core::Vector<T> global_vector;

                distribution::gather(
                    global_vector, vector.distribution_info(), vector.local_vector(),
                    root, comm
                );

                if (comm.rank() == root) {
                    vmafu::io::save_vector(filename, global_vector);
                }
            }

            template <typename T>
            void save_matrix(
                const std::string& filename,
                const containers::MatrixMPI<T>& matrix,
                int root,
                const communication::Communicator& comm 
            ) {
                vmafu::core::Matrix<T> global_matrix;

                distribution::gather(
                    global_matrix, matrix.distribution_info(), matrix.local_matrix(),
                    root, comm
                );

                if (comm.rank() == root) {
                    vmafu::io::save_matrix(filename, global_matrix);
                }
            }

            template <typename T>
            std::ostream& operator<<(
                std::ostream& os,
                const containers::VectorMPI<T>& vector
            ) {
                const communication::Communicator& comm = vector.communicator();

                int comm_rank = comm.rank();
                int comm_size = comm.size();

                for (int i = 0; i < comm_size; i++) {
                    if (comm_rank == i) {
                        os << "Process " << comm_rank << ":";
                        os << "\n( local ) ";

                        vmafu::core::operator<<(os, vector.local_vector());

                        os << "\n";
                    }

                    comm.barrier();
                }

                vmafu::core::Vector<T> global_vector;
                if (comm_rank == 0) {
                    global_vector = vmafu::core::Vector<T>(vector.global_size());
                }

                distribution::gather(
                    global_vector, vector.distribution_info(), vector.local_vector(),
                    0, comm
                );

                if (comm_rank == 0) {
                    os << "\n( global ) ";
                    vmafu::core::operator<<(os, global_vector);
                }

                return os;
            }

            template <typename T>
            std::ostream& operator<<(
                std::ostream& os,
                const containers::MatrixMPI<T>& matrix
            ) {
                const communication::Communicator& comm = matrix.communicator();

                int comm_rank = comm.rank();
                int comm_size = comm.size();

                for (int i = 0; i < comm_size; i++) {
                    if (comm_rank == i) {
                        os << "Process " << comm_rank << ":";
                        os << "\n( local ) ";

                        vmafu::core::operator<<(os, matrix.local_matrix());

                        os << "\n";
                    }

                    comm.barrier();
                }

                vmafu::core::Matrix<T> global_matrix;
                if (comm_rank == 0) {
                    global_matrix = vmafu::core::Matrix<T>(
                        matrix.global_rows(), matrix.global_cols()
                    );
                }

                distribution::gather(
                    global_matrix, matrix.distribution_info(), matrix.local_matrix(),
                    0, comm
                );

                if (comm_rank == 0) {
                    os << "\n( global ) ";
                    vmafu::core::operator<<(os, global_matrix);
                }

                return os;
            }
        }
    }
}
