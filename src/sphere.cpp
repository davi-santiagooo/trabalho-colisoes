#include "sphere.hpp"

#include <cmath>
#include <vector>

void col_CreateUvSphere(int sectors, int layers, float radius, std::vector<float>& points, std::vector<int>& indices) {
    float x, y, z = 0;
    int i = 0;

    /* {points} vector */
    // First point on top
    points.push_back(0);
    points.push_back(radius);
    points.push_back(0);
    i++;
    
    // Points in middle
    int layer, sector;
    float layerRadius;
    for(layer = 1; layer < layers - 1; layer++) {
        y = radius - layer * 2*radius/(layers - 1);
        layerRadius = std::sqrt(radius*radius - y*y);

        // Append all points of the layer to {points} vector
        for(sector = 0; sector < sectors; sector++) {
            float angle = sector * 2*M_PI/sectors;

            x = layerRadius * std::cos(angle); 
            z = layerRadius * std::sin(angle);
            
            points.push_back(x);
            points.push_back(y);
            points.push_back(z);
            i++;
        }
    }

    // Last point on bottom
    points.push_back(0);
    points.push_back(-radius);
    points.push_back(0);

    
    /* {incices} vector */

    // Top triangles
    for(i = 1; i <= sectors; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i%sectors + 1);
    }

    // Middle triangles
    int l1, l2;
    sector = 1;
    l1 = 1;
    l2 = sectors + 1;
    for(layer = 1; layer < layers - 1; layer++) {
        for(sector = 1; sector < sectors; sector++) {
            indices.push_back(l1 + sector);
            indices.push_back(l1 + (sector + 1)%sectors + 1);
            indices.push_back(l2 + sector);

            indices.push_back(l1 + sector);
            indices.push_back(l2 + sector);
            indices.push_back(l2 + sector + 1);
        }

        l1 += sectors;
        l2 += sectors;
    }

    // Bottom triangles
    int last_pi = points.size()/3 - 1;
    int last_layer = (layers - 3) * sectors + 1;
    for(i = 0; i < sectors; i++) {
        indices.push_back(last_pi);
        indices.push_back(last_layer + i);
        indices.push_back(last_layer + (i+1)%sectors);
    }
}
