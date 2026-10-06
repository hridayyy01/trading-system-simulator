#include "feed/FeedSimulator.hpp"

#include <chrono>

FeedSimulator::FeedSimulator(std::uint32_t seed) : rng_(seed) {}

Order FeedSimulator::nextOrder() {
    const Side side = side_dist_(rng_) == 0 ? Side::BUY : Side::SELL;
    return Order{next_id_++,
                 side,
                 price_dist_(rng_),
                 qty_dist_(rng_),
                 std::chrono::nanoseconds{next_ts_++},
                 std::chrono::high_resolution_clock::now()};
}

std::size_t FeedSimulator::pushOrders(SPSCQueue<Order>& queue,
                                      std::size_t count) {
    std::size_t pushed = 0;
    for (std::size_t i = 0; i < count; ++i) {
        const Order order = nextOrder();
        while (!queue.push(order)) {
        }
        ++pushed;
    }
    return pushed;
}
