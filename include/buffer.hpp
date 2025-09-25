#pragma once

#include <queue>              // FIFO
#include <mutex>              // locked shared data safely
#include <condition_variable> // lets threads wait and notify and each other
#include <optional>           // allows returning maybe a value or nothing
#include <thread>
#include <vector>

template <typename type>
class Buffer
{
public:
    explicit Buffer(size_t maxSize) : maxSize_(maxSize) {}

    // Add iteam to the buffer blocks if full
    void push(const type &item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condFull_.wait(lock, [this]()
                       { return queue_.size() < maxSize_; };)
            queue_.push(item);
        condEmpty_.notify_one();
    }

private:
    size_t maxSize_;
    std::queue<type> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condFull_;
    std::condition_variable condEmpty_;
};