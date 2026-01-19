#include "../include/road_network.hpp"
#include <functional>

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
        auto [cost, u] = pq.top(); 
        pq.pop();

        if (cost > dist[u]) continue;

        if (u == goal) break;

        auto it = adj.find(u);
        if (it == adj.end()) continue;

        for (const auto& e : it->second) {
            double alt = cost + e.weight;

            auto dist_it = dist.find(e.to);
            double current_dist = (dist_it == dist.end()) 
                                ? numeric_limits<double>::infinity() 
                                : dist_it->second;

            if (alt < current_dist) {
                dist[e.to] = alt;
                prev[e.to] = u;
                pq.push({alt, e.to});
            }
        }
    }

    vector<int> path;
    if (prev.find(goal) == prev.end() && start != goal) {
        return {};
    }

    for (int at = goal; at != start; at = prev[at]) {
        path.push_back(at);
        if (prev.find(at) == prev.end()) {
            return {};
        }
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

vector<double> RoadNetwork::bellman_ford(int start) const {
    unordered_map<int, double> dist;

    for (const auto& [u, _] : adj) {
        dist[u] = numeric_limits<double>::infinity();
    }

    for (const auto& [u, edges] : adj) {
        for (const auto& e : edges) {
            dist[e.to];
        }
    }

    dist[start] = 0.0;

    for (size_t i = 0; i < dist.size() - 1; ++i) {
        bool changed = false;
        for (const auto& [u, edges] : adj) {
            if (dist[u] == numeric_limits<double>::infinity()) continue;
            for (const auto& e : edges) {
                if (dist[u] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[u] + e.weight;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }

    vector<double> result;
    result.reserve(dist.size());
    for (const auto& [node, d] : dist) {
        result.push_back(d);
    }

    return result;
}

void RoadNetwork::bfs(int start, unordered_set<int>& visited) const {
    queue<int> q;
    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        auto it = adj.find(u);
        if (it == adj.end()) continue;
        for (const auto& e : it->second) {
            if (!visited.count(e.to)) {
                visited.insert(e.to);
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
    unordered_map<int, bool> visited;
    unordered_map<int, bool> rec_stack;

    function<void(int)> dfs_visit = [&](int node) {
        visited[node] = true;
        rec_stack[node] = true;

        auto it = adj.find(node);
        if (it != adj.end()) {
            for (const auto& e : it->second) {
                if (!visited[e.to]) {
                    dfs_visit(e.to);
                } else if (rec_stack[e.to]) {
                }
            }
        }

        rec_stack[node] = false;
        order.push_back(node);
    };

    for (const auto& [node, _] : adj) {
        if (!visited[node]) {
            dfs_visit(node);
        }
    }

    reverse(order.begin(), order.end());
    return order;
}

const unordered_map<int, vector<Edge>>& RoadNetwork::get_adj() const {
    return adj;
}