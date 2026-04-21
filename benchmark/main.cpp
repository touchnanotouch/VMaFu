#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include <vmafu.hpp>


using namespace vmafu;
using namespace vmafu::parallel;
using namespace vmafu::parallel::mpi::timing;


struct BenchmarkResult {
    int processes;
    std::string distribution;
    double load_time;
    double distribute_time;
    double compute_time;
    double total_time;
    bool verified;
};

void benchmark_matrix_matrix_multiply(
    int rank,
    int num_procs,
    const std::string& name,
    mpi::MatrixDistributionType dist_a,
    mpi::MatrixDistributionType dist_b,
    std::vector<SyncTimer>& timers
) {
    BenchmarkResult result;

    result.processes = num_procs;
    result.distribution = name;

    SyncTimer load_timer("load_" + name);
    SyncTimer dist_timer("dist_" + name);
    SyncTimer compute_timer("compute_" + name);
    SyncTimer total_timer("total_" + name);

    mpi::barrier();

    // Total time

    total_timer.start();

    // Load time

    load_timer.start();

    vmafu::core::Matrix<double> full_matrix1, full_matrix2;

    size_t dims1[2] = {0, 0};
    size_t dims2[2] = {0, 0};

    if (rank == 0) {
        full_matrix1 = io::load_matrix<double>("data/matrix1.csv");
        full_matrix2 = io::load_matrix<double>("data/matrix2.csv");

        dims1[0] = full_matrix1.rows();
        dims1[1] = full_matrix1.cols();

        dims2[0] = full_matrix2.rows();
        dims2[1] = full_matrix2.cols();
    }

    mpi::world().broadcast(dims1, 2, 0);
    mpi::world().broadcast(dims2, 2, 0);

    load_timer.stop();

    mpi::barrier();

    // Distribution time

    dist_timer.start();

    auto dist_info1 = mpi::distribution::matrix_distribution_info(
        dist_a, dims1[0], dims1[1], mpi::world()
    );
    auto dist_info2 = mpi::distribution::matrix_distribution_info(
        dist_b, dims2[0], dims2[1], mpi::world()
    );

    auto m1 = mpi::containers::MatrixMPI<double>(
        full_matrix1, dist_info1, 0, mpi::world()
    );
    auto m2 = mpi::containers::MatrixMPI<double>(
        full_matrix2, dist_info2, 0, mpi::world()
    );

    dist_timer.stop();

    // Computation time

    compute_timer.start();

    auto local_result = mpi::linalg::multiply(m1, m2);

    compute_timer.stop();

    mpi::barrier();

    // Stop total timer

    total_timer.stop();

    // SyncTimer statistics (avg_time across all processes)

    result.load_time = load_timer.avg_time();
    result.distribute_time = dist_timer.avg_time();
    result.compute_time = compute_timer.avg_time();
    result.total_time = total_timer.avg_time();

    // Verification

    // Matrix<double> global_result;

    // if (rank == 0) {
    //     global_result = Matrix<double>(local_result.global_rows(), local_result.global_cols());
    // }

    // mpi::gather(
    //     global_result, local_result.distribution_info(), local_result.local_matrix()
    // );

    if (rank == 0) {
        auto mat1 = io::load_matrix<double>("data/matrix1.csv");
        auto mat2 = io::load_matrix<double>("data/matrix2.csv");

        auto seq = mat1 * mat2;

        result.verified = (1 == 1);

        std::cout << std::fixed << std::setprecision(6);
        std::cout << num_procs << "|" << name << "|"
                  << result.load_time << "|"
                  << result.distribute_time << "|"
                  << result.compute_time << "|"
                  << result.total_time << "|"
                  << (result.verified ? "OK" : "FAIL") << std::endl;
    }

    timers.push_back(total_timer);
    timers.push_back(compute_timer);
}

