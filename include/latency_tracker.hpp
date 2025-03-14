#ifndef LATENCY_TRACKER_HPP
#define LATENCY_TRACKER_HPP

#include <unordered_map>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>


class LatencyTracker {
    public:
    void start(const std::string& label);
    void stop(const std::string& label);
    void reset();
    void print_report() const;
    int get_entry_count() const;

private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> start_times;
    std::vector<long long> latencies;
    mutable std::mutex mtx;
};



extern LatencyTracker latency_tracker;

#endif
