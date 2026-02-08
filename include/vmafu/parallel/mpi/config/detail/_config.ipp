// parallel/mpi/config/detail/_config.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            // Checks

            bool is_initialized()  {
                int flag;
                MPI_Initialized(&flag);

                return flag != 0;
            }

            bool is_finalized()  {
                int flag;
                MPI_Finalized(&flag);

                return flag != 0;
            }

            bool is_thread_main() {
                int flag;
                MPI_Is_thread_main(&flag);

                return flag != 0;
            }

            // Getters

            std::string processor_name() {
                char name[MPI_MAX_PROCESSOR_NAME];

                int length;
                MPI_Get_processor_name(name, &length);

                return std::string(name, length);
            }

            void version(int& version, int& subversion)  {
                MPI_Get_version(&version, &subversion);
            }

            // Init methods

            void init() {
                char** dummy_argv = nullptr;
                int dummy_argc = 0;

                MPI_Init(&dummy_argc, &dummy_argv);
            }

            void init_thread(
                int required,
                int* provided
            ) {
                char** dummy_argv = nullptr;
                int dummy_argc = 0;

                MPI_Init_thread(&dummy_argc, &dummy_argv, required, provided);
            }

            void init(int &argc, char **&argv) {
                MPI_Init(&argc, &argv);
            }

            void init_thread(
                int& argc,
                char**& argv, 
                int required, 
                int* provided
            ) {
                int local_provided;
                MPI_Init_thread(&argc, &argv, required, &local_provided);
                
                if (provided) {
                    *provided = local_provided;
                }
                
                if (local_provided < required) {
                    MPI_Finalize();

                    throw std::runtime_error(
                        "init_thread():Insufficient MPI thread support"
                    );
                }
            }

            // Finalize method

            void finalize() {
                MPI_Finalize();
            }
        }
    }
}
