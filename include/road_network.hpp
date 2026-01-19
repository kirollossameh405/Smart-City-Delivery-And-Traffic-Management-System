#ifndef ROAD_NETWORK_HPP
#define ROAD_NETWORK_HPP

#include "types.hpp"
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <utility>
#include <set>

using namespace std;

class RoadNetwork {
private:
    unordered_map<int, vector<Edge>> adj;

public:
    void add_edge(int from, int to, double weight);
    void update_edge_weight(int from, int to, double new_weight);
    bool remove_edge(int from, int to);
    vector<int> dijkstra(int start, int goal) const;
    vector<double> bellman_ford(int start) const;
    void bfs(int start, vector<bool>& visited) const;
    void dfs(int node, vector<bool>& visited) const;
    vector<pair<int, int>> kruskal_mst() const;
    vector<int> topological_sort() const;
    const unordered_map<int, vector<Edge>>& get_adj() const;
};

#endif