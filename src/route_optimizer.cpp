#include "../include/route_optimizer.hpp"
#include <algorithm>
#include <limits>

vector<int> greedy_route(const RoadNetwork& graph, int start, const std::vector<int>& destinations) {
    vector<int> remaining = destinations;
    vector<int> path{start};
    int current = start;

    while (!remaining.empty()) {
        double min_cost = numeric_limits<double>::infinity();
        int next = -1;
        auto best_it = remaining.end();

        for (auto it = remaining.begin(); it != remaining.end(); ++it) {
            auto subpath = graph.dijkstra(current, *it);
            if (subpath.empty()) continue;

            double cost = 0.0;
            for (size_t k = 0; k < subpath.size() - 1; ++k) {
                int u = subpath[k];
                int v = subpath[k + 1];
                auto adj_it = graph.get_adj().find(u);
                if (adj_it == graph.get_adj().end()) continue;
                for (const auto& e : adj_it->second) {
                    if (e.to == v) {
                        cost += e.weight;
                        break;
                    }
                }
            }

            if (cost < min_cost) {
                min_cost = cost;
                next = *it;
                best_it = it;
            }
        }

        if (next == -1) break;

        path.push_back(next);
        remaining.erase(best_it);
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