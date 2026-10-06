#include "engine/MatchingEngine.hpp"
#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

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
    return 0;
}
