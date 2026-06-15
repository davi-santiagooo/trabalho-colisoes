#include "physics.hpp"

#include <cmath>
#include <vector>

void col_CreateUvSphere(int sectors, int layers, float radius, std::vector<float>& points, std::vector<int>& indices, std::vector<float>& normals) {
    float x, y, z;
    float r, angle;

    points.reserve((layers - 2)*sectors + 2);
    indices.reserve(6*sectors*(layers - 1));

    /* Points & Normals*/
    points.push_back(0.0f); points.push_back(-radius); points.push_back(0.0f);
    normals.push_back(0.0f); normals.push_back(-1); normals.push_back(0.0f);
    
    int lay, sec;
    for(lay = 1; lay < layers; lay++) {
        y = radius * std::sin(lay * M_PI / layers + 3*M_PI/2);
        r = sqrt(radius*radius - y*y);
        
        for(sec = 0; sec < sectors; sec++) {
            angle = sec*(2*M_PI/sectors);
            x = r*cos((double)angle);
            z = r*sin((double)angle);
            
            points.push_back(x);
            points.push_back(y);
            points.push_back(z);

            normals.push_back(x/radius);
            normals.push_back(y/radius);
            normals.push_back(z/radius);
        }
    }

    points.push_back(0.0f); points.push_back(radius); points.push_back(0.0f);
    normals.push_back(0.0f); normals.push_back(1); normals.push_back(0.0f);

    /* Indices */
    for(sec = 0; sec < sectors; sec++) {
        indices.push_back(0);
        indices.push_back(sec+1);
        indices.push_back((sec+1)%sectors + 1);
    }

    int p1, p2, p3, p4;
    for(lay = 1; lay < layers - 1; lay++) {
        for(sec = 0; sec < sectors; sec++) {
            p1 = (lay - 1)*sectors + sec + 1;
            p2 = (lay - 1)*sectors + (sec + 1)%sectors + 1;
            p3 = p1 + sectors;
            p4 = p2 + sectors;
            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p4);
            indices.push_back(p1);
            indices.push_back(p3);
            indices.push_back(p4);
        }
    }

    int last_i = points.size()/3-1;
    for(sec = 0; sec < sectors; sec++) {
        indices.push_back(last_i);
        indices.push_back(last_i-1 - sec);
        indices.push_back(last_i-1 - (sec + 1)%sectors);
    }
}
