#ifndef DELIVERY_HPP
#define DELIVERY_HPP

#include "types.hpp"
#include "priority_queue.hpp"

using namespace std;

struct DeliveryCompare {
    bool operator()(const Delivery& a, const Delivery& b) const;
};

using DeliveryPQ = PriorityQueue<Delivery, DeliveryCompare>;

#endif