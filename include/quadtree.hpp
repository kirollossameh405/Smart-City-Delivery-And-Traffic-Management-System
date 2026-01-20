#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "types.hpp"
#include <vector>
#include <memory>
#include <optional>

using namespace std;

struct QuadNode {
    double min_x, min_y, max_x, max_y;
    vector<pair<Location*, Vehicle*>> items;
    unique_ptr<QuadNode> children[4];
    static constexpr int CAPACITY = 4;

    QuadNode(double mx, double my, double Mx, double My);
    bool contains(double px, double py) const;
};

class QuadTree {
private:
    unique_ptr<QuadNode> root;

    void insert(QuadNode* node, Location* loc, Vehicle* veh = nullptr);
    void subdivide(QuadNode* node);
    void query(QuadNode* node, double x, double y, double radius, 
               vector<pair<Location*, Vehicle*>>& result) const;

public:
    QuadTree(double minx, double miny, double maxx, double maxy);
    void insert_location(Location* loc);
    void insert_vehicle(Vehicle* veh);
    void update_vehicle_position(int veh_id, double new_x, double new_y);
    vector<pair<Location*, Vehicle*>> query_radius(double x, double y, double radius) const;
    pair<Location*, Vehicle*> find_nearest_vehicle(double x, double y) const;
};

#endif