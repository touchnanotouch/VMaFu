#include <iostream>

#include <vmafu.hpp>


using namespace vmafu;
using namespace vmafu::parallel;


int main() {
    mpi::init();

    int rank = mpi::rank();

    auto m1 = mpi::load_matrix<double>("data/matrix1.csv");
    auto m2 = mpi::load_matrix<double>("data/matrix2.csv");

    mpi::barrier();

    double start_parallel = MPI_Wtime();

    auto res_parallel = m1.multiply(m2);

    mpi::barrier();

    double end_parallel = MPI_Wtime();

    mpi::save_matrix("data/result1.csv", res_parallel);

    double start_seq = 0, end_seq = 0;

    if (rank == 0) {
        auto mat1 = io::load_matrix<double>("data/matrix1.csv");
        auto mat2 = io::load_matrix<double>("data/matrix2.csv");

        start_seq = MPI_Wtime();

        auto res_seq = mat1 * mat2;

        end_seq = MPI_Wtime();

        io::save_matrix("data/result2.csv", res_seq);
    }

    if (rank == 0) {
        double parallel_time = end_parallel - start_parallel;
        double seq_time = end_seq - start_seq;

        double speedup = seq_time / parallel_time;
        double percent = (speedup - 1.0) * 100.0;

        std::cout << "SEQ -> " << seq_time << " seconds" << std::endl;
        std::cout << "MPI -> " << parallel_time << " seconds" << std::endl;
        std::cout << "\nSpeedup: " << speedup << "x" << std::endl;
        std::cout << "Percent: " << percent << "%" << std::endl;
    }

    mpi::finalize();

    return 0;
}
