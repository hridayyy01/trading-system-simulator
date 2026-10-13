#pragma once

#include "orderbook/Order.hpp"
#include "orderbook/Trade.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

struct Config {
    std::size_t orders = 10'000;
    std::uint32_t seed = 42;
};

void printUsage(const char* program);
Config parseArgs(int argc, char* argv[]);
void printTrades(Side aggressor, const std::vector<Trade>& trades);