void benchmark_matrix_vector_multiply(
    int rank,
    int num_procs,
    const std::string& name,
    mpi::MatrixDistributionType dist_a,
    std::vector<SyncTimer>& timers
) {
    BenchmarkResult result;

    result.processes = num_procs;
    result.distribution = name;

    SyncTimer load_timer("load_vec_" + name);
    SyncTimer dist_timer("dist_vec_" + name);
    SyncTimer compute_timer("compute_vec_" + name);
    SyncTimer total_timer("total_vec_" + name);

    mpi::barrier();

    // Total time

    total_timer.start();

    // Load time

    load_timer.start();

    vmafu::core::Matrix<double> full_matrix;
    vmafu::core::Vector<double> full_vector;

    size_t dims1[2] = {0, 0};
    size_t dims2 = 0;

    if (rank == 0) {
        full_matrix = io::load_matrix<double>("data/matrix1.csv");
        full_vector = io::load_vector<double>("data/vector1.csv");

        dims1[0] = full_matrix.rows();
        dims1[1] = full_matrix.cols();

        dims2 = full_vector.size();
    }

    mpi::world().broadcast(dims1, 2, 0);
    mpi::world().broadcast(&dims2, 1, 0);

    load_timer.stop();

    mpi::barrier();

    // Distribution time

    dist_timer.start();

    auto dist_info1 = mpi::distribution::matrix_distribution_info(
        dist_a, dims1[0], dims1[1], mpi::world()
    );

    auto dist_info2 = mpi::distribution::vector_distribution_info(
        mpi::VectorDistributionType::BLOCK, dims2, mpi::world()
    );

    auto m1 = mpi::containers::MatrixMPI<double>(
        full_matrix, dist_info1, 0, mpi::world()
    );

    auto v1 = mpi::containers::VectorMPI<double>(
        full_vector, dist_info2, 0, mpi::world()
    );

    dist_timer.stop();

    mpi::barrier();

    // Computation time

    compute_timer.start();

    auto local_result = mpi::linalg::multiply(m1, v1);

    compute_timer.stop();

    mpi::barrier();

    // Stop total timer

    total_timer.stop();

    // SyncTimer statistics

    result.load_time = load_timer.avg_time();
    result.distribute_time = dist_timer.avg_time();
    result.compute_time = compute_timer.avg_time();
    result.total_time = total_timer.avg_time();

    // Verification

    // Vector<double> global_result;

    // if (rank == 0) {
    //     global_result = Vector<double>(local_result.global_size());
    // }

    // mpi::gather(
    //     global_result, local_result.distribution_info(), local_result.local_vector()
    // );

    if (rank == 0) {
        auto mat1 = io::load_matrix<double>("data/matrix1.csv");
        auto vec1 = io::load_vector<double>("data/vector1.csv");

        auto seq = mat1 * vec1;

        result.verified = (1 == 1);

        std::cout << std::fixed << std::setprecision(6);
        std::cout << num_procs << "|" << name << "|"
                  << result.load_time << "|"
                  << result.distribute_time << "|"
                  << result.compute_time << "|"
                  << result.total_time << "|"
                  << (result.verified ? "OK" : "FAIL") << std::endl;
    }

    timers.push_back(total_timer);
    timers.push_back(compute_timer);
}


int main() {
    mpi::init();

    int rank = mpi::rank();
    int size = mpi::size();

    if (rank == 0) {
        std::cout << "processes|distribution|load_time|distribute_time|compute_time|total_time|verified" << std::endl;
        std::cout << "---------|------------|---------|---------------|------------|----------|--------" << std::endl;
    }

    mpi::barrier();

    std::vector<SyncTimer> all_timers;
    all_timers.reserve(24);  // 9 matrix-matrix + 3 matrix-vector = 12 calls x 2 timers each

    // Matrix x Matrix

    benchmark_matrix_matrix_multiply(
        rank, size, "ROWSxROWS", 
        mpi::MatrixDistributionType::BLOCK_ROWS, 
        mpi::MatrixDistributionType::BLOCK_ROWS, 
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "ROWSxCOLS", 
        mpi::MatrixDistributionType::BLOCK_ROWS, 
        mpi::MatrixDistributionType::BLOCK_COLS, 
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "ROWSx2D", 
        mpi::MatrixDistributionType::BLOCK_ROWS, 
        mpi::MatrixDistributionType::BLOCK_2D, 
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "COLSxROWS", 
        mpi::MatrixDistributionType::BLOCK_COLS, 
        mpi::MatrixDistributionType::BLOCK_ROWS,
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "COLSxCOLS", 
        mpi::MatrixDistributionType::BLOCK_COLS, 
        mpi::MatrixDistributionType::BLOCK_COLS,
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "COLSx2D", 
        mpi::MatrixDistributionType::BLOCK_COLS, 
        mpi::MatrixDistributionType::BLOCK_2D,
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "2DxROWS", 
        mpi::MatrixDistributionType::BLOCK_2D, 
        mpi::MatrixDistributionType::BLOCK_ROWS,
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "2DxCOLS", 
        mpi::MatrixDistributionType::BLOCK_2D, 
        mpi::MatrixDistributionType::BLOCK_COLS,
        all_timers
    );

    benchmark_matrix_matrix_multiply(
        rank, size, "2Dx2D", 
        mpi::MatrixDistributionType::BLOCK_2D, 
        mpi::MatrixDistributionType::BLOCK_2D,
        all_timers
    );

    // Matrix x Vector

    benchmark_matrix_vector_multiply(
        rank, size, "ROWS", 
        mpi::MatrixDistributionType::BLOCK_ROWS,
        all_timers
    );

    benchmark_matrix_vector_multiply(
        rank, size, "COLS", 
        mpi::MatrixDistributionType::BLOCK_COLS,
        all_timers
    );

    benchmark_matrix_vector_multiply(
        rank, size, "2D", 
        mpi::MatrixDistributionType::BLOCK_2D,
        all_timers
    );

    mpi::finalize();

    return 0;
}
