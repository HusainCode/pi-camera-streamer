#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class Buffer
{
public:
    explicit Buffer(size_t maxSize);

    void push(const T &item);
    std::optional<T> pop();
    size_t size() const;

private:
    size_t maxSize_;
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condFull_;
    std::condition_variable condEmpty_;
};
