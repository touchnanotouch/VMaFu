// parallel/mpi/communication/_communication.hpp


#pragma once


#include <mpi.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace communication {
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

                template <typename T>
                inline T reduce(T value, MPI_Op op, int root = 0);

                template <typename T>
                inline T allreduce(T value, MPI_Op op);

                template <typename T>
                inline void allgather(const T* sendbuf, T* recvbuf, int count);

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

                        // Constructors / Destructor

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

                        // Copy / Assignment

                        Communicator(const Communicator&);
                        Communicator& operator=(const Communicator&);

                        // Move operators

                        Communicator(Communicator&& other) noexcept;
                        Communicator& operator=(Communicator&& other) noexcept;

                        // P-to-P MPI methods

                        template <typename T>
                        void send(
                            const T* data,
                            int count,
                            int dest,
                            int tag = 0
                        ) const;

                        template <typename T>
                        void recv(
                            T* data,
                            int count,
                            int source = MPI_ANY_SOURCE,
                            int tag = MPI_ANY_TAG
                        ) const;

                        template <typename T>
                        void send(T value, int dest, int tag = 0) const;

                        template <typename T>
                        T recv(
                            int source = MPI_ANY_SOURCE,
                            int tag = MPI_ANY_TAG
                        ) const;

                        // Collective MPI methods

                        void barrier() const;

                        template <typename T>
                        void broadcast(T* data, int count, int root = 0) const;

                        template <typename T>
                        T broadcast_single(T value, int root = 0) const;

                        template <typename T>
                        T reduce(T value, MPI_Op op, int root = 0) const;

                        template <typename T>
                        void reduce(
                            const T* sendbuf,
                            T* recvbuf,
                            int count,
                            MPI_Op op,
                            int root = 0
                        ) const;

                        template <typename T>
                        T allreduce(T value, MPI_Op op) const;

                        template <typename T>
                        void allreduce(
                            const T* sendbuf,
                            T* recvbuf,
                            int count,
                            MPI_Op op
                        ) const;

                        template <typename T>
                        void allgather(
                            const T* sendbuf,
                            T* recvbuf,
                            int count
                        ) const;

                        template <typename T>
                        void allgatherv(
                            const T* sendbuf,
                            int sendcount,
                            T* recvbuf,
                            const int* recvcounts,
                            const int* displs
                        ) const;

                        // Distribution methods

                        template <typename T>
                        void scatter(
                            const T* sendbuf,
                            T* recvbuf,
                            int count,
                            int root = 0
                        ) const;

                        template <typename T>
                        void scatterv(
                            const T* sendbuf,
                            const int* sendcounts,
                            const int* displs,
                            T* recvbuf,
                            int recvcount,
                            int root = 0
                        ) const;

                        template <typename T>
                        void gather(
                            const T* sendbuf,
                            T* recvbuf,
                            int count,
                            int root = 0
                        ) const;

                        template <typename T>
                        void gatherv(
                            const T* sendbuf,
                            int sendcount,
                            T* recvbuf,
                            const int* recvcounts,
                            const int* displs,
                            int root = 0
                        ) const;

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
}


#include "detail/_communication.ipp"
