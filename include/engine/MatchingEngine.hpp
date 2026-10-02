#pragma once

#include "orderbook/Order.hpp"
#include "orderbook/OrderBook.hpp"
#include "orderbook/Trade.hpp"

#include <vector>

// Self-trade policy: if an incoming order would cross resting liquidity
// with the same order id, the entire order is rejected (no trades, not
// added to the book).

class MatchingEngine {
public:
    std::vector<Trade> processOrder(Order order);
    void print() const;

private:
    OrderBook book_;
};
