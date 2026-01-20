#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include "types.hpp"
#include "road_network.hpp"
#include "hash_table.hpp"
#include <string>
#include <vector>

using namespace std;

bool load_city_map(const string& filename, RoadNetwork& graph);
bool load_locations(const string& filename, HashTable<int, Location>& loc_db, vector<Location*>& all_locs);
bool load_vehicles(const string& filename, HashTable<int, Vehicle>& vehicle_db, HashTable<int, Location>& loc_db, vector<int>& vehicle_ids);
bool load_deliveries(const string& filename, vector<Delivery>& deliveries, HashTable<int, Delivery>& delivery_db);
bool load_traffic_updates(const string& filename, RoadNetwork& graph);

#endif