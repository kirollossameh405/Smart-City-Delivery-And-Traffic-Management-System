#include "../include/road_network.hpp"

void RoadNetwork::add_edge(int from, int to, double weight) {
    adj[from].push_back({to, weight, weight});
}

void RoadNetwork::update_edge_weight(int from, int to, double new_weight) {
    for (auto& e : adj[from]) {
        if (e.to == to) {
            e.weight = new_weight;
            return;
        }
    }
}

bool RoadNetwork::remove_edge(int from, int to) {
    auto& edges = adj[from];
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        if (it->to == to) {
            edges.erase(it);
            return true;
        }
    }
    return false;
}

vector<int> RoadNetwork::dijkstra(int start, int goal) const {
    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    dist[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto [cost, u] = pq.top(); pq.pop();
        if (cost > dist[u]) continue;
        if (u == goal) break;
        for (const auto& e : adj.at(u)) {
            double alt = cost + e.weight;
            if (alt < dist[e.to]) {
                dist[e.to] = alt;
                prev[e.to] = u;
                pq.push({alt, e.to});
            }
        }
    }

    vector<int> path;
    for (int at = goal; at != start; at = prev[at]) {
        path.push_back(at);
        if (prev.find(at) == prev.end()) return {};
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

vector<double> RoadNetwork::bellman_ford(int start) const {
    vector<double> dist(adj.size(), numeric_limits<double>::infinity());
    dist[start] = 0.0;
    for (size_t i = 0; i < adj.size() - 1; ++i) {
        for (const auto& [u, edges] : adj) {
            for (const auto& e : edges) {
                if (dist[u] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[u] + e.weight;
                }
            }
        }
    }
    return dist;
}

void RoadNetwork::bfs(int start, vector<bool>& visited) const {
    queue<int> q;
    q.push(start);
    visited[start] = true;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (const auto& e : adj.at(u)) {
            if (!visited[e.to]) {
                visited[e.to] = true;
                q.push(e.to);
            }
        }
    }
}

void RoadNetwork::dfs(int node, vector<bool>& visited) const {
    visited[node] = true;
    for (const auto& e : adj.at(node)) {
        if (!visited[e.to]) {
            dfs(e.to, visited);
        }
    }
}

vector<pair<int, int>> RoadNetwork::kruskal_mst() const {
    vector<tuple<double, int, int>> edges;
    for (const auto& [u, es] : adj) {
        for (const auto& e : es) {
            edges.emplace_back(e.weight, u, e.to);
        }
    }
    sort(edges.begin(), edges.end());
    vector<pair<int, int>> mst;
    unordered_map<int, int> parent;
    for (const auto& [u, _] : adj) parent[u] = u;
    auto find = [&](auto& self, int x) -> int {
        return parent[x] == x ? x : parent[x] = self(self, parent[x]);
    };
    for (const auto& [w, u, v] : edges) {
        int pu = find(find, u), pv = find(find, v);
        if (pu != pv) {
            parent[pu] = pv;
            mst.emplace_back(u, v);
        }
    }
    return mst;
}

vector<int> RoadNetwork::topological_sort() const {
    vector<int> order;
    vector<bool> visited(adj.size(), false);
    for (const auto& [node, _] : adj) {
        if (!visited[node]) {
            dfs(node, visited);
            order.push_back(node);
        }
    }
    reverse(order.begin(), order.end());
    return order;
}