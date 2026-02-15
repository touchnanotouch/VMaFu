// parallel/mpi/communication/detail/_communicator.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            // Check

            bool is_master() {
                return world().is_master();
            }

            // Getters

            Communicator& world() {
                static Communicator world_comm(MPI_COMM_WORLD, false);
                return world_comm;
            }

            int rank() {
                return world().rank();
            }

            int size() {
                return world().size();
            }

            // Collective MPI methods

            void barrier() {
                world().barrier();
            }

            template <typename T>
            void broadcast(T* data, int count, int root) {
                world().broadcast(data, count, root);
            }

            // Communicator class

            // Helper method

            void Communicator::update_info() {
                if (_comm != MPI_COMM_NULL) {
                    MPI_Comm_rank(_comm, &_rank);
                    MPI_Comm_size(_comm, &_size);
                } else {
                    _rank = MPI_PROC_NULL;
                    _size = 0;
                }
            }

            // Checks

            bool Communicator::is_valid() const {
                return _comm != MPI_COMM_NULL;
            }

            bool Communicator::is_master() const {
                return _rank == 0;
            }

            // Constructors/Destructor

            Communicator::Communicator(

            ) : _comm(MPI_COMM_WORLD), _owned(false) {
                update_info();
            }

            Communicator::Communicator(
                MPI_Comm comm,
                bool take_ownership
            ) : _comm(comm), _owned(take_ownership) {
                update_info();
            }

            Communicator::~Communicator() {
                if (
                    _owned && \
                    _comm != MPI_COMM_NULL && _comm != MPI_COMM_WORLD
                ) {
                    MPI_Comm_free(&_comm);
                }
            }

            // Getters

            int Communicator::rank() const noexcept {
                return _rank;
            }

            int Communicator::size() const noexcept {
                return _size;
            }

            MPI_Comm Communicator::get() const noexcept {
                return _comm;
            }

            // Copy/Assignment

            Communicator::Communicator(
                const Communicator& other
            ) : _comm(other._comm),  _owned(false),
                _rank(other._rank), _size(other._size) {

            }

            Communicator& Communicator::operator=(const Communicator& other) {
                if (this != &other) {
                    if (
                        _owned && \
                        _comm != MPI_COMM_NULL && _comm != MPI_COMM_WORLD
                    ) {
                        MPI_Comm_free(&_comm);
                    }

                    _comm = other._comm;
                    _owned = false;
                    _rank = other._rank;
                    _size = other._size;
                }

                return *this;
            }

            // Move operators

            Communicator::Communicator(
                Communicator&& other
            ) noexcept
              : _comm(other._comm), _owned(other._owned),
                _rank(other._rank), _size(other._size) 
            {
                other._comm = MPI_COMM_NULL;
                other._owned = false;
            }

            Communicator& Communicator::operator=(
                Communicator&& other
            ) noexcept {
                if (this != &other) {
                    if (
                        _owned && \
                        _comm != MPI_COMM_NULL && _comm != MPI_COMM_WORLD
                    ) {
                        MPI_Comm_free(&_comm);
                    }

                    _comm = other._comm;
                    _owned = other._owned;
                    _rank = other._rank;
                    _size = other._size;

                    other._comm = MPI_COMM_NULL;
                    other._owned = false;
                    other._rank = MPI_PROC_NULL;
                    other._size = 0;
                }

                return *this;
            }

            // Collective MPI methods

            void Communicator::barrier() const {
                if (is_valid()) {
                    MPI_Barrier(_comm);
                }
            }

            template <typename T>
            void Communicator::broadcast(T* data, int count, int root) const {
                if (is_valid()) {
                    MPI_Bcast(data, count, mpi_type<T>(), root, _comm);
                }
            }

            template <typename T>
            T Communicator::broadcast_single(T value, int root) const {
                if (is_valid()) {
                    MPI_Bcast(&value, 1, mpi_type<T>(), root, _comm);
                }

                return value;
            }

            // Comparison operators

            bool Communicator::operator==(const Communicator& other) const {
                if (_comm == MPI_COMM_NULL || other._comm == MPI_COMM_NULL) {
                    return _comm == other._comm;
                }

                int result;
                MPI_Comm_compare(_comm, other._comm, &result);

                return result == MPI_IDENT || result == MPI_CONGRUENT;
            }

            bool Communicator::operator!=(const Communicator& other) const {
                return !(*this == other);
            }

            // Static methods

            template <typename T>
            MPI_Datatype Communicator::mpi_type() {
                static_assert(
                    VMAFU_IS_SAME_V(T, char) ||
                    VMAFU_IS_SAME_V(T, signed char) ||
                    VMAFU_IS_SAME_V(T, unsigned char) ||
                    VMAFU_IS_SAME_V(T, short) ||
                    VMAFU_IS_SAME_V(T, unsigned short) ||
                    VMAFU_IS_SAME_V(T, int) ||
                    VMAFU_IS_SAME_V(T, unsigned int) ||
                    VMAFU_IS_SAME_V(T, long) ||
                    VMAFU_IS_SAME_V(T, unsigned long) ||
                    VMAFU_IS_SAME_V(T, long long) ||
                    VMAFU_IS_SAME_V(T, unsigned long long) ||
                    VMAFU_IS_SAME_V(T, float) ||
                    VMAFU_IS_SAME_V(T, double) ||
                    VMAFU_IS_SAME_V(T, long double),
                    "Communicator::mpi_type(): Unsupported type for MPI operations"
                );

                VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, char)) {
                    return MPI_CHAR;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, signed char)) {
                    return MPI_SIGNED_CHAR;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, unsigned char)) {
                    return MPI_UNSIGNED_CHAR;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, short)) {
                    return MPI_SHORT;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, unsigned short)) {
                    return MPI_UNSIGNED_SHORT;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, int)) {
                    return MPI_INT;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, unsigned int)) {
                    return MPI_UNSIGNED;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, long)) {
                    return MPI_LONG;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, unsigned long)) {
                    return MPI_UNSIGNED_LONG;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, long long)) {
                    return MPI_LONG_LONG_INT;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, unsigned long long)) {
                    return MPI_UNSIGNED_LONG_LONG;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, float)) {
                    return MPI_FLOAT;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, double)) {
                    return MPI_DOUBLE;
                } VMAFU_IF_CONSTEXPR (VMAFU_IS_SAME_V(T, long double)) {
                    return MPI_LONG_DOUBLE;
                }
            }

            Communicator Communicator::duplicate(
                const Communicator& other
            ) {
                MPI_Comm new_comm;
                MPI_Comm_dup(other._comm, &new_comm);

                return Communicator(new_comm, true);
            }

            Communicator Communicator::split(
                const Communicator& parent,
                int color,
                int key
            ) {
                MPI_Comm new_comm;
                MPI_Comm_split(parent._comm, color, key, &new_comm);

                return Communicator(new_comm, true);
            }

            Communicator Communicator::create_group(
                const Communicator& parent,
                const std::vector<int>& ranks
            ) {
                MPI_Group world_group, new_group;
                MPI_Comm new_comm;

                MPI_Comm_group(parent._comm, &world_group);

                MPI_Group_incl(
                    world_group, ranks.size(), ranks.data(), &new_group
                );

                MPI_Comm_create(parent._comm, new_group, &new_comm);

                MPI_Group_free(&world_group);
                MPI_Group_free(&new_group);

                return Communicator(new_comm, true);
            }
        }
    }
}
