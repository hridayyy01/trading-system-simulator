#include "engine/MatchingEngine.hpp"

namespace {

bool isValid(const Order& order) {
    return order.qty > 0 && order.price > 0.0;
}

}  // namespace

std::vector<Trade> MatchingEngine::processOrder(Order order,
                                                std::vector<int64_t>* latency_ns) {
    if (!isValid(order)) {
        return {};
    }
    if (book_.wouldSelfTrade(order)) {
        return {};
    }

    auto trades = book_.matchOrder(order, latency_ns);
    if (order.qty > 0) {
        book_.addOrder(order);
    }
    return trades;
}

void MatchingEngine::print() const { book_.print(); }
