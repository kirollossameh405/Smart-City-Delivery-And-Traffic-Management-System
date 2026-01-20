#include "../include/scheduler.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

Scheduler::Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db,
                     double minx, double miny, double maxx, double maxy)
    : graph(g), 
      location_db(loc_db),
      location_qt(minx, miny, maxx, maxy),
      vehicle_qt(minx, miny, maxx, maxy),
      delivery_db(101, [](int k){ return static_cast<size_t>(k); }),
      vehicle_db(101, [](int k){ return static_cast<size_t>(k); }) {}

void Scheduler::add_delivery(Delivery del) {
    pending.push(del);
    delivery_db.insert(del.id, del);
}

void Scheduler::add_vehicle(int id, Vehicle veh) {
    veh.id = id;
    vehicle_db.insert(id, veh);
}

void Scheduler::add_location_to_quadtree(Location* loc) {
    location_qt.insert(loc);
}

void Scheduler::update_vehicle_availability(int veh_id, bool available) {
    auto opt = vehicle_db.find(veh_id);
    if (!opt.has_value()) return;
    
    Vehicle updated = opt.value();
    updated.available = available;
    vehicle_db.insert(veh_id, updated);
    
    if (available) {
        available_vehicles[veh_id] = nullptr;
    } else {
        available_vehicles.erase(veh_id);
    }
}

Vehicle* Scheduler::find_nearest_vehicle(double x, double y) {
    double best_dist = std::numeric_limits<double>::infinity();
    int best_id = -1;
    
    for (auto& [id, _] : available_vehicles) {
        auto v_opt = vehicle_db.find(id);
        if (!v_opt.has_value()) continue;
        
        const Vehicle& v = v_opt.value();
        if (!v.available) continue;
        
        double dx = v.current_pos.x - x;
        double dy = v.current_pos.y - y;
        double dist = dx * dx + dy * dy;
        
        if (dist < best_dist) {
            best_dist = dist;
            best_id = id;
        }
    }
    
    if (best_id == -1) return nullptr;
    
    static Vehicle temp_vehicle;
    auto v_opt = vehicle_db.find(best_id);
    if (v_opt.has_value()) {
        temp_vehicle = v_opt.value();
        return &temp_vehicle;
    }
    return nullptr;
}

void Scheduler::assign_delivery(int del_id, int veh_id) {
    auto del_opt = delivery_db.find(del_id);
    auto veh_opt = vehicle_db.find(veh_id);
    
    if (!del_opt.has_value() || !veh_opt.has_value()) return;

    Delivery del = del_opt.value();
    Vehicle veh = veh_opt.value();

    if (veh.current_load + del.weight > veh.capacity) return;

    del.assigned_vehicle = veh_id;
    del.status = "assigned";
    delivery_db.insert(del_id, del);

    veh.assigned_deliveries.push_back(del_id);
    veh.current_load += del.weight;

    vector<int> destinations;
    for (int d : veh.assigned_deliveries) {
        auto d_opt = delivery_db.find(d);
        if (d_opt.has_value()) {
            destinations.push_back(d_opt.value().dest_id);
        }
    }

    if (!destinations.empty()) {
        veh.route = greedy_route(graph, veh.current_pos.id, destinations);
    }

    veh.available = veh.assigned_deliveries.empty();
    
    vehicle_db.insert(veh_id, veh);
    
    if (!veh.available) {
        available_vehicles.erase(veh_id);
    }
}

void Scheduler::process_deliveries() {
    int max_iterations = 1000;
    int iterations = 0;
    
    while (!pending.empty() && iterations < max_iterations) {
        iterations++;
        
        Delivery del = pending.pop();
        if (del.status != "pending") continue;

        auto src_opt = location_db.find(del.source_id);
        if (!src_opt.has_value()) continue;

        Vehicle* v = find_nearest_vehicle(src_opt.value().x, src_opt.value().y);
        if (v) {
            assign_delivery(del.id, v->id);
        } else {
            break;
        }
    }
}

void Scheduler::update_traffic(int from, int to, double new_w) {
    graph.update_edge_weight(from, to, new_w);
}

vector<Delivery> Scheduler::sorted_deliveries() const {
    vector<Delivery> dels;
    
    for (int i = 0; i < 100; ++i) {
        auto opt = delivery_db.find(i);
        if (opt.has_value()) {
            dels.push_back(opt.value());
        }
    }
    
    sort(dels.begin(), dels.end(), [](const Delivery& a, const Delivery& b) {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.deadline < b.deadline;
    });
    
    return dels;
}

HashTable<int, Vehicle>& Scheduler::get_vehicle_db() {
    return vehicle_db;
}

const HashTable<int, Vehicle>& Scheduler::get_vehicle_db() const {
    return vehicle_db;
}