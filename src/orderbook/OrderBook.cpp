#include "orderbook/OrderBook.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

namespace {

void recordTradeLatency(const Order& incoming,
                        std::vector<int64_t>* latency_ns) {
    if (latency_ns == nullptr) {
        return;
    }
    const auto exec_time = std::chrono::high_resolution_clock::now();
    latency_ns->push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(exec_time -
                                                             incoming.created_at)
            .count());
}

}  // namespace

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

std::vector<Trade> OrderBook::matchOrder(Order& incoming,
                                         std::vector<int64_t>* latency_ns) {
    std::vector<Trade> trades;
    uint32_t& qty = incoming.qty;

    if (incoming.side == Side::BUY) {
        while (qty > 0 && !asks_.empty()) {
            auto ask_it = asks_.begin();
            const double ask_price = ask_it->first;
            if (incoming.price < ask_price) {
                break;
            }

            auto& level = ask_it->second;
            Order& resting = level.front();
            const uint32_t fill_qty = std::min(qty, resting.qty);
            recordTradeLatency(incoming, latency_ns);
            trades.push_back(
                Trade{incoming.id, resting.id, ask_price, fill_qty});
            qty -= fill_qty;
            resting.qty -= fill_qty;

            if (resting.qty == 0) {
                level.pop_front();
            }
            if (level.empty()) {
                asks_.erase(ask_it);
            }
        }
    } else {
        while (qty > 0 && !bids_.empty()) {
            auto bid_it = bids_.begin();
            const double bid_price = bid_it->first;
            if (incoming.price > bid_price) {
                break;
            }

            auto& level = bid_it->second;
            Order& resting = level.front();
            const uint32_t fill_qty = std::min(qty, resting.qty);
            recordTradeLatency(incoming, latency_ns);
            trades.push_back(
                Trade{resting.id, incoming.id, bid_price, fill_qty});
            qty -= fill_qty;
            resting.qty -= fill_qty;

            if (resting.qty == 0) {
                level.pop_front();
            }
            if (level.empty()) {
                bids_.erase(bid_it);
            }
        }
    }

    return trades;
}

bool OrderBook::wouldSelfTrade(const Order& incoming) const {
    if (incoming.side == Side::BUY) {
        for (const auto& [price, orders] : asks_) {
            if (incoming.price < price) {
                break;
            }
            for (const auto& resting : orders) {
                if (resting.id == incoming.id) {
                    return true;
                }
            }
        }
    } else {
        for (const auto& [price, orders] : bids_) {
            if (incoming.price > price) {
                break;
            }
            for (const auto& resting : orders) {
                if (resting.id == incoming.id) {
                    return true;
                }
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
