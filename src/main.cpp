#include "../include/road_network.hpp"
#include "../include/hash_table.hpp"
#include "../include/quadtree.hpp"
#include "../include/scheduler.hpp"
#include "../include/file_io.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <vector>

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

    if (!load_vehicles("vehicles.txt", vehicle_db, loc_db)) {
        cerr << "Failed to load vehicles" << endl;
        return 1;
    }

    if (!load_deliveries("deliveries.txt", deliveries, delivery_db)) {
        cerr << "Failed to load deliveries" << endl;
        return 1;
    }

    for (auto del : deliveries) scheduler.add_delivery(del);

    load_traffic_updates("traffic_updates.txt", graph);

    scheduler.process_deliveries();

    auto sorted = scheduler.sorted_deliveries();
    for (const auto& del : sorted) {
        cout << "Delivery " << del.id << " to " << del.dest_id << endl;
    }

    return 0;
}