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
    unordered_map<int, Vehicle*> available_vehicles;

public:
    Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db,
              double minx, double miny, double maxx, double maxy);

    void add_delivery(Delivery del);
    void add_vehicle(int id, Vehicle veh);
    void add_location_to_quadtree(Location* loc);
    void update_vehicle_availability(int veh_id, bool available);
    Vehicle* find_nearest_vehicle(double x, double y);
    void assign_delivery(int del_id, int veh_id);
    void process_deliveries();
    void update_traffic(int from, int to, double new_w);
    vector<Delivery> sorted_deliveries() const;
    HashTable<int, Vehicle>& get_vehicle_db();
    const HashTable<int, Vehicle>& get_vehicle_db() const;
};

#endif