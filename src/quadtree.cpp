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
    if (node->children[0]) return;  // Already subdivided

    double mid_x = (node->min_x + node->max_x) / 2.0;
    double mid_y = (node->min_y + node->max_y) / 2.0;
    
    // Check if subdivision is meaningful
    if (node->max_x - node->min_x < 0.01 || node->max_y - node->min_y < 0.01) {
        return;  // Too small to subdivide further
    }

    node->children[0] = make_unique<QuadNode>(node->min_x, mid_y, mid_x, node->max_y);   // NW
    node->children[1] = make_unique<QuadNode>(mid_x, mid_y, node->max_x, node->max_y);   // NE
    node->children[2] = make_unique<QuadNode>(node->min_x, node->min_y, mid_x, mid_y);   // SW
    node->children[3] = make_unique<QuadNode>(mid_x, node->min_y, node->max_x, mid_y);   // SE

    vector<Location*> temp = move(node->points);
    node->points.clear();

    for (auto* loc : temp) {
        if (!loc) continue;  // Skip null pointers
        
        bool placed = false;
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->contains(loc->x, loc->y)) {
                insert(node->children[i].get(), loc);
                placed = true;
                break;
            }
        }
        if (!placed) {
            node->points.push_back(loc);  // Keep at current level if doesn't fit children
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
        node->points.push_back(loc);
    } else {
        node->points.push_back(loc);
        if (node->points.size() > QuadNode::CAPACITY) {
            subdivide(node);
        }
    }
}

void QuadTree::insert(Location* loc) {
    insert(root.get(), loc);
}

void QuadTree::query(QuadNode* node, double x, double y, double radius, vector<Location*>& result) const {
    if (!node) return;

    vector<QuadNode*> node_stack;
    node_stack.push_back(node);

    while (!node_stack.empty()) {
        QuadNode* current = node_stack.back();
        node_stack.pop_back();

        for (auto* loc : current->points) {
            double dx = loc->x - x;
            double dy = loc->y - y;
            if (dx * dx + dy * dy <= radius * radius) {
                result.push_back(loc);
            }
        }

        if (current->children[0]) {
            for (int i = 0; i < 4; ++i) {
                node_stack.push_back(current->children[i].get());
            }
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
    if (!root) return nullptr;

    Location* best = nullptr;
    double best_dist = numeric_limits<double>::infinity();

    vector<QuadNode*> node_stack;
    node_stack.push_back(root.get());

    while (!node_stack.empty()) {
        QuadNode* node = node_stack.back();
        node_stack.pop_back();

        for (auto* loc : node->points) {
            double dx = loc->x - x;
            double dy = loc->y - y;
            double dist = dx * dx + dy * dy;
            if (dist < best_dist) {
                best_dist = dist;
                best = loc;
            }
        }

        if (node->children[0]) {
            for (int i = 0; i < 4; ++i) {
                node_stack.push_back(node->children[i].get());
            }
        }
    }

    return best;
}