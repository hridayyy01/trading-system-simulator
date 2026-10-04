#pragma once

#include "orderbook/Order.hpp"
#include "queue/SPSCQueue.hpp"

#include <cstddef>
#include <cstdint>
#include <random>

class FeedSimulator {
public:
    explicit FeedSimulator(std::uint32_t seed = 42);

    Order nextOrder();
    std::size_t pushOrders(SPSCQueue<Order>& queue, std::size_t count);

private:
    std::mt19937 rng_;
    std::uniform_int_distribution<int> side_dist_{0, 1};
    std::uniform_real_distribution<double> price_dist_{99.5, 100.5};
    std::uniform_int_distribution<std::uint32_t> qty_dist_{1, 50};
    std::uint64_t next_id_{1};
    std::int64_t next_ts_{1};
};
