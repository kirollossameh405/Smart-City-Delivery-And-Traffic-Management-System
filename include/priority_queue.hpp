#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include <vector>
#include <functional>
#include <utility>

using namespace std;

template<typename T, typename Compare = less<T>>
class PriorityQueue {
private:
    vector<T> heap;
    Compare comp;

    void heapify_up(size_t idx);
    void heapify_down(size_t idx);

public:
    PriorityQueue(Compare c = Compare()) : comp(c) {}

    void push(T item);
    T pop();
    const T& top() const;
    bool empty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }
    void update_priority(const T& item); 
};

#endif