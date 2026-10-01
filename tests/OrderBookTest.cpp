#include "orderbook/OrderBook.hpp"

#include <chrono>
#include <gtest/gtest.h>

using namespace std::chrono;

static nanoseconds ns(int64_t n) { return nanoseconds{n}; }

TEST(OrderBookTest, AddThenCancel) {
    OrderBook book;

    book.addOrder(Order{1, Side::SELL, 100.0, 10, ns(1)});
    EXPECT_TRUE(book.cancelOrder(1));
    EXPECT_FALSE(book.cancelOrder(1));
    EXPECT_FALSE(book.cancelOrder(99));

    Order buy{2, Side::BUY, 100.0, 5, ns(2)};
    const auto trades = book.matchOrder(buy);
    EXPECT_TRUE(trades.empty());
    EXPECT_EQ(buy.qty, 5u);
}

TEST(OrderBookTest, FullFill) {
    OrderBook book;

    book.addOrder(Order{1, Side::SELL, 100.0, 10, ns(1)});

    Order buy{2, Side::BUY, 100.0, 10, ns(2)};
    const auto trades = book.matchOrder(buy);

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].buyId, 2u);
    EXPECT_EQ(trades[0].sellId, 1u);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
    EXPECT_EQ(trades[0].qty, 10u);
    EXPECT_EQ(buy.qty, 0u);
}

TEST(OrderBookTest, PartialFill) {
    OrderBook book;

    book.addOrder(Order{1, Side::SELL, 100.0, 10, ns(1)});

    Order buy{2, Side::BUY, 100.0, 4, ns(2)};
    const auto trades = book.matchOrder(buy);

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].qty, 4u);
    EXPECT_EQ(buy.qty, 0u);

    Order buy2{3, Side::BUY, 100.0, 10, ns(3)};
    const auto trades2 = book.matchOrder(buy2);

    ASSERT_EQ(trades2.size(), 1u);
    EXPECT_EQ(trades2[0].sellId, 1u);
    EXPECT_EQ(trades2[0].qty, 6u);
    EXPECT_EQ(buy2.qty, 4u);
}

TEST(OrderBookTest, PriceTimePriority) {
    OrderBook book;

    book.addOrder(Order{1, Side::SELL, 100.0, 5, ns(1)});
    book.addOrder(Order{2, Side::SELL, 100.0, 5, ns(2)});

    Order buy{3, Side::BUY, 100.0, 5, ns(3)};
    const auto trades = book.matchOrder(buy);

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].sellId, 1u);
    EXPECT_EQ(trades[0].qty, 5u);
}
