#include "../include/priority_queue.hpp"
#include "../include/delivery.hpp"

template<typename T, typename Compare>
void PriorityQueue<T, Compare>::heapify_up(size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (comp(heap[idx], heap[parent])) {
            swap(heap[idx], heap[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

template<typename T, typename Compare>
void PriorityQueue<T, Compare>::heapify_down(size_t idx) {
    size_t n = heap.size();
    while (true) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;
        if (left < n && comp(heap[left], heap[smallest])) smallest = left;
        if (right < n && comp(heap[right], heap[smallest])) smallest = right;
        if (smallest != idx) {
            swap(heap[idx], heap[smallest]);
            idx = smallest;
        } else {
            break;
        }
    }
}

template<typename T, typename Compare>
void PriorityQueue<T, Compare>::push(T item) {
    heap.push_back(move(item));
    heapify_up(heap.size() - 1);
}

template<typename T, typename Compare>
T PriorityQueue<T, Compare>::pop() {
    if (empty()) throw runtime_error("PQ empty");
    T item = move(heap[0]);
    heap[0] = move(heap.back());
    heap.pop_back();
    if (!empty()) heapify_down(0);
    return item;
}

template<typename T, typename Compare>
const T& PriorityQueue<T, Compare>::top() const {
    if (empty()) throw runtime_error("PQ empty");
    return heap[0];
}

template<typename T, typename Compare>
bool PriorityQueue<T, Compare>::empty() const {
    return heap.empty();
}

template<typename T, typename Compare>
size_t PriorityQueue<T, Compare>::size() const {
    return heap.size();
}

template<typename T, typename Compare>
void PriorityQueue<T, Compare>::update_priority(const T& item) {
    for (auto& e : heap) {
        if (e == item) {
            break;
        }
    }
    make_heap(heap.begin(), heap.end(), comp);
}

struct DeliveryCompare {
    bool operator()(const Delivery& a, const Delivery& b) const {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.deadline < b.deadline;
    }
};

template class PriorityQueue<Delivery, DeliveryCompare>;