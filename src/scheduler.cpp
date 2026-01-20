#include "../include/scheduler.hpp"
#include "../include/utils.hpp"
#include "../include/route_optimizer.hpp"
#include <algorithm>
#include <cmath>

Scheduler::Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db, double minx, double miny, double maxx, double maxy) 
    : qt(minx, miny, maxx, maxy), graph(g), location_db(loc_db) {}

void Scheduler::add_delivery(Delivery del) {
    pending.push(del);
    delivery_db.insert(del.id, del);
}

Vehicle* Scheduler::find_nearest_vehicle(double x, double y) {
    Vehicle* nearest = nullptr;
    double min_dist = numeric_limits<double>::infinity();

    for (size_t i = 0; i < vehicle_db.size(); ++i) {
        for (int id = 0; id < 1000; ++id) {
            auto opt = vehicle_db.find(id);
            if (!opt) continue;
            if (!opt->available) continue;

            double dist = hypot(opt->current_pos.x - x, opt->current_pos.y - y);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = &opt.value();
            }
        }
    }

    return nearest;
}

void Scheduler::assign_delivery(int del_id, int veh_id) {
    auto del_opt = delivery_db.find(del_id);
    auto veh_opt = vehicle_db.find(veh_id);
    if (del_opt && veh_opt && veh_opt->current_load + del_opt->weight <= veh_opt->capacity) {
        del_opt->assigned_vehicle = veh_id;
        del_opt->status = "assigned";
        veh_opt->assigned_deliveries.push_back(del_id);
        veh_opt->current_load += del_opt->weight;
        veh_opt->available = false;
    }
}

void Scheduler::process_deliveries() {
    while (!pending.empty()) {
        Delivery del = pending.pop();
        if (del.status != "pending") continue;
        auto loc_opt = location_db.find(del.source_id);
        if (!loc_opt) continue;
        Vehicle* v = find_nearest_vehicle(loc_opt->x, loc_opt->y);
        if (v) {
            assign_delivery(del.id, v->id);
            vector<int> dests;
            for (int d : v->assigned_deliveries) dests.push_back(delivery_db.find(d).value().dest_id);
            v->assigned_deliveries = greedy_route(graph, v->current_pos.id, dests);
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
        if (opt) dels.push_back(opt.value());
    }
    sort(dels.begin(), dels.end(), [](const Delivery& a, const Delivery& b){
        return a.deadline < b.deadline;
    });
    return dels;
}