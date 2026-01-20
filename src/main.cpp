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
    try {
        RoadNetwork graph;
        HashTable<int, Location> loc_db(101, [](int k){ return static_cast<size_t>(k); });
        vector<Location*> all_locs;
        HashTable<int, Vehicle> vehicle_db(101, [](int k){ return static_cast<size_t>(k); });
        vector<Delivery> deliveries;
        HashTable<int, Delivery> delivery_db(101, [](int k){ return static_cast<size_t>(k); });

        cout << "Loading city map..." << endl;
        if (!load_city_map("city_map.txt", graph)) {
            cerr << "Failed to load city map" << endl;
            return 1;
        }

        cout << "Loading locations..." << endl;
        if (!load_locations("locations.txt", loc_db, all_locs)) {
            cerr << "Failed to load locations" << endl;
            return 1;
        }

        double minx = 0, maxx = 100, miny = 0, maxy = 100;
        Scheduler scheduler(graph, loc_db, minx, miny, maxx, maxy);

        cout << "Adding locations to quadtree..." << endl;
        for (auto* loc : all_locs) {
            scheduler.add_location_to_quadtree(loc);
        }

        cout << "Loading vehicles..." << endl;
        if (!load_vehicles("vehicles.txt", vehicle_db, loc_db)) {
            cerr << "Failed to load vehicles" << endl;
            return 1;
        }

        cout << "Registering vehicles..." << endl;
        for (int i = 0; i < 100; ++i) {
            auto opt = vehicle_db.find(i);
            if (opt.has_value()) {
                scheduler.add_vehicle(i, opt.value());
                scheduler.update_vehicle_availability(i, true);
            }
        }

        cout << "Loading deliveries..." << endl;
        if (!load_deliveries("deliveries.txt", deliveries, delivery_db)) {
            cerr << "Failed to load deliveries" << endl;
            return 1;
        }

        cout << "Adding deliveries to scheduler..." << endl;
        for (auto& del : deliveries) {
            scheduler.add_delivery(del);
        }

        cout << "Loading traffic updates..." << endl;
        load_traffic_updates("traffic_updates.txt", graph);

        cout << "Processing deliveries..." << endl;
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

        cout << "\n=== Vehicle Routes ===\n";
        for (int i = 0; i < 100; ++i) {
            auto v_opt = scheduler.get_vehicle_db().find(i);
            if (!v_opt.has_value()) continue;
            const auto& v = v_opt.value();
            if (v.route.empty()) continue;

            cout << "Vehicle " << v.id << " (load: " << v.current_load << "/" << v.capacity << "):\n";
            cout << "  Route: ";
            for (size_t j = 0; j < v.route.size() && j < 10; ++j) {
                cout << v.route[j];
                if (j < v.route.size() - 1) cout << " -> ";
            }
            if (v.route.size() > 10) cout << " ...";
            cout << "\n";
        }

        return 0;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}