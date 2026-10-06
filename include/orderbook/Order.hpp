#pragma once

#include <chrono>
#include <cstdint>

enum class Side { BUY, SELL };

struct Order {
    uint64_t id;
    Side side;
    double price;
    uint32_t qty;
    std::chrono::nanoseconds ts;
    std::chrono::high_resolution_clock::time_point created_at =
        std::chrono::high_resolution_clock::now();
};
