#include "queue/MutexQueue.hpp"

#include <gtest/gtest.h>

TEST(MutexQueueTest, PreservesOrder) {
    MutexQueue<int> queue;
    constexpr int item_count = 100;

    for (int item = 0; item < item_count; ++item) {
        ASSERT_TRUE(queue.push(item));
    }

    for (int expected = 0; expected < item_count; ++expected) {
        int actual = -1;
        ASSERT_TRUE(queue.pop(actual));
        EXPECT_EQ(actual, expected);
    }

    int leftover = -1;
    EXPECT_FALSE(queue.pop(leftover));
}
