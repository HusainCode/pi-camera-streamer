#pragma once

#include <queue>              // FIFO
#include <mutex>              // locked shared data safely
#include <condition_variable> // lets threads wait and notify and each other
#include <optional>           // allows returning maybe a value or nothing
#include <thread>
#include <vector>

template <typename T>
class Buffer
{
public:
    explicit Buffer(size_t maxSize) : maxSize_(maxSize) {}

    // Blocking push — waits if the queue is full
    void push(const T &item)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condFull_.wait(lock, [this]
                       { return queue_.size() < maxSize_; });
        queue_.push(item);
        condEmpty_.notify_one();
    }

    // Blocking pop — waits if the queue is empty
    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condEmpty_.wait(lock, [this]
                        { return !queue_.empty(); });
        T item = queue_.front();
        queue_.pop();
        condFull_.notify_one();
        return item;
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    size_t maxSize_;
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condFull_;
    std::condition_variable condEmpty_;
};
