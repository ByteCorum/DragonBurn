#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include "vector.h"
#undef min()
#undef max()

// credits tni & learn_more (www.unknowncheats.me/forum/3868338-post34.html)
#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define get_byte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

struct BoundingBox {
    Vector min, max;
    bool intersect(const Vector& ray_origin, const Vector& ray_end) const;
};

struct TriangleW {
    Vector p1, p2, p3;
    bool intersect(Vector ray_origin, Vector ray_end) const;
};

struct KDNode {
    BoundingBox bbox;
    std::vector<TriangleW> triangle;
    KDNode* left = nullptr;
    KDNode* right = nullptr;
    int axis;

    void deleteKDTree(KDNode* node);
};

// Function declarations
bool rayIntersectsKDTree(KDNode* node, const Vector& ray_origin, const Vector& ray_end);
BoundingBox calculateBoundingBox(const std::vector<TriangleW>& triangles);
KDNode* buildKDTree(std::vector<TriangleW>& triangles, int depth = 0);

class map_loader {
public:
    std::vector<TriangleW> triangles;
    KDNode* kd_tree = nullptr;

    void unload();
    bool load_map(std::string map_name);
    bool is_visible(Vector ray_origin, Vector ray_end) const;
};