#ifndef UTILS_HPP
#define UTILS_HPP

#include "types.hpp"
#include <vector>

double distance(const Location& a, const Location& b);
void merge_sort(std::vector<Delivery>& arr);
std::vector<Delivery>::iterator binary_search(const std::vector<Delivery>& arr, const TimePoint& tp);

#endif