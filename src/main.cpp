#include "orderbook/OrderBook.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono;

int main() {
    OrderBook book;

    book.addOrder(Order{1, Side::BUY, 100.0, 10, nanoseconds{1}});
    book.addOrder(Order{2, Side::BUY, 100.0, 5, nanoseconds{2}});
    book.addOrder(Order{3, Side::SELL, 101.0, 8, nanoseconds{3}});

    std::cout << "After add:\n";
    book.print();

    const bool cancelled = book.cancelOrder(1);
    std::cout << "\nAfter cancel id=1 (found=" << std::boolalpha << cancelled
              << "):\n";
    book.print();

    const bool missing = book.cancelOrder(99);
    std::cout << "\nCancel id=99 (found=" << missing << "):\n";

    return 0;
}
