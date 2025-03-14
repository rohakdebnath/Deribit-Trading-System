#include "latency_tracker.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "latency_tracker.hpp"

LatencyTracker latency_tracker;

void LatencyTracker::start(const std::string& label) {
    std::lock_guard<std::mutex> lock(mtx);
    start_times[label] = std::chrono::high_resolution_clock::now();
}

void LatencyTracker::stop(const std::string& label) {
    auto t2 = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(mtx);

    auto it = start_times.find(label);
    if (it != start_times.end()) {
        auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - it->second).count();
        latencies.push_back(duration_us);
        // std::cout << "[LatencyTracker] Latency for label '" << label << "': " << duration_us << " μs" << std::endl;
        std::cout << duration_us << std::endl;
        start_times.erase(it);
    }
}

void LatencyTracker::reset() {
    std::lock_guard<std::mutex> lock(mtx);
    start_times.clear();
    latencies.clear();
}

int LatencyTracker::get_entry_count() const {
    std::lock_guard<std::mutex> lock(mtx);
    return latencies.size();
}

void LatencyTracker::print_report() const {
    std::lock_guard<std::mutex> lock(mtx);
    if (latencies.empty()) {
        std::cout << "[LatencyTracker] No measurements recorded.\n";
        return;
    }

    auto min_latency = *std::min_element(latencies.begin(), latencies.end());
    auto max_latency = *std::max_element(latencies.begin(), latencies.end());
    long long sum = std::accumulate(latencies.begin(), latencies.end(), 0LL);
    double mean = static_cast<double>(sum) / latencies.size();

    std::cout << "\n========= LATENCY REPORT =========\n";
    std::cout << "  Measurements: " << latencies.size() << "\n";
    std::cout << "  Min Latency : " << min_latency << " μs\n";
    std::cout << "  Max Latency : " << max_latency << " μs\n";
    std::cout << "  Mean Latency: " << mean << " μs\n";
    std::cout << "==================================\n";
}
