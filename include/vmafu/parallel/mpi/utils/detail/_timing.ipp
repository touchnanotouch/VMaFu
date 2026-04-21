// parallel/mpi/utils/detail/_timing.ipp


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace timing {
                // Helper method

                inline TimingInfo compute_statistics(
                    const std::vector<double>& samples
                ) {
                    TimingInfo result;

                    if (samples.empty()) {
                        result.min_time = 0.0;
                        result.max_time = 0.0;
                        result.avg_time = 0.0;

                        return result;
                    }

                    result.min_time = *std::min_element(
                        samples.begin(), samples.end()
                    );
                    result.max_time = *std::max_element(
                        samples.begin(), samples.end()
                    );
                    result.avg_time = std::accumulate(
                        samples.begin(), samples.end(), 0.0
                    ) / samples.size();

                    return result;
                }

                // Timer class

                // Constructor

                inline Timer::Timer(

                ) : _start_time(0.0), _end_time(0.0), _running(false) {}

                // Getter

                inline bool Timer::is_running() const {
                    return _running;
                }

                // Start / Stop methods

                inline void Timer::start() {
                    _start_time = now();
                    _running = true;
                }

                inline void Timer::stop() {
                    if (_running) {
                        _end_time = now();
                        _running = false;
                    }
                }

                // Elapsed time method

                inline double Timer::elapsed() const {
                    if (_running) {
                        return now() - _start_time;
                    }

                    return _end_time - _start_time;
                }

                // Reset method

                inline void Timer::reset() {
                    _start_time = 0.0;
                    _end_time = 0.0;
                    _running = false;
                }

                // Static method

                inline double Timer::now() {
                    return MPI_Wtime();
                }

                // SamplerTimer class

                // Constructor

                inline SampleTimer::SampleTimer(
                    const std::string& name
                ) : _name(name) {}

                // Getters

                inline const std::string& SampleTimer::name() const {
                    return _name;
                }

                inline bool SampleTimer::is_running() const {
                    return _timer.is_running();
                }

                inline TimingInfo SampleTimer::info() const {
                    return compute_statistics(_samples);
                }

                // Start / Stop methods

                inline void SampleTimer::start() {
                    _timer.start();
                }

                inline void SampleTimer::stop() {
                    _timer.stop();
                }

                // Record method

                inline void SampleTimer::record() {
                    if (!_timer.is_running()) {
                        _samples.push_back(_timer.elapsed());
                        _timer.reset();
                    }
                }

                // Reset method

                inline void SampleTimer::reset() {
                    _samples.clear();
                    _timer.reset();
                }

                // SyncTimer class

                // Constructor

                inline SyncTimer::SyncTimer(
                    const std::string& name
                ) : _name(name), _local_time(0.0), _min_time(0.0), 
                    _max_time(0.0), _avg_time(0.0) {}

                // Getters

                inline const std::string& SyncTimer::name() const {
                    return _name;
                }

                inline double SyncTimer::min_time() const {
                    return _min_time;
                }

                inline double SyncTimer::max_time() const {
                    return _max_time;
                }

                inline double SyncTimer::avg_time() const {
                    return _avg_time;
                }

                // Start / Stop methods

                inline void SyncTimer::start() {
                    mpi::communication::barrier();

                    _local_time = Timer::now();
                }

                inline void SyncTimer::stop() {
                    mpi::communication::barrier();

                    double end_time = Timer::now();

                    _local_time = end_time - _local_time;

                    int rank = mpi::communication::rank();
                    int size = mpi::communication::size();

                    std::vector<double> all_times(size);

                    mpi::communication::allgather(
                        &_local_time, all_times.data(), 1
                    );

                    _min_time = *std::min_element(
                        all_times.begin(), all_times.end()
                    );
                    _max_time = *std::max_element(
                        all_times.begin(), all_times.end()
                    );
                    _avg_time = std::accumulate(
                        all_times.begin(), all_times.end(), 0.0
                    ) / size;
                }

                // Elapsed time method

                inline double SyncTimer::elapsed() const {
                    return _local_time;
                }

                // ScopedTimer class

                // Constructor / Destructor

                inline ScopedTimer::ScopedTimer(
                    const std::string& name
                ) : _name(name) {
                    _timer.start();
                }

                inline ScopedTimer::~ScopedTimer() {
                    _timer.stop();

                    double elapsed = _timer.elapsed();

                    std::cout << "( " << _name << " ) " << "ScopedTimer: { " << "elapsed: " << elapsed << " s }\n";
                }
            }
        }
    }
}
