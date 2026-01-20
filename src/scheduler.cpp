#include "../include/scheduler.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

Scheduler::Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db,
                     double minx, double miny, double maxx, double maxy)
    : graph(g), location_db(loc_db),
      location_qt(minx, miny, maxx, maxy),
      vehicle_qt(minx, miny, maxx, maxy) {}

void Scheduler::add_delivery(Delivery del) {
    pending.push(del);
    delivery_db.insert(del.id, del);
}

void Scheduler::add_vehicle(int id, Vehicle veh) {
    vehicle_db.insert(id, std::move(veh));

    auto opt = vehicle_db.find(id);
    if (!opt.has_value()) return;  

    Vehicle& stored = opt.value();

    if (stored.available) {
        available_vehicles[id] = &stored;
        vehicle_qt.insert(&stored.current_pos);
    }
}

void Scheduler::add_location_to_quadtree(Location* loc) {
    location_qt.insert(loc);
}

void Scheduler::update_vehicle_availability(int veh_id, bool available) {
    auto opt = vehicle_db.find(veh_id);
    if (!opt) return;
    Vehicle& v = opt.value();

    if (available && !v.available) {
        v.available = true;
        available_vehicles[veh_id] = &v;
        vehicle_qt.insert(&v.current_pos);
    }
    else if (!available && v.available) {
        v.available = false;
        available_vehicles.erase(veh_id);
    }
}

Vehicle* Scheduler::find_nearest_vehicle(double x, double y) {
    if (available_vehicles.empty()) return nullptr;

    Location* nearest_loc = vehicle_qt.find_nearest(x, y);
    if (!nearest_loc) return nullptr;

    for (auto& [id, veh_ptr] : available_vehicles) {
        if (&veh_ptr->current_pos == nearest_loc) {
            return veh_ptr;
        }
    }
    return nullptr;
}

void Scheduler::assign_delivery(int del_id, int veh_id) {
    auto del_opt = delivery_db.find(del_id);
    auto veh_opt = vehicle_db.find(veh_id);
    if (!del_opt || !veh_opt) return;

    Delivery& del = del_opt.value();
    Vehicle& veh = veh_opt.value();

    if (veh.current_load + del.weight > veh.capacity) return;

    del.assigned_vehicle = veh_id;
    del.status = "assigned";
    veh.assigned_deliveries.push_back(del_id);
    veh.current_load += del.weight;

    vector<int> destinations;
    for (int d : veh.assigned_deliveries) {
        auto d_opt = delivery_db.find(d);
        if (d_opt) destinations.push_back(d_opt->dest_id);
    }

    veh.route = greedy_route(graph, veh.current_pos.id, destinations);

    update_vehicle_availability(veh_id, veh.assigned_deliveries.empty());
}

void Scheduler::process_deliveries() {
    while (!pending.empty()) {
        Delivery del = pending.pop();
        if (del.status != "pending") continue;

        auto src_opt = location_db.find(del.source_id);
        if (!src_opt) continue;

        Vehicle* v = find_nearest_vehicle(src_opt->x, src_opt->y);
        if (v) {
            assign_delivery(del.id, v->id);
        } else {
            pending.push(del);
        }
    }
}

void Scheduler::update_traffic(int from, int to, double new_w) {
    graph.update_edge_weight(from, to, new_w);
}

vector<Delivery> Scheduler::sorted_deliveries() const {
    vector<Delivery> dels;
    for (int i = 0; i < 1000; ++i) {
        auto opt = delivery_db.find(i);
        if (opt) dels.push_back(opt.value());
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