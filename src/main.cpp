#include "../include/road_network.hpp"
#include "../include/hash_table.hpp"
#include "../include/quadtree.hpp"
#include "../include/scheduler.hpp"
#include "../include/file_io.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    RoadNetwork graph;
    HashTable<int, Location> loc_db(101, [](int k){ return static_cast<size_t>(k); });
    vector<Location*> all_locs;
    HashTable<int, Vehicle> vehicle_db;
    vector<Delivery> deliveries;
    HashTable<int, Delivery> delivery_db;

    if (!load_city_map("city_map.txt", graph)) {
        cerr << "Failed to load city map" << endl;
        return 1;
    }

    if (!load_locations("locations.txt", loc_db, all_locs)) {
        cerr << "Failed to load locations" << endl;
        return 1;
    }

    double minx = 0, maxx = 100, miny = 0, maxy = 100;
    Scheduler scheduler(graph, loc_db, minx, miny, maxx, maxy);

    for (auto* loc : all_locs) {
        scheduler.add_location_to_quadtree(loc);
    }

    if (!load_vehicles("vehicles.txt", vehicle_db, loc_db)) {
        cerr << "Failed to load vehicles" << endl;
        return 1;
    }

    for (int i = 0; i < 1000; ++i) {
        auto opt = vehicle_db.find(i);
        if (opt) {
            scheduler.add_vehicle(i, opt.value());
        }
    }

    if (!load_deliveries("deliveries.txt", deliveries, delivery_db)) {
        cerr << "Failed to load deliveries" << endl;
        return 1;
    }

    for (auto del : deliveries) {
        scheduler.add_delivery(del);
    }

    load_traffic_updates("traffic_updates.txt", graph);

    scheduler.process_deliveries();

    cout << "\n=== Processed Deliveries ===\n";
    auto sorted = scheduler.sorted_deliveries();

    if (sorted.empty()) {
        cout << "No deliveries loaded or processed.\n";
    } else {
        for (const auto& del : sorted) {
            cout << "Delivery #" << del.id 
                 << " | From: " << del.source_id 
                 << " -> To: " << del.dest_id
                 << " | Priority: " << del.priority
                 << " | Status: " << del.status
                 << " | Assigned Vehicle: " << (del.assigned_vehicle != -1 ? to_string(del.assigned_vehicle) : "None")
                 << endl;
        }
    }

    cout << "\nAvailable vehicles in scheduler:\n";
    for (int i = 0; i < 1000; ++i) {
        auto v = scheduler.get_vehicle_db().find(i);
        if (v) {
            cout << "Vehicle " << v->id << " at location " << v->current_pos.id 
                 << " | Capacity: " << v->capacity << " | Available: " << v->available << endl;
        }
    }

    cout << "\n=== Final Delivery Status & Routes ===\n";
    for (const auto& del : sorted) {
        cout << "Delivery " << del.id << " to " << del.dest_id << endl;
    }

    cout << "\n=== Vehicle Routes ===\n";
    for (int i = 0; i < 1000; ++i) {
        auto v_opt = scheduler.get_vehicle_db().find(i);
        if (!v_opt) continue;
        const auto& v = v_opt.value();
        if (v.route.empty()) continue;

        cout << "Vehicle " << v.id << " (load: " << v.current_load << "/" << v.capacity << "):\n";
        cout << "  Route: ";
        for (int node : v.route) cout << node << " -> ";
        cout << "\n";
    }

    return 0;
}