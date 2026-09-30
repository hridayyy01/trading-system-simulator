#pragma once

#include "orderbook/Order.hpp"
#include "orderbook/Trade.hpp"

#include <deque>
#include <map>
#include <vector>

class OrderBook {
public:
    void addOrder(const Order& order);
    bool cancelOrder(uint64_t id);
    std::vector<Trade> matchOrder(Order& order);
    void print() const;

private:
    std::map<double, std::deque<Order>, std::greater<double>> bids_;
    std::map<double, std::deque<Order>> asks_;
};
