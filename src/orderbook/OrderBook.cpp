#include "orderbook/OrderBook.hpp"

#include <iostream>

void OrderBook::addOrder(const Order& order) {
    if (order.side == Side::BUY) {
        bids_[order.price].push_back(order);
    } else {
        asks_[order.price].push_back(order);
    }
}

bool OrderBook::cancelOrder(uint64_t id) {
    for (auto price_it = bids_.begin(); price_it != bids_.end(); ++price_it) {
        auto& orders = price_it->second;
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if (it->id == id) {
                orders.erase(it);
                if (orders.empty()) {
                    bids_.erase(price_it);
                }
                return true;
            }
        }
    }

    for (auto price_it = asks_.begin(); price_it != asks_.end(); ++price_it) {
        auto& orders = price_it->second;
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if (it->id == id) {
                orders.erase(it);
                if (orders.empty()) {
                    asks_.erase(price_it);
                }
                return true;
            }
        }
    }

    return false;
}

void OrderBook::print() const {
    std::cout << "Bids:\n";
    for (const auto& [price, orders] : bids_) {
        for (const auto& order : orders) {
            std::cout << "  id=" << order.id << " price=" << price
                      << " qty=" << order.qty << '\n';
        }
    }

    std::cout << "Asks:\n";
    for (const auto& [price, orders] : asks_) {
        for (const auto& order : orders) {
            std::cout << "  id=" << order.id << " price=" << price
                      << " qty=" << order.qty << '\n';
        }
    }
}
