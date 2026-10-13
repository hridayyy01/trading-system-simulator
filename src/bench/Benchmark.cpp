#include "bench/Benchmark.hpp"

#include <algorithm>
#include <iostream>

LatencyPercentiles computePercentiles(std::vector<int64_t> samples) {
    std::sort(samples.begin(), samples.end());

    const auto sample_at = [&](double percentile) {
        const auto index =
            static_cast<std::size_t>((samples.size() - 1) * percentile);
        return samples[index];
    };

    return LatencyPercentiles{sample_at(0.50), sample_at(0.95),
                              sample_at(0.99)};
}

void printBenchmarkReport(std::size_t orders, std::uint64_t total_trades,
                          const std::vector<int64_t>& latency_samples_ns,
                          int64_t runtime_us) {
    std::cout << "Total orders: " << orders << '\n';
    std::cout << "Total trades: " << total_trades << '\n';
    if (!latency_samples_ns.empty()) {
        const auto latency = computePercentiles(latency_samples_ns);
        std::cout << "Latency p50: " << latency.p50 << " ns\n";
        std::cout << "Latency p95: " << latency.p95 << " ns\n";
        std::cout << "Latency p99: " << latency.p99 << " ns\n";
    }
    std::cout << "Wall-clock runtime: " << runtime_us << " us\n";
}
