#include "engine/MatchingEngine.hpp"

std::vector<Trade> MatchingEngine::processOrder(Order order) {
    auto trades = book_.matchOrder(order);
    if (order.qty > 0) {
        book_.addOrder(order);
    }
    return trades;
}

void MatchingEngine::print() const { book_.print(); }
