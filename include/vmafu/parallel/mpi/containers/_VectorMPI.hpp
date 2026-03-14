// parallel/mpi/containers/_VectorMPI.hpp


#pragma once


#include <string>

#include "../../../core/_Vector.hpp"

#include "../communication/communication.hpp"
#include "../distribution/distribution.hpp"


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace containers {
                template <typename T>
                class VectorMPI {
                    private:
                        vmafu::core::Vector<T> _local_vector;
                        distribution::VectorDistributionInfo _dist_info;
                        communication::Communicator _comm;

                        // Friend methods

                        template <typename U>
                        friend VectorMPI<U> load_vector(
                            const std::string& filename,
                            distribution::VectorDistributionType dist_type,
                            int root,
                            const communication::Communicator& comm
                        );

                        template <typename U>
                        friend void save_vector(
                            const std::string& filename,
                            const VectorMPI<U>& vector,
                            int root,
                            const communication::Communicator& comm
                        );

                    public:
                        // Constructors

                        VectorMPI();
                        VectorMPI(const communication::Communicator& comm);

                        VectorMPI(
                            const vmafu::core::Vector<T>& global_vector,
                            distribution::VectorDistributionInfo dist_info,
                            int root = 0,
                            const communication::Communicator& comm = communication::world()
                        );

                        // Getters

                        const vmafu::core::Vector<T>& local_vector() const noexcept;
                        const distribution::VectorDistributionInfo& distribution_info() const noexcept;
                        const communication::Communicator& communicator() const noexcept;

                        size_t local_size() const noexcept;
                        size_t global_size() const noexcept;

                        // Setters

                        void set_local_vector(
                            const vmafu::core::Vector<T>& vector
                        );
                        void set_dist_info(
                            const distribution::VectorDistributionInfo& dist_info
                        );
                        void set_comm(
                            const communication::Communicator& comm
                        );
                };
            }
        }
    }
}


#include "detail/_VectorMPI.ipp"
