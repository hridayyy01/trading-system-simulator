#include "engine/MatchingEngine.hpp"

#include <chrono>
#include <gtest/gtest.h>

using namespace std::chrono;

static nanoseconds ns(int64_t n) { return nanoseconds{n}; }

TEST(MatchingEngineTest, RejectsZeroQty) {
    MatchingEngine engine;

    const auto trades =
        engine.processOrder(Order{1, Side::BUY, 100.0, 0, ns(1)});
    EXPECT_TRUE(trades.empty());

    engine.processOrder(Order{2, Side::SELL, 100.0, 5, ns(2)});
    const auto follow_up =
        engine.processOrder(Order{3, Side::BUY, 100.0, 5, ns(3)});

    ASSERT_EQ(follow_up.size(), 1u);
    EXPECT_EQ(follow_up[0].sellId, 2u);
}

TEST(MatchingEngineTest, RejectsInvalidPrice) {
    MatchingEngine engine;

    EXPECT_TRUE(
        engine.processOrder(Order{1, Side::SELL, 0.0, 5, ns(1)}).empty());
    EXPECT_TRUE(
        engine.processOrder(Order{2, Side::BUY, -10.0, 5, ns(2)}).empty());

    engine.processOrder(Order{3, Side::SELL, 100.0, 5, ns(3)});
    const auto trades =
        engine.processOrder(Order{4, Side::BUY, 100.0, 5, ns(4)});

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].sellId, 3u);
}

TEST(MatchingEngineTest, RejectsSelfTrade) {
    MatchingEngine engine;

    engine.processOrder(Order{5, Side::SELL, 100.0, 10, ns(1)});

    const auto self_trades =
        engine.processOrder(Order{5, Side::BUY, 100.0, 4, ns(2)});
    EXPECT_TRUE(self_trades.empty());

    const auto other_trades =
        engine.processOrder(Order{6, Side::BUY, 100.0, 10, ns(3)});

    ASSERT_EQ(other_trades.size(), 1u);
    EXPECT_EQ(other_trades[0].sellId, 5u);
    EXPECT_EQ(other_trades[0].qty, 10u);
}
