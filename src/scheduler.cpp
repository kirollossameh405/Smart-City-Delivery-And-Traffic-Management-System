#include "../include/scheduler.hpp"
#include "../include/utils.hpp"
#include <algorithm>
#include <limits>
#include <iostream>

Scheduler::Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db,
                     double minx, double miny, double maxx, double maxy)
    : graph(g),
      location_db(loc_db),
      location_qt(minx, miny, maxx, maxy),
      vehicle_qt(minx, miny, maxx, maxy),
      delivery_db(101, [](int k){ return static_cast<size_t>(k); }),
      vehicle_db(101, [](int k){ return static_cast<size_t>(k); }),
      qt_min_x(minx), qt_min_y(miny),
      qt_max_x(maxx), qt_max_y(maxy)
{}

void Scheduler::add_delivery(Delivery del) {
    pending.push(del);
    delivery_db.insert(del.id, del);
}

void Scheduler::add_vehicle(Vehicle veh) {
    veh.current_x = veh.current_pos.x;
    veh.current_y = veh.current_pos.y;
    vehicle_db.insert(veh.id, veh);

    auto veh_opt = vehicle_db.find(veh.id);
    if (veh_opt) {
        vehicle_qt.insert_vehicle(*veh_opt);
    }
}

void Scheduler::add_location_to_quadtree(Location* loc) {
    if (loc) location_qt.insert_location(loc);
}

void Scheduler::update_vehicle_position(int veh_id, double new_x, double new_y) {
    auto opt = vehicle_db.find(veh_id);
    if (!opt) return;
    Vehicle* veh = *opt;
    veh->current_x = new_x;
    veh->current_y = new_y;
    rebuild_vehicle_qt();  // Rebuild QuadTree with updated position
}

void Scheduler::rebuild_vehicle_qt() {
    // Use stored bounds to recreate vehicle QuadTree
    vehicle_qt = QuadTree(qt_min_x, qt_min_y, qt_max_x, qt_max_y);

    // Re-insert all vehicles (small number, so efficient enough)
    for (int i = 1; i <= 100; ++i) {
        auto opt = vehicle_db.find(i);
        if (opt) {
            vehicle_qt.insert_vehicle(*opt);
        }
    }
}

pair<Location*, Vehicle*> Scheduler::find_nearest_vehicle(double x, double y) {
    rebuild_vehicle_qt(); // Ensure QuadTree reflects latest positions
    return vehicle_qt.find_nearest_vehicle(x, y);
}

void Scheduler::assign_delivery(int del_id, int veh_id) {
    auto del_opt = delivery_db.find(del_id);
    auto veh_opt = vehicle_db.find(veh_id);
    if (!del_opt || !veh_opt) return;

    Delivery* del = *del_opt;
    Vehicle* veh = *veh_opt;

    if (veh->current_load + del->weight > veh->capacity) return;

    del->assigned_vehicle = veh_id;
    del->status = "assigned";
    veh->assigned_deliveries.push_back(del_id);
    veh->current_load += del->weight;

    vector<int> destinations;
    for (int d : veh->assigned_deliveries) {
        auto d_opt = delivery_db.find(d);
        if (d_opt) destinations.push_back((*d_opt)->dest_id);
    }

    if (!destinations.empty()) {
        veh->route = greedy_route(graph, veh->current_pos.id, destinations);
    }

    veh->available = veh->assigned_deliveries.empty();

    // Simulate movement: move vehicle to the last destination in the route
    if (!veh->route.empty()) {
        int last_dest = veh->route.back();
        auto next_loc_opt = location_db.find(last_dest);
        if (next_loc_opt) {
            const Location* next_loc = *next_loc_opt;
            update_vehicle_position(veh_id, next_loc->x, next_loc->y);
            veh->current_pos = *next_loc;
        }
    }
}

void Scheduler::process_deliveries() {
    const int MAX_ATTEMPTS = 2000;
    int attempts = 0;
    size_t consecutive_fails = 0;          // FIXED: changed to size_t to match initial_size
    size_t initial_size = pending.size();

    while (!pending.empty() && attempts < MAX_ATTEMPTS) {
        attempts++;
        Delivery del = pending.pop();

        if (del.status != "pending") continue;

        auto src_opt = location_db.find(del.source_id);
        if (!src_opt) {
            pending.push(std::move(del));
            consecutive_fails++;
            continue;
        }

        auto [loc, veh] = find_nearest_vehicle((*src_opt)->x, (*src_opt)->y);
        if (veh && veh->current_load + del.weight <= veh->capacity) {
            assign_delivery(del.id, veh->id);
            consecutive_fails = 0;
        } else {
            pending.push(std::move(del));
            consecutive_fails++;
        }

        // No more warning: both are size_t
        if (consecutive_fails > initial_size) break; // No progress possible
    }
}

void Scheduler::update_traffic(int from, int to, double new_weight) {
    graph.update_edge_weight(from, to, new_weight);
}

vector<Delivery> Scheduler::sorted_deliveries() const {
    vector<Delivery> result;
    for (int i = 0; i < 10000; ++i) {
        auto opt = delivery_db.find(i);
        if (opt) result.push_back(**opt);
    }

    sort(result.begin(), result.end(), [](const Delivery& a, const Delivery& b) {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.deadline < b.deadline;
    });

    return result;
}

HashTable<int, Vehicle>& Scheduler::get_vehicle_db() {
    return vehicle_db;
}

const HashTable<int, Vehicle>& Scheduler::get_vehicle_db() const {
    return vehicle_db;
}

Scheduler::Stats Scheduler::get_stats() const {
    Stats s;
    for (int i = 0; i < 10000; ++i) {
        auto opt = delivery_db.find(i);
        if (opt) {
            s.total_deliveries++;
            if ((*opt)->status == "assigned") {
                s.assigned++;
                s.total_load_assigned += (*opt)->weight;
            } else if ((*opt)->status == "pending") {
                s.pending++;
            } else {
                s.unassigned++;
            }
        }
    }
    return s;
}