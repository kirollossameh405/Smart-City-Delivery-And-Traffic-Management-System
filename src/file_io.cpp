#include "../include/file_io.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

bool load_city_map(const string& filename, RoadNetwork& graph) {
    ifstream file(filename);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int from, to;
        double weight;
        if (iss >> from >> to >> weight) {
            graph.add_edge(from, to, weight);
        }
    }
    return true;
}

bool load_locations(const string& filename, HashTable<int, Location>& loc_db, vector<Location*>& all_locs) {
    ifstream file(filename);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int id;
        string name, type;
        double x, y;
        if (iss >> id >> name >> x >> y >> type) {
            Location loc = {id, name, x, y, type};
            loc_db.insert(id, loc);
            auto loc_opt = loc_db.find(id);
            if (loc_opt) {
                all_locs.push_back(&loc_opt.value());
            }
        }
    }
    return true;
}

bool load_vehicles(const string& filename, HashTable<int, Vehicle>& vehicle_db, HashTable<int, Location>& loc_db) {
    ifstream file(filename);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int id, start_loc;
        double capacity, speed;
        if (iss >> id >> capacity >> speed >> start_loc) {
            auto loc_opt = loc_db.find(start_loc);
            if (loc_opt) {
                Vehicle v = {id, capacity, speed, loc_opt.value()};
                vehicle_db.insert(id, v);
            }
        }
    }
    return true;
}

bool load_deliveries(const string& filename, vector<Delivery>& deliveries, HashTable<int, Delivery>& delivery_db) {
    ifstream file(filename);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int id, source, dest, priority;
        double weight;
        string deadline_str;
        if (iss >> id >> source >> dest >> deadline_str >> priority >> weight) {
            std::tm tm{};
            std::istringstream ss(deadline_str);
            ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
            if (ss.fail()) {
                continue;
            }
            auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
            Delivery d = {id, source, dest, tp, priority, weight};
            deliveries.push_back(d);
            delivery_db.insert(id, d);
        }
    }
    return true;
}

bool load_traffic_updates(const string& filename, RoadNetwork& graph) {
    ifstream file(filename);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int from, to;
        double new_weight;
        string timestamp;
        if (iss >> from >> to >> new_weight >> timestamp) {
            graph.update_edge_weight(from, to, new_weight);
        }
    }
    return true;
}