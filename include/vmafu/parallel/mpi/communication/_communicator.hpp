// parallel/mpi/communication/_communicator.hpp


#pragma once


#include <mpi.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>


namespace vmafu {
    namespace parallel {
        namespace mpi {
            // Forward declaration

            class Communicator;

            // Check

            inline bool is_master();

            // Getters

            inline Communicator& world();

            inline int rank();
            inline int size();

            // Collective MPI methods

            inline void barrier();

            template <typename T>
            inline void broadcast(T* data, int count, int root = 0);

            // Communicator class

            class Communicator {
                private:
                    MPI_Comm _comm;

                    bool _owned;

                    int _rank;
                    int _size;

                    // Helper method

                    void update_info();

                public:
                    // Checks

                    bool is_valid() const;
                    bool is_master() const;

                    // Constructors/Destructor

                    Communicator();

                    explicit Communicator(
                        MPI_Comm comm,
                        bool take_ownership = false
                    );

                    ~Communicator();

                    // Getters

                    int rank() const noexcept;
                    int size() const noexcept;

                    MPI_Comm get() const noexcept;

                    // Copy/Assignment

                    Communicator(const Communicator&);
                    Communicator& operator=(const Communicator&);

                    // Move operators

                    Communicator(Communicator&& other) noexcept;
                    Communicator& operator=(Communicator&& other) noexcept;

                    // Collective MPI methods

                    void barrier() const;

                    template <typename T>
                    void broadcast(T* data, int count, int root = 0) const;

                    template <typename T>
                    T broadcast_single(T value, int root = 0) const;

                    // Comparison operators

                    bool operator==(const Communicator& other) const;
                    bool operator!=(const Communicator& other) const;

                    // Static methods
                    
                    template <typename T>
                    static MPI_Datatype mpi_type();

                    static Communicator duplicate(
                        const Communicator& other
                    );

                    static Communicator split(
                        const Communicator& parent,
                        int color,
                        int key
                    );

                    static Communicator create_group(
                        const Communicator& parent, 
                        const std::vector<int>& ranks
                    );
            };
        }
    }
}


#include "detail/_communicator.ipp"
