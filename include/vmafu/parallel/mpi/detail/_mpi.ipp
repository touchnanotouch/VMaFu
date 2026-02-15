// parallel/mpi/detail/_mpi.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            template <typename T>
            MatrixMPI<T> load_matrix(
                const std::string& filename,
                DistributionType dist_type,
                int root,
                const Communicator& comm
            ) {
                Matrix<T> global_matrix;

                if (comm.rank() == root) {
                    global_matrix = io::load_matrix<T>(filename);
                }

                size_t dims[2] = {0, 0};
                if (comm.rank() == root) {
                    dims[0] = global_matrix.rows();
                    dims[1] = global_matrix.cols();
                }

                comm.broadcast(dims, 2, root);

                auto dist_info = distribution_info(
                     dist_type, dims[0], dims[1], comm
                );

                // На ненулевых процессах global_matrix пустая, но это нормально

                return MatrixMPI<T>(global_matrix, dist_info, root, comm);
            }

            template <typename T>
            void save_matrix(
                const std::string& filename,
                const wrappers::MatrixMPI<T>& matrix,
                int root,
                const Communicator& comm 
            ) {
                Matrix<T> global_matrix;

                gather(
                    global_matrix, matrix.dist_info(), matrix.local_matrix(),
                    root, comm
                );

                if (comm.rank() == root) {
                    vmafu::io::save(filename, global_matrix);
                }
            }

            template <typename T>
            std::ostream& operator<<(
                std::ostream& os,
                const MatrixMPI<T>& matrix
            ) {
                const Communicator& comm = matrix.communicator();

                int rank = comm.rank();
                int size = comm.size();

                for (int i = 0; i < size; i++) {
                    if (rank == i) {
                        if (i > 0) {
                            os << "\n";
                        }

                        os << "Process " << rank << ":\n" << "( local ) ";

                        core::operator<<(os, matrix.local_matrix());

                        os << std::flush;
                    }

                    comm.barrier();
                }

                // TODO: убрать лишние пробелы для ( global ) Matrix

                Matrix<T> global_matrix;
                if (rank == 0) {
                    global_matrix = Matrix<T>(
                        matrix.global_rows(), matrix.global_cols()
                    );
                }

                internal::gather(
                    global_matrix, matrix.dist_info(), matrix.local_matrix(),
                    0, comm
                );

                if (rank == 0) {
                    os << "( global ) ";

                    core::operator<<(os, global_matrix);
                }

                return os;
            }
        }
    }
}
