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
            cerr << "Warning: Could not load city map" << endl;
        }

        cout << "Loading locations..." << endl;
        if (!load_locations("locations.txt", loc_db, all_locs)) {
            cerr << "Warning: Could not load locations" << endl;
        }

        cout << "Loaded " << all_locs.size() << " locations." << endl;

        double minx = 0, maxx = 100, miny = 0, maxy = 100;
        
        if (!all_locs.empty()) {
            minx = maxx = all_locs[0]->x;
            miny = maxy = all_locs[0]->y;
            
            for (auto* loc : all_locs) {
                if (loc->x < minx) minx = loc->x;
                if (loc->x > maxx) maxx = loc->x;
                if (loc->y < miny) miny = loc->y;
                if (loc->y > maxy) maxy = loc->y;
            }
            
            double padding = 10.0;
            minx -= padding;
            maxx += padding;
            miny -= padding;
            maxy += padding;
            
            cout << "QuadTree bounds: (" << minx << "," << miny << ") to (" << maxx << "," << maxy << ")" << endl;
        }

        Scheduler scheduler(graph, loc_db, minx, miny, maxx, maxy);

        cout << "Adding locations to quadtree..." << endl;
        int count = 0;
        for (auto* loc : all_locs) {
            if (loc == nullptr) {
                cout << "Warning: null location pointer at index " << count << endl;
                continue;
            }
            
            cout << "Adding location " << count << ": " << loc->name 
                 << " at (" << loc->x << ", " << loc->y << ")" << endl;
            
            scheduler.add_location_to_quadtree(loc);
            count++;
            
            if (count >= 10) {
                cout << "Limited to first 10 locations for testing" << endl;
                break;
            }
        }
        cout << "Successfully added " << count << " locations to quadtree." << endl;

        cout << "Loading vehicles..." << endl;
        if (!load_vehicles("vehicles.txt", vehicle_db, loc_db)) {
            cerr << "Warning: Could not load vehicles" << endl;
        }

        cout << "Registering vehicles..." << endl;
        int veh_count = 0;
        for (int i = 0; i < 100; ++i) {
            auto opt = vehicle_db.find(i);
            if (opt.has_value()) {
                scheduler.add_vehicle(i, opt.value());
                scheduler.update_vehicle_availability(i, true);
                veh_count++;
            }
        }
        cout << "Registered " << veh_count << " vehicles." << endl;

        cout << "Loading deliveries..." << endl;
        if (!load_deliveries("deliveries.txt", deliveries, delivery_db)) {
            cerr << "Warning: Could not load deliveries" << endl;
        }
        cout << "Loaded " << deliveries.size() << " deliveries." << endl;

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
        cerr << "Exception caught: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown exception caught" << endl;
        return 1;
    }
}