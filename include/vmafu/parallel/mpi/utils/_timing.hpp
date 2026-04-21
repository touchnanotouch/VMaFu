// parallel/mpi/utils/_timing.hpp


#pragma once


#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>


namespace vmafu {
    namespace parallel {
        namespace mpi {
            namespace timing {
                // Timing struct

                struct TimingInfo {
                    double min_time;
                    double max_time;

                    double avg_time;
                };

                // Helper method

                TimingInfo compute_statistics(
                    const std::vector<double>& samples
                );

                // Timer class

                class Timer {
                    private:
                        double _start_time;
                        double _end_time;

                        bool _running;

                    public:
                        // Constructor

                        Timer();

                        // Getter

                        bool is_running() const;

                        // Start / Stop methods

                        void start();
                        void stop();

                        // Elapsed time method

                        double elapsed() const;
                        
                        // Reset method

                        void reset();
                        
                        // Static method

                        static double now();
                };

                // SampleTimer class

                class SampleTimer {
                    private:
                        std::string _name;

                        Timer _timer;

                        std::vector<double> _samples;

                    public:
                        // Constructor

                        SampleTimer(const std::string& name = "Untitled");

                        // Getters

                        const std::string& name() const;

                        bool is_running() const;

                        TimingInfo info() const;

                        // Start / Stop methods

                        void start();
                        void stop();

                        // Record method

                        void record();

                        // Reset method

                        void reset();
                };

                // SyncTimer class

                class SyncTimer {
                    private:
                        std::string _name;

                        double _local_time;

                        double _min_time;
                        double _max_time;
                        double _avg_time;

                    public:
                        // Constructor

                        SyncTimer(const std::string& name = "Untitled");

                        // Getters

                        const std::string& name() const;
                        
                        double min_time() const;
                        double max_time() const;
                        double avg_time() const;

                        // Start / Stop methods

                        void start();
                        void stop();

                        // Elapsed time method

                        double elapsed() const;
                };

                // ScopedTimer class

                class ScopedTimer {
                    private:
                        std::string _name;

                        Timer _timer;

                    public:
                        // Constructor / Destructor

                        ScopedTimer(const std::string& name);

                        ~ScopedTimer();
                };
            }
        }
    }
}


#include "detail/_timing.ipp"
