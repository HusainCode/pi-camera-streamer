#pragma once
#include "buffer.hpp"

template <typename T>
Buffer<T>::Buffer(size_t maxSize) : maxSize_(maxSize) {}

template <typename T>
void Buffer<T>::push(const T &item)
{
    std::unique_lock<std::mutex> lock(mutex_);
    condFull_.wait(lock, [this]
                   { return queue_.size() < maxSize_; });
    queue_.push(item);
    condEmpty_.notify_one();
}

template <typename T>
std::optional<T> Buffer<T>::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    condEmpty_.wait(lock, [this]
                    { return !queue_.empty(); });
    if (queue_.empty())
    {
        return std::nullopt;
    }
    T item = std::move(queue_.front());
    queue_.pop();
    condFull_.notify_one();
    return item;
}

template <typename T>
size_t Buffer<T>::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}
