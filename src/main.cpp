#include "../include/road_network.hpp"
#include "../include/hash_table.hpp"
#include "../include/quadtree.hpp"
#include "../include/scheduler.hpp"
#include "../include/file_io.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main() {
    try {
        cout << "=== Smart City Delivery & Traffic Management System ===\n\n";
        RoadNetwork graph;
        HashTable<int, Location> loc_db(101, [](int k){ return static_cast<size_t>(k); });
        vector<Location*> all_locs;

        cout << "Loading city map...\n";
        if (!load_city_map("city_map.txt", graph)) cerr << "Warning: Could not load city map\n";
        cout << "Loading locations...\n";
        if (!load_locations("locations.txt", loc_db, all_locs)) cerr << "Warning: Could not load locations\n";
        cout << "Loaded " << all_locs.size() << " locations\n";
        double minx = 0, maxx = 100, miny = 0, maxy = 100;
        if (!all_locs.empty()) {
            minx = maxx = all_locs[0]->x;
            miny = maxy = all_locs[0]->y;
            for (const auto* loc : all_locs) {
                minx = min(minx, loc->x);
                maxx = max(maxx, loc->x);
                miny = min(miny, loc->y);
                maxy = max(maxy, loc->y);
            }
            double padding = 20.0;
            minx -= padding; maxx += padding;
            miny -= padding; maxy += padding;
        }

        Scheduler scheduler(graph, loc_db, minx, miny, maxx, maxy);

        cout << "Building location QuadTree...\n";
        for (auto* loc : all_locs) {
            if (loc) scheduler.add_location_to_quadtree(loc);
        }

        HashTable<int, Vehicle> vehicle_db(101, [](int k){ return static_cast<size_t>(k); });
        vector<int> vehicle_ids;
        cout << "Loading vehicles...\n";
        if (!load_vehicles("vehicles.txt", vehicle_db, loc_db, vehicle_ids)) cerr << "Warning: Could not load vehicles\n";
        cout << "Registering vehicles...\n";
        int veh_count = 0;
        for (int id : vehicle_ids) {
            auto opt = vehicle_db.find(id);
            if (opt) {
                scheduler.add_vehicle(**opt);
                veh_count++;
            }
        }
        cout << "Registered " << veh_count << " vehicles\n";

        vector<Delivery> deliveries;
        HashTable<int, Delivery> delivery_db(101, [](int k){ return static_cast<size_t>(k); });
        cout << "Loading deliveries...\n";
        if (!load_deliveries("deliveries.txt", deliveries, delivery_db)) cerr << "Warning: Could not load deliveries\n";
        cout << "Loaded " << deliveries.size() << " deliveries\n";

        cout << "Adding deliveries to scheduler...\n";
        for (auto& del : deliveries) {
            scheduler.add_delivery(del);
        }

        cout << "Applying traffic updates...\n";
        load_traffic_updates("traffic_updates.txt", graph);
        cout << "Processing all deliveries...\n";
        scheduler.process_deliveries();

        cout << "\n=== FINAL STATISTICS ===\n";
        auto stats = scheduler.get_stats();
        cout << left << setw(25) << "Total deliveries:" << stats.total_deliveries << "\n";
        cout << setw(25) << "Assigned:" << stats.assigned << "\n";
        cout << setw(25) << "Still pending:" << stats.pending << "\n";
        cout << setw(25) << "Unassigned:" << stats.unassigned << "\n";
        cout << setw(25) << "Total load assigned:" << fixed << setprecision(2) << stats.total_load_assigned << " units\n";
        cout << "\n=== Processed Deliveries (sorted by priority & deadline) ===\n";
        auto sorted = scheduler.sorted_deliveries();
        if (sorted.empty()) {
            cout << "No deliveries were loaded or processed.\n";
        } else {
            for (const auto& del : sorted) {
                cout << "Delivery #" << setw(4) << del.id
                     << " | " << del.source_id << " " << del.dest_id
                     << " | Prio: " << del.priority
                     << " | Status: " << setw(10) << del.status
                     << " | Vehicle: " << (del.assigned_vehicle != -1 ? to_string(del.assigned_vehicle) : "None")
                     << "\n";
            }
        }

        cout << "\n=== Vehicle Status & Routes ===\n";
        int active = 0;
        for (int id : vehicle_ids) {
            auto v_opt = scheduler.get_vehicle_db().find(id);
            if (!v_opt) continue;
            const Vehicle* v = *v_opt;
            if (v->assigned_deliveries.empty() && v->route.empty()) continue;
            active++;
            cout << "Vehicle " << v->id
                 << " | Load: " << fixed << setprecision(1) << v->current_load << "/" << v->capacity
                 << " | Available: " << (v->available ? "YES" : "NO") << "\n";
            cout << " Current Position: (" << v->current_x << ", " << v->current_y << ")\n";
            if (!v->route.empty()) {
                cout << " Route: ";
                for (size_t j = 0; j < v->route.size() && j < 12; ++j) {
                    cout << v->route[j];
                    if (j < v->route.size() - 1) cout << " ";
                }
                if (v->route.size() > 12) cout << "...";
                cout << "\n";
            }
        }
        if (active == 0) cout << "No vehicles with assignments.\n";
        cout << "\n=== System finished ===\n";
        return 0;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error\n";
        return 1;
    }
}