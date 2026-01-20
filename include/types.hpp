#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>
#include <utility>
#include <chrono>

using namespace std; 
using TimePoint = chrono::system_clock::time_point;

struct Location {
    int id;
    string name;
    double x, y;
    string type;
};

struct Vehicle {
    int id;
    double capacity;
    double speed;
    Location current_pos;
    bool available = true;
    vector<int> assigned_deliveries;
    vector<int> route;
    double current_load = 0.0;
};

struct Delivery {
    int id;
    int source_id;
    int dest_id;
    TimePoint deadline;
    int priority;
    double weight;
    string status = "pending";
    int assigned_vehicle = -1;
};

struct Edge {
    int to;
    double weight;
    double base_weight;
};

#endif