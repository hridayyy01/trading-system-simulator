#include "queue/SPSCQueue.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <thread>

TEST(SPSCQueueStressTest, MillionItemsInOrder) {
    SPSCQueue<uint64_t> queue;
    constexpr uint64_t item_count = 1'000'000;

    uint64_t popped = 0;
    uint64_t expected = 0;
    uint64_t sum = 0;

    std::thread consumer([&] {
        uint64_t value = 0;
        while (popped < item_count) {
            if (queue.pop(value)) {
                ASSERT_EQ(value, expected);
                ++expected;
                sum += value;
                ++popped;
            }
        }
    });

    std::thread producer([&] {
        for (uint64_t item = 0; item < item_count; ++item) {
            while (!queue.push(item)) {
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(popped, item_count);
    EXPECT_EQ(sum, item_count * (item_count - 1) / 2);
}
