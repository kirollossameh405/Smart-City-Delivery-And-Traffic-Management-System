#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "types.hpp"
#include <vector>
#include <memory>

using namespace std;

struct QuadNode {
    double min_x, min_y, max_x, max_y;
    vector<Location*> points;
    unique_ptr<QuadNode> children[4];
    static constexpr int CAPACITY = 4;

    QuadNode(double mx, double my, double Mx, double My);
    bool contains(double px, double py) const;
};

class QuadTree {
private:
    unique_ptr<QuadNode> root;

    void insert(QuadNode* node, Location* loc);
    void subdivide(QuadNode* node);
    void query(QuadNode* node, double x, double y, double radius, vector<Location*>& result) const;
    Location* nearest(QuadNode* node, double x, double y, Location* best, double& best_dist) const;

public:
    QuadTree(double minx, double miny, double maxx, double maxy);
    void insert(Location* loc);
    vector<Location*> query_radius(double x, double y, double radius) const;
    Location* find_nearest(double x, double y) const;
};

#endif