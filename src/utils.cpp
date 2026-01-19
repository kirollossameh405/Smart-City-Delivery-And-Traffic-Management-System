#include "../include/utils.hpp"
#include <algorithm>
#include <cmath>

double distance(const Location& a, const Location& b) {
    return hypot(a.x - b.x, a.y - b.y);
}

void merge(vector<Delivery>& arr, int left, int mid, int right) {
    vector<Delivery> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (arr[i].deadline <= arr[j].deadline) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int p = 0; p < k; ++p) arr[left + p] = temp[p];
}

void merge_sort_helper(vector<Delivery>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort_helper(arr, left, mid);
    merge_sort_helper(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void merge_sort(vector<Delivery>& arr) {
    merge_sort_helper(arr, 0, arr.size() - 1);
}

vector<Delivery>::const_iterator find_delivery_by_deadline(const vector<Delivery>& arr, const TimePoint& tp) {
    return lower_bound(arr.begin(), arr.end(), tp, [](const Delivery& d, const TimePoint& t){
        return d.deadline < t;
    });
}