#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "types.hpp"
#include "priority_queue.hpp"
#include "quadtree.hpp"
#include "hash_table.hpp"
#include "road_network.hpp"
#include "delivery.hpp"
#include <vector>

using namespace std;

class Scheduler {
private:
    DeliveryPQ pending;
    HashTable<int, Delivery> delivery_db;
    HashTable<int, Vehicle> vehicle_db;
    RoadNetwork& graph;
    HashTable<int, Location> location_db;
    QuadTree qt;

public:
    Scheduler(RoadNetwork& g, HashTable<int, Location>& loc_db, double minx, double miny, double maxx, double maxy);
    void add_delivery(Delivery del);
    void process_deliveries();
    Vehicle* find_nearest_vehicle(double x, double y);
    void assign_delivery(int del_id, int veh_id);
    void update_traffic(int from, int to, double new_w);
    std::vector<Delivery> sorted_deliveries() const;
};

#endif