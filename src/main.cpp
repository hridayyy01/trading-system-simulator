#include "bench/Benchmark.hpp"
#include "cli/Cli.hpp"
#include "engine/MatchingEngine.hpp"
#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

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
                printTrades(order.side, trades);
                total_trades += trades.size();
            } else if (!running.load(std::memory_order_acquire)) {
                break;
            }
        }

        while (queue.pop(order)) {
            const auto trades = engine.processOrder(order, &latency_samples_ns);
            printTrades(order.side, trades);
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

    printBenchmarkReport(config.orders, total_trades, latency_samples_ns,
                         runtime_us);
    return 0;
}
