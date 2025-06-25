#include "ray_trace.h"

bool BoundingBox::intersect(const Vector& ray_origin, const Vector& ray_end) const {
    Vector dir = ray_end - ray_origin;

    // Branch-free version using conditional moves
    float inv_dir_x = 1.0f / (dir.x + (dir.x == 0.0f ? 1e-8f : 0.0f));
    float inv_dir_y = 1.0f / (dir.y + (dir.y == 0.0f ? 1e-8f : 0.0f));
    float inv_dir_z = 1.0f / (dir.z + (dir.z == 0.0f ? 1e-8f : 0.0f));

    float tx1 = (min.x - ray_origin.x) * inv_dir_x;
    float tx2 = (max.x - ray_origin.x) * inv_dir_x;
    float ty1 = (min.y - ray_origin.y) * inv_dir_y;
    float ty2 = (max.y - ray_origin.y) * inv_dir_y;
    float tz1 = (min.z - ray_origin.z) * inv_dir_z;
    float tz2 = (max.z - ray_origin.z) * inv_dir_z;

    float tmin = fmaxf(fmaxf(fminf(tx1, tx2), fminf(ty1, ty2)), fminf(tz1, tz2));
    float tmax = fminf(fminf(fmaxf(tx1, tx2), fmaxf(ty1, ty2)), fmaxf(tz1, tz2));

    return tmax >= 0 && tmin <= tmax && tmin <= 1.0f;
}

