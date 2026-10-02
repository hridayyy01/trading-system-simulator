#pragma once

#include <array>
#include <atomic>
#include <cstddef>

template <typename T>
class SPSCQueue {
public:
    static constexpr std::size_t capacity = 1024;

    bool push(const T& value) {
        const std::size_t head = head_.load(std::memory_order_acquire);
        const std::size_t tail = tail_.load(std::memory_order_acquire);
        if (head - tail == capacity) {
            return false;
        }

        buffer_[head % capacity] = value;
        head_.store(head + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        const std::size_t tail = tail_.load(std::memory_order_acquire);
        const std::size_t head = head_.load(std::memory_order_acquire);
        if (tail == head) {
            return false;
        }

        value = buffer_[tail % capacity];
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

private:
    std::array<T, capacity> buffer_{};

    // Separate cache lines keep producer and consumer index updates from
    // invalidating each other's cache line (false sharing).
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};