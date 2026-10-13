#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

struct LatencyPercentiles {
    int64_t p50;
    int64_t p95;
    int64_t p99;
};

LatencyPercentiles computePercentiles(std::vector<int64_t> samples);
void printBenchmarkReport(std::size_t orders, std::uint64_t total_trades,
                          const std::vector<int64_t>& latency_samples_ns,
                          int64_t runtime_us);
