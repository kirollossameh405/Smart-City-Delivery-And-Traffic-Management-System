#include "../include/delivery.hpp"

bool DeliveryCompare::operator()(const Delivery& a, const Delivery& b) const {
    if (a.priority != b.priority) return a.priority > b.priority;
    return a.deadline < b.deadline;
}