bool TriangleW::intersect(Vector ray_origin, Vector ray_end) const {
    const float EPSILON = 0.0000001f;
    Vector edge1, edge2, h, s, q;
    float a, f, u, v, t;
    edge1 = p2 - p1;
    edge2 = p3 - p1;
    h = CrossProduct(ray_end - ray_origin, edge2);
    a = edge1.Dot(h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1.0 / a;
    s = ray_origin - p1;
    u = f * s.Dot(h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = CrossProduct(s, edge1);
    v = f * (ray_end - ray_origin).Dot(q);

    if (v < 0.0 || u + v > 1.0)
        return false;

   
    t = f * edge2.Dot(q);

    if (t > EPSILON && t < 1.0)
        return true;

    return false;
}

void KDNode::deleteKDTree(KDNode* node) {
    if (node == nullptr) return;

    
    deleteKDTree(node->left);
    deleteKDTree(node->right);

    
    delete node;
}

bool rayIntersectsKDTree(KDNode* node, const Vector& ray_origin, const Vector& ray_end) {
    if (node == nullptr) return false;

    if (!node->bbox.intersect(ray_origin, ray_end)) {
        return false;
    }

    // Leaf node - check triangles
    if (node->triangle.size() > 0) {
        for (const auto& tri : node->triangle) {
            if (tri.intersect(ray_origin, ray_end)) {
                return true; // Early exit on first hit
            }
        }
        return false;
    }

    // Internal node - traverse children with early exit
    if (rayIntersectsKDTree(node->left, ray_origin, ray_end)) {
        return true; // Early exit - don't check right child
    }
    return rayIntersectsKDTree(node->right, ray_origin, ray_end);
}

BoundingBox calculateBoundingBox(const std::vector<TriangleW>& triangles) {
    if (triangles.empty()) {
        // Return a default bounding box for empty input
        return { {0, 0, 0}, {0, 0, 0} };
    }

    BoundingBox box;
    
    box.min = box.max = triangles[0].p1;
    for (const auto& tri : triangles) {
        for (const auto& p : { tri.p1, tri.p2, tri.p3 }) {
            box.min.x = std::min(box.min.x, p.x);
            box.min.y = std::min(box.min.y, p.y);
            box.min.z = std::min(box.min.z, p.z);
            box.max.x = std::max(box.max.x, p.x);
            box.max.y = std::max(box.max.y, p.y);
            box.max.z = std::max(box.max.z, p.z);
        }
    }
    return box;
}

KDNode* buildKDTree(std::vector<TriangleW>& triangles, int depth) {
    if (triangles.empty()) return nullptr;

    KDNode* node = new KDNode();
    node->bbox = calculateBoundingBox(triangles);
    node->axis = depth % 3; 

    if (triangles.size() <= 3) {
        node->triangle = triangles;
        return node;
    }

    auto comparator = [axis = node->axis](const TriangleW& a, const TriangleW& b) {
        
        float a_center, b_center;
        switch (axis) {
        case 0:
            a_center = (a.p1.x + a.p2.x + a.p3.x) / 3;
            b_center = (b.p1.x + b.p2.x + b.p3.x) / 3;
            break;
        case 1:
            a_center = (a.p1.y + a.p2.y + a.p3.y) / 3;
            b_center = (b.p1.y + b.p2.y + b.p3.y) / 3;
            break;
        case 2:
            a_center = (a.p1.z + a.p2.z + a.p3.z) / 3;
            b_center = (b.p1.z + b.p2.z + b.p3.z) / 3;
            break;
        }
        return a_center < b_center;
        };

    std::nth_element(triangles.begin(), triangles.begin() + triangles.size() / 2, triangles.end(), comparator);

    std::vector<TriangleW> left_triangles(triangles.begin(), triangles.begin() + triangles.size() / 2);
    std::vector<TriangleW> right_triangles(triangles.begin() + triangles.size() / 2, triangles.end());

    node->left = buildKDTree(left_triangles, depth + 1);
    node->right = buildKDTree(right_triangles, depth + 1);

    return node;
}

void map_loader::unload() {
    if (kd_tree) {
        kd_tree->deleteKDTree(kd_tree);
        kd_tree = nullptr;
    }
}

bool map_loader::load_map(std::string map_name) {
    std::cout << "[DEBUG] Attempting to load map: " << map_name << ".tri" << std::endl;
    auto begin = std::chrono::steady_clock::now();

    // Clean up existing tree if any
    if (kd_tree) {
        std::cout << "[DEBUG] Cleaning up existing KD tree" << std::endl;
        unload();
    }

    std::ifstream in(map_name + ".tri", std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "[ERROR] Failed to open file: " << map_name << ".tri" << std::endl;
        return false;
    }

    in.seekg(0, std::ios::end);
    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::cout << "[DEBUG] File size: " << fileSize << " bytes" << std::endl;
    std::cout << "[DEBUG] Expected triangle size: " << sizeof(TriangleW) << " bytes" << std::endl;

    if (fileSize == 0) {
        std::cerr << "[ERROR] File is empty: " << map_name << ".tri" << std::endl;
        return false;
    }

    if (fileSize % sizeof(TriangleW) != 0) {
        std::cerr << "[ERROR] Invalid file size: " << fileSize << " bytes (not divisible by " << sizeof(TriangleW) << ")" << std::endl;
        return false;
    }

    std::size_t num_elements = fileSize / sizeof(TriangleW);
    std::cout << "[DEBUG] Number of triangles to load: " << num_elements << std::endl;

    triangles.resize(num_elements);

    if (!in.read(reinterpret_cast<char*>(triangles.data()), fileSize)) {
        std::cerr << "[ERROR] Failed to read file: " << map_name << ".tri" << std::endl;
        return false;
    }

    in.close();
    std::cout << "[DEBUG] File read successfully" << std::endl;

    if (triangles.empty()) {
        std::cerr << "[ERROR] No triangles loaded from file" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] Building KD tree..." << std::endl;
    kd_tree = buildKDTree(triangles);

    if (!kd_tree) {
        std::cerr << "[ERROR] Failed to build KD tree" << std::endl;
        return false;
    }

    std::cout << "[DEBUG] KD tree built successfully" << std::endl;
    std::vector<TriangleW>().swap(triangles); // Clear memory

    auto i_end = std::chrono::steady_clock::now();
    std::cout << "[MAP] Loaded {" << map_name << "} "
        << std::chrono::duration<double, std::milli>(i_end - begin).count()
        << "ms (" << num_elements << " triangles)" << std::endl;
    return true;
}

bool map_loader::is_visible(Vector ray_origin, Vector ray_end) const {
    if (!kd_tree) {
        std::cerr << "[ERROR] KD tree not initialized! Call load_map() first." << std::endl;
        return false;
    }

    // Validate input vectors
    if (std::isnan(ray_origin.x) || std::isnan(ray_origin.y) || std::isnan(ray_origin.z) ||
        std::isnan(ray_end.x) || std::isnan(ray_end.y) || std::isnan(ray_end.z)) {
        std::cerr << "[ERROR] Invalid vector data (NaN values)" << std::endl;
        return false;
    }

    return !rayIntersectsKDTree(kd_tree, ray_origin, ray_end);
}