#include "../include/quadtree.hpp"
#include <cmath>
#include <limits>
#include <iostream>

QuadNode::QuadNode(double mx, double my, double Mx, double My) 
    : min_x(mx), min_y(my), max_x(Mx), max_y(My) {}

bool QuadNode::contains(double px, double py) const {
    return px >= min_x && px <= max_x && py >= min_y && py <= max_y;
}

QuadTree::QuadTree(double minx, double miny, double maxx, double maxy) {
    root = make_unique<QuadNode>(minx, miny, maxx, maxy);
}

void QuadTree::subdivide(QuadNode* node) {
    if (node->children[0]) return;

    double mid_x = (node->min_x + node->max_x) / 2.0;
    double mid_y = (node->min_y + node->max_y) / 2.0;

    if (node->max_x - node->min_x < 0.01 || node->max_y - node->min_y < 0.01) {
        return;
    }

    node->children[0] = make_unique<QuadNode>(node->min_x, mid_y, mid_x, node->max_y);
    node->children[1] = make_unique<QuadNode>(mid_x, mid_y, node->max_x, node->max_y);
    node->children[2] = make_unique<QuadNode>(node->min_x, node->min_y, mid_x, mid_y);
    node->children[3] = make_unique<QuadNode>(mid_x, node->min_y, node->max_x, mid_y);

    auto temp = move(node->items);
    node->items.clear();

    for (auto& item : temp) {
        bool placed = false;
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->contains(item.first->x, item.first->y)) {
                insert(node->children[i].get(), item.first, item.second);
                placed = true;
                break;
            }
        }
        if (!placed) node->items.push_back(item);
    }
}

void QuadTree::insert(QuadNode* node, Location* loc, Vehicle* veh) {
    if (!node || !loc) return;
    if (!node->contains(loc->x, loc->y)) return;

    if (node->children[0]) {
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->contains(loc->x, loc->y)) {
                insert(node->children[i].get(), loc, veh);
                return;
            }
        }
        node->items.emplace_back(loc, veh);
    } else {
        node->items.emplace_back(loc, veh);
        if (node->items.size() > static_cast<size_t>(QuadNode::CAPACITY)) {
            subdivide(node);
        }
    }
}

void QuadTree::insert_location(Location* loc) {
    insert(root.get(), loc, nullptr);
}

void QuadTree::insert_vehicle(Vehicle* veh) {
    insert(root.get(), &veh->current_pos, veh);
}

void QuadTree::update_vehicle_position(int veh_id, double new_x, double new_y) {
}

vector<pair<Location*, Vehicle*>> QuadTree::query_radius(double x, double y, double radius) const {
    vector<pair<Location*, Vehicle*>> result;
    query(root.get(), x, y, radius, result);
    return result;
}

void QuadTree::query(QuadNode* node, double x, double y, double radius,
                     vector<pair<Location*, Vehicle*>>& result) const {
    if (!node) return;

    for (const auto& item : node->items) {
        double dx = item.first->x - x;
        double dy = item.first->y - y;
        if (dx*dx + dy*dy <= radius*radius) {
            result.push_back(item);
        }
    }

    if (node->children[0]) {
        for (int i = 0; i < 4; ++i) {
            query(node->children[i].get(), x, y, radius, result);
        }
    }
}

pair<Location*, Vehicle*> QuadTree::find_nearest_vehicle(double x, double y) const {
    pair<Location*, Vehicle*> best = {nullptr, nullptr};
    double best_dist = numeric_limits<double>::infinity();

    vector<pair<Location*, Vehicle*>> candidates = query_radius(x, y, 100000.0);

    for (const auto& [loc, veh] : candidates) {
        if (!veh || !veh->available) continue;
        double dx = veh->current_x - x;
        double dy = veh->current_y - y;
        double dist = dx*dx + dy*dy;
        if (dist < best_dist) {
            best_dist = dist;
            best = {loc, veh};
        }
    }

    return best;
}