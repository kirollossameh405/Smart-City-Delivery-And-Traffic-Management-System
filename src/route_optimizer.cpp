#include "../include/route_optimizer.hpp"
#include <algorithm>
#include <limits>

vector<int> greedy_route(const RoadNetwork& graph, int start, const vector<int>& destinations) {
    vector<int> remaining = destinations;
    vector<int> path {start};
    int current = start;
    while (!remaining.empty()) {
        double min_dist = numeric_limits<double>::infinity();
        int next = -1;
        auto it = remaining.begin();
        for (auto jt = remaining.begin(); jt != remaining.end(); ++jt) {
            auto p = graph.dijkstra(current, *jt);
            if (!p.empty()) {
                double cost = 0.0;
                
                if (p.size() - 1 < min_dist) {
                    min_dist = p.size() - 1;
                    next = *jt;
                    it = jt;
                }
            }
        }
        if (next == -1) break;
        path.push_back(next);
        remaining.erase(it);
        current = next;
    }
    return path;
}

double route_cost(const RoadNetwork& graph, const vector<int>& path) {
    double cost = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        auto subpath = graph.dijkstra(path[i], path[i+1]);
        cost += subpath.size() - 1;
    }
    return cost;
}

vector<vector<int>> partition_deliveries(const vector<Delivery>& deliveries, int num_vehicles) {
    vector<Delivery> sorted = deliveries;
    sort(sorted.begin(), sorted.end(), [](const Delivery& a, const Delivery& b){
        return a.dest_id < b.dest_id;
    });
    vector<vector<int>> partitions(num_vehicles);
    for (size_t i = 0; i < sorted.size(); ++i) {
        partitions[i % num_vehicles].push_back(sorted[i].id);
    }
    return partitions;
}