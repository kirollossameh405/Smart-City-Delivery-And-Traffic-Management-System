#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "types.hpp"
#include "priority_queue.hpp"
#include "quadtree.hpp"
#include "hash_table.hpp"
#include "road_network.hpp"
#include "delivery.hpp"
#include "route_optimizer.hpp"
#include <vector>
#include <unordered_map>
#include <optional>

using namespace std;

class Scheduler {
private:
    RoadNetwork& graph;
    HashTable<int, Location>& location_db;
    QuadTree location_qt;
    QuadTree vehicle_qt;
    HashTable<int, Delivery> delivery_db;
    HashTable<int, Vehicle> vehicle_db;
    DeliveryPQ pending;

public:
    Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db,
              double minx, double miny, double maxx, double maxy);

    void add_delivery(Delivery del);
    void add_vehicle(Vehicle veh);
    void add_location_to_quadtree(Location* loc);

    void update_vehicle_position(int veh_id, double new_x, double new_y);

    pair<Location*, Vehicle*> find_nearest_vehicle(double x, double y);
    void assign_delivery(int del_id, int veh_id);
    void process_deliveries();

    void update_traffic(int from, int to, double new_weight);

    vector<Delivery> sorted_deliveries() const;
    HashTable<int, Vehicle>& get_vehicle_db();
    const HashTable<int, Vehicle>& get_vehicle_db() const;

    struct Stats {
        int total_deliveries = 0;
        int assigned = 0;
        int pending = 0;
        int unassigned = 0;
        double total_load_assigned = 0.0;
    };
    Stats get_stats() const;
};

#endif