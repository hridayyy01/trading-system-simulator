#include "engine/MatchingEngine.hpp"
#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

struct LatencyPercentiles {
    int64_t p50;
    int64_t p95;
    int64_t p99;
};

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

int main() {
    SPSCQueue<Order> queue;
    MatchingEngine engine;
    std::atomic<bool> running{true};
    std::uint64_t total_trades = 0;
    std::vector<int64_t> latency_samples_ns;

    constexpr std::size_t order_count = 10'000;

    std::thread consumer([&] {
        Order order{};
        while (true) {
            if (queue.pop(order)) {
                const auto trades =
                    engine.processOrder(order, &latency_samples_ns);
                total_trades += trades.size();
            } else if (!running.load(std::memory_order_acquire)) {
                break;
            }
        }

        while (queue.pop(order)) {
            const auto trades = engine.processOrder(order, &latency_samples_ns);
            total_trades += trades.size();
        }
    });

    std::thread producer([&] {
        FeedSimulator feed{42};
        feed.pushOrders(queue, order_count);
        running.store(false, std::memory_order_release);
    });

    producer.join();
    consumer.join();

    std::cout << "Total trades: " << total_trades << '\n';
    std::cout << "Latency samples collected: " << latency_samples_ns.size()
              << '\n';
    if (!latency_samples_ns.empty()) {
        const auto latency = computePercentiles(latency_samples_ns);
        std::cout << "Latency p50: " << latency.p50 << " ns ("
                  << latency.p50 / 1000.0 << " us)\n";
        std::cout << "Latency p95: " << latency.p95 << " ns ("
                  << latency.p95 / 1000.0 << " us)\n";
        std::cout << "Latency p99: " << latency.p99 << " ns ("
                  << latency.p99 / 1000.0 << " us)\n";
    }
    return 0;
}
