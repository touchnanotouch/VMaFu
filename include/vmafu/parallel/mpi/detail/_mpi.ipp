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
        }
    }
}