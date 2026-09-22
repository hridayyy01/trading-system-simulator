#pragma once

#include "orderbook/Order.hpp"

#include <deque>
#include <map>

class OrderBook {
public:
    void addOrder(const Order& order);
    bool cancelOrder(uint64_t id);
    void print() const;

private:
    std::map<double, std::deque<Order>, std::greater<double>> bids_;
    std::map<double, std::deque<Order>> asks_;
};
