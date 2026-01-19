#ifndef ROUTE_OPTIMIZER_HPP
#define ROUTE_OPTIMIZER_HPP

#include "types.hpp"
#include "road_network.hpp"
#include <vector>

using namespace std;

vector<int> greedy_route(const RoadNetwork& graph, int start, const vector<int>& destinations);
double route_cost(const RoadNetwork& graph, const vector<int>& path);
vector<vector<int>> partition_deliveries(const vector<Delivery>& deliveries, int num_vehicles);

#endif