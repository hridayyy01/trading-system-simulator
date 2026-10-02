#include "engine/MatchingEngine.hpp"

namespace {

bool isValid(const Order& order) {
    return order.qty > 0 && order.price > 0.0;
}

}  // namespace

std::vector<Trade> MatchingEngine::processOrder(Order order) {
    if (!isValid(order)) {
        return {};
    }
    if (book_.wouldSelfTrade(order)) {
        return {};
    }

    auto trades = book_.matchOrder(order);
    if (order.qty > 0) {
        book_.addOrder(order);
    }
    return trades;
}

void MatchingEngine::print() const { book_.print(); }
