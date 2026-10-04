#include "feed/FeedSimulator.hpp"
#include "queue/SPSCQueue.hpp"

#include <iostream>

int main() {
    SPSCQueue<Order> queue;
    FeedSimulator feed{42};

    constexpr std::size_t order_count = 100;
    const std::size_t pushed = feed.pushOrders(queue, order_count);

    std::size_t popped = 0;
    Order order{};
    while (queue.pop(order)) {
        ++popped;
    }

    std::cout << "Generated and pushed: " << pushed << '\n';
    std::cout << "Popped from queue: " << popped << '\n';

    if (pushed != order_count || popped != order_count) {
        return 1;
    }

    return 0;
}
