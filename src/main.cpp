#include "orderbook/OrderBook.hpp"

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
    OrderBook book;

    book.addOrder(Order{1, Side::SELL, 100.0, 10, nanoseconds{1}});
    book.addOrder(Order{2, Side::SELL, 100.0, 5, nanoseconds{2}});
    book.addOrder(Order{3, Side::SELL, 101.0, 8, nanoseconds{3}});

    Order buy{10, Side::BUY, 100.5, 12, nanoseconds{10}};
    const auto trades = book.matchOrder(buy);

    std::cout << "Incoming buy id=10 qty remaining=" << buy.qty << "\nTrades:\n";
    printTrades(trades);

    std::cout << "\nBook after match:\n";
    book.print();

    book.addOrder(Order{4, Side::BUY, 99.0, 7, nanoseconds{4}});

    Order sell{20, Side::SELL, 98.0, 4, nanoseconds{20}};
    const auto sell_trades = book.matchOrder(sell);

    std::cout << "\nIncoming sell id=20 qty remaining=" << sell.qty
              << "\nTrades:\n";
    printTrades(sell_trades);

    std::cout << "\nBook after sell match:\n";
    book.print();

    return 0;
}
