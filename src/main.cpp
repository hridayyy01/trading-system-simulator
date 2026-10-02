#include "engine/MatchingEngine.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono;

static void printTrades(const std::vector<Trade>& trades) {
    for (const auto& trade : trades) {
        std::cout << "  buyId=" << trade.buyId << " sellId=" << trade.sellId
                  << " price=" << trade.price << " qty=" << trade.qty << '\n';
    }
    if (trades.empty()) {
        std::cout << "  (none)\n";
    }
}

int main() {
    MatchingEngine engine;

    engine.processOrder(Order{1, Side::SELL, 100.0, 10, nanoseconds{1}});
    engine.processOrder(Order{2, Side::SELL, 100.0, 5, nanoseconds{2}});
    engine.processOrder(Order{3, Side::SELL, 101.0, 8, nanoseconds{3}});

    const auto trades =
        engine.processOrder(Order{10, Side::BUY, 100.5, 12, nanoseconds{10}});

    std::cout << "Incoming buy id=10\nTrades:\n";
    printTrades(trades);

    std::cout << "\nBook after match:\n";
    engine.print();

    engine.processOrder(Order{4, Side::BUY, 99.0, 7, nanoseconds{4}});

    const auto sell_trades =
        engine.processOrder(Order{20, Side::SELL, 98.0, 4, nanoseconds{20}});

    std::cout << "\nIncoming sell id=20\nTrades:\n";
    printTrades(sell_trades);

    std::cout << "\nBook after sell match:\n";
    engine.print();

    return 0;
}
