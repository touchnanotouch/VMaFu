// parallel/mpi/config/_config.hpp


#pragma once


#include <mpi.h>
#include <stdexcept>
#include <string>


namespace vmafu {
    namespace parallel {
        namespace mpi {
            // Checks

            inline bool is_initialized();
            inline bool is_finalized();
            inline bool is_thread_main();

            // Getters

            inline std::string processor_name();
            inline void version(int& version, int& subversion);

            // Init methods

            inline void init();
            
            inline void init_thread(
                int required = MPI_THREAD_SINGLE, 
                int* provided = nullptr
            );

            inline void init(
                int& argc,
                char**& argv
            );

            inline void init_thread(
                int& argc,
                char**& argv, 
                int required = MPI_THREAD_SINGLE, 
                int* provided = nullptr
            );

            // Finalize method

            inline void finalize();
        }
    }
}


#include "detail/_config.ipp"
