#pragma once

#include "orderbook/Order.hpp"
#include "orderbook/OrderBook.hpp"
#include "orderbook/Trade.hpp"

#include <vector>

class MatchingEngine {
public:
    std::vector<Trade> processOrder(Order order);
    void print() const;

private:
    OrderBook book_;
};
