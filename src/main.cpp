#include "engine/MatchingEngine.hpp"
#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <atomic>
#include <cstddef>
#include <iostream>
#include <thread>

int main() {
    SPSCQueue<Order> queue;
    MatchingEngine engine;
    std::atomic<bool> running{true};
    std::uint64_t total_trades = 0;

    constexpr std::size_t order_count = 10'000;

    std::thread consumer([&] {
        Order order{};
        while (true) {
            if (queue.pop(order)) {
                const auto trades = engine.processOrder(order);
                total_trades += trades.size();
            } else if (!running.load(std::memory_order_acquire)) {
                break;
            }
        }

        while (queue.pop(order)) {
            const auto trades = engine.processOrder(order);
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
    return 0;
}
