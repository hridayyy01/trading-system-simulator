#include "engine/MatchingEngine.hpp"
#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

struct Config {
    std::size_t orders = 10'000;
    std::uint32_t seed = 42;
};

struct LatencyPercentiles {
    int64_t p50;
    int64_t p95;
    int64_t p99;
};

void printUsage(const char* program) {
    std::cerr << "Usage: " << program << " [--orders N] [--seed N]\n";
}

Config parseArgs(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--orders") {
            if (++i >= argc) {
                throw std::invalid_argument("--orders requires a value");
            }
            config.orders = static_cast<std::size_t>(std::stoull(argv[i]));
        } else if (arg == "--seed") {
            if (++i >= argc) {
                throw std::invalid_argument("--seed requires a value");
            }
            config.seed = static_cast<std::uint32_t>(std::stoul(argv[i]));
        } else {
            throw std::invalid_argument("unknown argument: " + arg);
        }
    }

    return config;
}

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

int main(int argc, char* argv[]) {
    Config config;
    try {
        config = parseArgs(argc, argv);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    SPSCQueue<Order> queue;
    MatchingEngine engine;
    std::atomic<bool> running{true};
    std::uint64_t total_trades = 0;
    std::vector<int64_t> latency_samples_ns;

    const auto start_time = std::chrono::high_resolution_clock::now();
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
        FeedSimulator feed{config.seed};
        feed.pushOrders(queue, config.orders);
        running.store(false, std::memory_order_release);
    });

    producer.join();
    consumer.join();
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto runtime_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                              start_time)
            .count();

    std::cout << "Total orders: " << config.orders << '\n';
    std::cout << "Total trades: " << total_trades << '\n';
    if (!latency_samples_ns.empty()) {
        const auto latency = computePercentiles(latency_samples_ns);
        std::cout << "Latency p50: " << latency.p50 << " ns\n";
        std::cout << "Latency p95: " << latency.p95 << " ns\n";
        std::cout << "Latency p99: " << latency.p99 << " ns\n";
    }
    std::cout << "Wall-clock runtime: " << runtime_us << " us\n";
    return 0;
}
