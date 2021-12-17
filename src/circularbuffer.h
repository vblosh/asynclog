#pragma once
#include <vector>
#include <assert.h>

namespace simplelogger
{

template<typename T,
    typename Allocator = std::allocator<T>>
    class circullar_buffer
{
protected:
    std::vector<T, Allocator> data;
    size_t head, tail;

public:
    explicit circullar_buffer(size_t capacity)
        : head(0), tail(0)
    {
        data.resize(capacity);
    }

    ~circullar_buffer()
    {
    }

    bool empty()
    {
        return head == tail;
    }

    size_t size()
    {
        if (tail < head)
            return tail + _capacity - head;
        else
            return tail - head;
    }

    size_t capacity()
    {
        return data.capacity();
    }

    T& front()
    {
        assert(!empty());
        return data[head];
    }

    void push_back(T const& value)
    {
        data[tail] = value;
        tail = (tail + 1) % capacity();
        if (tail == head) { // buffer full, resize it
            size_t old_capacity = data.capacity();
            data.resize(data.capacity() * 2);
            std::copy(data.begin(), data.begin() + tail, data.begin() + old_capacity);
            tail = old_capacity + tail;
        }
    }

    void pop_front()
    {
        assert(!empty());
        head = (head + 1) % capacity();
    }
};

}