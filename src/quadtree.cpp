#include "../include/quadtree.hpp"
#include <cmath>

QuadNode::QuadNode(double mx, double my, double Mx, double My) : min_x(mx), min_y(my), max_x(Mx), max_y(My) {}

bool QuadNode::contains(double px, double py) const {
    return px >= min_x && px <= max_x && py >= min_y && py <= max_y;
}

QuadTree::QuadTree(double minx, double miny, double maxx, double maxy) {
    root = make_unique<QuadNode>(minx, miny, maxx, maxy);
}

void QuadTree::subdivide(QuadNode* node) {
    double mid_x = (node->min_x + node->max_x) / 2;
    double mid_y = (node->min_y + node->max_y) / 2;
    node->children[0] = make_unique<QuadNode>(node->min_x, mid_y, mid_x, node->max_y);
    node->children[1] = make_unique<QuadNode>(mid_x, mid_y, node->max_x, node->max_y);
    node->children[2] = make_unique<QuadNode>(node->min_x, node->min_y, mid_x, mid_y);
    node->children[3] = make_unique<QuadNode>(mid_x, node->min_y, node->max_x, mid_y);
    vector<Location*> temp = move(node->points);
    for (auto* loc : temp) {
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->contains(loc->x, loc->y)) {
                insert(node->children[i].get(), loc);
                break;
            }
        }
    }
}

void QuadTree::insert(QuadNode* node, Location* loc) {
    if (!node->contains(loc->x, loc->y)) return;
    if (node->children[0]) {
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->contains(loc->x, loc->y)) {
                insert(node->children[i].get(), loc);
                return;
            }
        }
    }
    node->points.push_back(loc);
    if (node->points.size() > QuadNode::CAPACITY && !node->children[0]) {
        subdivide(node);
    }
}

void QuadTree::insert(Location* loc) {
    insert(root.get(), loc);
}

void QuadTree::query(QuadNode* node, double x, double y, double radius, vector<Location*>& result) const {
    if (!node) return;
    for (auto* loc : node->points) {
        double dx = loc->x - x, dy = loc->y - y;
        if (dx*dx + dy*dy <= radius*radius) result.push_back(loc);
    }
    if (node->children[0]) {
        for (int i = 0; i < 4; ++i) {
            query(node->children[i].get(), x, y, radius, result);
        }
    }
}

vector<Location*> QuadTree::query_radius(double x, double y, double radius) const {
    vector<Location*> result;
    query(root.get(), x, y, radius, result);
    return result;
}

Location* QuadTree::nearest(QuadNode* node, double x, double y, Location* best, double& best_dist) const {
    if (!node) return best;
    for (auto* loc : node->points) {
        double dx = loc->x - x, dy = loc->y - y;
        double dist = dx*dx + dy*dy;
        if (dist < best_dist) {
            best_dist = dist;
            best = loc;
        }
    }
    if (node->children[0]) {
        for (int i = 0; i < 4; ++i) {
            best = nearest(node->children[i].get(), x, y, best, best_dist);
        }
    }
    return best;
}

Location* QuadTree::find_nearest(double x, double y) const {
    double best_dist = numeric_limits<double>::infinity();
    return nearest(root.get(), x, y, nullptr, best_dist);
}